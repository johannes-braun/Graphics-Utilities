#include <random>
#include <jpu/memory>
#include <jpu/data>
#include <jpu/log>
#include "io/window.hpp"
#include "opengl/framebuffer.hpp"
#include "io/camera.hpp"
#include "res/geometry.hpp"
#include "jpu/impl/geometry/bvh/bvh.hpp"
#include "res/image.hpp"
#include "opengl/query.hpp"

#include "tinyfd/tinyfiledialogs.h"
#include "opengl/pipeline.hpp"
#include "opengl/texture.hpp"
#include "opengl/buffer.hpp"
#include "opengl/vertex_array.hpp"
#include "stb_image_write.h"
#include "res/presets.hpp"
#include "framework/renderer.hpp"

glm::ivec2 resolution{ 1280, 720 };
jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::compute_pipeline>> compute_pipelines;
std::unique_ptr<gfx::renderer> main_renderer;

jpu::ref_ptr<gl::framebuffer> target_framebuffer;
std::array<jpu::ref_ptr<gl::texture>, 3> target_textures;
jpu::ref_ptr<gl::image> target_image;

struct mesh;
class mesh_proxy : public jpu::ref_count
{
public:
    friend mesh;
    mesh_proxy(const res::mesh& mesh)
    {
        std::vector<uint32_t> indices = mesh.indices;
        jpu::bvh<3> bvh;
        bvh.assign_to(indices, mesh.vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
        _vertex_buffer = jpu::make_ref<gl::buffer>(mesh.vertices);
        _index_buffer = jpu::make_ref<gl::buffer>(indices);
        _bvh_buffer = jpu::make_ref<gl::buffer>(bvh.pack());
        _vao = jpu::make_ref<gl::vertex_array>();
        _vao->set_element_buffer(*_index_buffer);
        _vao->add_binding(gl::vertex_attribute_binding(0, *_vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false));
        _transform = mesh.transform;
        _idx_count = indices.size();
    }

    mesh_proxy(const std::vector<res::mesh>& meshes)
    {
        std::vector<uint32_t> indices;
        std::vector<res::vertex> vertices;

        int base_offset = 0;
        int index_offset = 0;
        for(int i=0; i<meshes.size(); ++i)
        {
            indices.resize(meshes[i].indices.size() + indices.size());
            vertices.resize(meshes[i].vertices.size() + vertices.size());

            const glm::mat4 model = static_cast<glm::mat4>(meshes[i].transform);
            const glm::mat4 normal_matrix = transpose(inverse(model));

#pragma omp parallel for
            for(int vtx = 0; vtx < meshes[i].vertices.size(); ++vtx)
            {
                vertices[base_offset + vtx].position = model * meshes[i].vertices[vtx].position;
                vertices[base_offset + vtx].normal = normalize(normal_matrix * meshes[i].vertices[vtx].normal);
                vertices[base_offset + vtx].uv = meshes[i].vertices[vtx].uv;
                vertices[base_offset + vtx].uv.z = i;   // mesh material sub-index-offset
            }

#pragma omp parallel for
            for(int idx = 0; idx < meshes[i].indices.size(); ++idx)
            {
                indices[idx + index_offset] = meshes[i].indices[idx] + base_offset;
            }
            index_offset += meshes[i].indices.size();
            base_offset += meshes[i].vertices.size();
        }

        jpu::bvh<3> bvh;
        bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);


        std::vector<res::vertex> opt_vertices(indices.size());
#pragma omp parallel for
        for(int i=0; i< opt_vertices.size(); ++i)
        {
            opt_vertices[i] = vertices[indices[i]];
            indices[i] = i;
        }

        _vertex_buffer = jpu::make_ref<gl::buffer>(opt_vertices);
        _index_buffer = jpu::make_ref<gl::buffer>(indices);
        _bvh_buffer = jpu::make_ref<gl::buffer>(bvh.pack());
        _vao = jpu::make_ref<gl::vertex_array>();
        _vao->set_element_buffer(*_index_buffer);
        _vao->add_binding(gl::vertex_attribute_binding(0, *_vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false));
        _transform = glm::mat4(1.f);
        _idx_count = indices.size();
    }

    void draw() const
    {
        _vao->bind();
        glDrawElements(GL_TRIANGLES, _idx_count, GL_UNSIGNED_INT, nullptr);
    }

private:
    int _idx_count;
    jpu::ref_ptr<gl::buffer> _vertex_buffer;
    jpu::ref_ptr<gl::buffer> _index_buffer;
    jpu::ref_ptr<gl::buffer> _bvh_buffer;
    jpu::ref_ptr<gl::vertex_array> _vao;
    res::transform _transform;
};

struct material
{
    glm::vec3 glass_tint{ 1, 1, 1 };
    float roughness_sqrt = 0.3f;
    glm::vec3 reflection_tint{ 1 };
    float glass = 1.0f;
    glm::vec3 base_color{ 0.4, 0.6, 0.1 };
    float ior = 1.5f;
    glm::vec3 glass_scatter_color = glm::vec3(0.7, 0.1, 0.5);
    float glass_density = 8;

    float glass_density_falloff = 8;
    float extinction_coefficient = 1.01f;

private:
    float _p[2];
};

struct mesh
{
    mesh(mesh_proxy* proxy, const intptr_t material_addr, const glm::mat4 model)
        : vertices(proxy->_vertex_buffer->address()),
        elements(proxy->_index_buffer->address()),
        bvh(proxy->_bvh_buffer->address()),
        material(material_addr),
        inv_model(inverse(model)),
        model(model)
    {}

    intptr_t vertices;
    intptr_t elements;
    intptr_t bvh;
    intptr_t material;
    glm::mat4 inv_model;
    glm::mat4 model;
};

jpu::named_vector<std::string, std::pair<jpu::ref_ptr<mesh_proxy>, std::vector<mesh*>>> meshes;

bool intersect_bounds(
    const glm::vec3 origin,
    const glm::vec3 direction,

    const glm::vec3 bounds_min,
    const glm::vec3 bounds_max,
    const float max_distance)
{
    const glm::vec3 inv_direction = 1.f / direction;

    //intersections with box planes parallel to x, y, z axis
    const glm::vec3 t135 = (bounds_min - origin) * inv_direction;
    const glm::vec3 t246 = (bounds_max - origin) * inv_direction;

    const glm::vec3 min_values = min(t135, t246);
    const glm::vec3 max_values = max(t135, t246);

    const float tmin = glm::max(glm::max(min_values.x, min_values.y), min_values.z);
    const float tmax = glm::min(glm::min(max_values.x, max_values.y), max_values.z);

    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

void resize()
{
    main_renderer->resize(resolution.x, resolution.y, 8);
    target_textures[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_textures[0]->storage_2d(resolution.x, resolution.y, GL_RGBA32F);
    target_textures[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_textures[1]->storage_2d(resolution.x, resolution.y, GL_RGBA16F);
    target_textures[2] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_textures[2]->storage_2d(resolution.x, resolution.y, GL_RGBA16F);
    target_image = jpu::make_ref<gl::image>(target_textures[0], 0, false, 0, GL_RGBA32F, GL_READ_WRITE);
    target_framebuffer = jpu::make_ref<gl::framebuffer>();
    target_framebuffer->attach(GL_COLOR_ATTACHMENT0, target_textures[0]);
    target_framebuffer->attach(GL_COLOR_ATTACHMENT1, target_textures[1]);
    target_framebuffer->attach(GL_COLOR_ATTACHMENT2, target_textures[2]);
    glViewport(0, 0, resolution.x, resolution.y);
}

int main()
{
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    main_window = jpu::make_ref<io::window>(io::api::opengl, resolution.x, resolution.y, "Grid");
    main_window->load_icon("../res/ui/logo.png");
    main_window->set_cursor(new io::cursor("../res/cursor.png", 0, 0));
    main_window->limit_framerate(60.f);
    gl::setup_shader_paths("../shaders");
    main_renderer = std::make_unique<gfx::renderer>(resolution.x, resolution.y, 8);

    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            for (auto&& pipeline : compute_pipelines)
                pipeline->reload_stages();
        }
    });
    glfwSetScrollCallback(*main_window, [](GLFWwindow*, double x, double y) {
        main_window->gui()->scrolled(y);
    });
    glfwSetCharCallback(*main_window, [](GLFWwindow*, uint32_t ch) {
        if (main_window->gui()->char_input(static_cast<wchar_t>(ch)))
            return;
    });
    glfwSetMouseButtonCallback(*main_window, [](GLFWwindow*, int btn, int action, int mods) {
        if (main_window->gui()->mouse_button_action(btn, action, mods))
            return;
    });

    glfwSetFramebufferSizeCallback(*main_window, [](GLFWwindow*, int w, int h) {
        resolution = { w, h };
        resize();
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0.8f, 0.9f, 1.f, 0);
    glClearDepth(0);

    resize();

    const auto pp_trace = compute_pipelines.push("Trace", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/trace.comp")));
    gl::graphics_pipeline pp_chunk;
    pp_chunk.use_stages(new gl::shader(gl::shader_root / "pathtracer/chunk.vert"), new gl::shader(gl::shader_root / "pathtracer/chunk.frag"));
    gl::vertex_array vao;

    io::camera cam;
    cam.inverse_z = true;
    cam.transform.position = glm::vec3(0, 0, 4);
    io::default_cam_controller ctrl;

    const auto sampler = jpu::make_ref<gl::sampler>();
    sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sampler->set(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
    sampler->set(GL_TEXTURE_CUBE_MAP_SEAMLESS, 16);
    sampler->set(GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    auto cubemap = jpu::make_ref<gl::texture>(gl::texture_type::cube_map);
    int w, h, c; stbi_info("../res/rtag/hdr/posx.hdr", &w, &h, &c);
    cubemap->storage_2d(w, h, GL_R11F_G11F_B10F);
    cubemap->assign_3d(0, 0, 0, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/posx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 1, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/negx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 2, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/posy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 3, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/negy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 4, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/posz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 5, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/rtag/hdr/negz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->generate_mipmaps();

    std::mt19937 gen;
    const std::uniform_real_distribution<float> dist(0.f, 1.f);

    // Test out mesh
    
    jpu::ref_ptr<gl::buffer> material_buffer;
    jpu::ref_ptr<gl::buffer> meshes_buffer;

    const auto load_mesh = [&](const auto& path, const bool combine)
    {
        auto geometry = res::load_geometry(path);
        if(combine)
        {
            // Combine all vertex data into one object

            using namespace jpu::flags_operators;
            material_buffer = jpu::make_ref<gl::buffer>(geometry.meshes.size() * sizeof(material), gl::buffer_flag_bits::map_persistent | gl::buffer_flag_bits::map_write | gl::buffer_flag_bits::dynamic_storage);
            meshes_buffer = jpu::make_ref<gl::buffer>(sizeof(mesh), gl::buffer_flag_bits::map_persistent | gl::buffer_flag_bits::map_write | gl::buffer_flag_bits::dynamic_storage);
            for (int i = 0; i < geometry.meshes.size(); ++i)
            {
                material_buffer->data_as<material>()[i] = material();
            }
            meshes[geometry.meshes.id_by_index(0)] = std::make_pair(jpu::make_ref<mesh_proxy>(geometry.meshes.payloads()), std::vector<mesh*>());
            meshes.get_by_index(0).second.push_back(&(meshes_buffer->data_as<mesh>()[0] = mesh(meshes.get_by_index(0).first, material_buffer->address(), glm::mat4(1.f))));
        }
        else
        {
            // Keep each object as it's own mesh

            using namespace jpu::flags_operators;
            material_buffer = jpu::make_ref<gl::buffer>(geometry.meshes.size() * sizeof(material), gl::buffer_flag_bits::map_persistent | gl::buffer_flag_bits::map_write | gl::buffer_flag_bits::dynamic_storage);
            meshes_buffer = jpu::make_ref<gl::buffer>(geometry.meshes.size() * sizeof(mesh), gl::buffer_flag_bits::map_persistent | gl::buffer_flag_bits::map_write | gl::buffer_flag_bits::dynamic_storage);
            for (int i = 0; i < geometry.meshes.size(); ++i)
            {
                material_buffer->data_as<material>()[i] = material();
                meshes[geometry.meshes.id_by_index(i)] = std::make_pair(jpu::make_ref<mesh_proxy>(geometry.meshes.get_by_index(i)), std::vector<mesh*>());
                meshes.get_by_index(i).second.push_back(&(meshes_buffer->data_as<mesh>()[i] = mesh(meshes.get_by_index(i).first, material_buffer->address() + i * sizeof(material), geometry.meshes.get_by_index(i).transform)));
            }
        }
    };
    load_mesh("../res/cube.dae", true);

    gl::query query_time(GL_TIME_ELAPSED);

    const int cube_vcount = static_cast<int>(res::presets::cube::vertices.size());
    const int cube_icount = static_cast<int>(res::presets::cube::indices.size());
    std::vector<glm::vec3> gizmo_positions(3 * cube_vcount);
    std::vector<uint16_t> gizmo_indices(3 * cube_icount);
    const glm::mat4 scale = glm::scale(glm::vec3(0.01f, 0.01f, 0.1f));
    const glm::mat4 translate = glm::translate(glm::vec3(0, 0, 1.f));
    const glm::mat4 rot1 = rotate(glm::radians(-90.f), glm::vec3(1, 0, 0));
    const glm::mat4 rot2 = rotate(glm::radians(90.f), glm::vec3(0, 1, 0));
    int moving = -1;
    for (int i = 0; i < cube_vcount; ++i)
    {
        gizmo_positions[i] = scale * translate * res::presets::cube::vertices[i].position;
        gizmo_positions[i + cube_vcount] = rot1 * scale * translate * res::presets::cube::vertices[i].position;
        gizmo_positions[i + 2 * cube_vcount] = rot2 * scale * translate * res::presets::cube::vertices[i].position;
    }
    for (int i = 0; i < cube_icount; ++i)
    {
        gizmo_indices[i] = res::presets::cube::indices[i];
        gizmo_indices[i + cube_icount] = cube_vcount + res::presets::cube::indices[i];
        gizmo_indices[i + 2 * cube_icount] = 2 * cube_vcount + res::presets::cube::indices[i];
    }
    const gl::buffer gizmo_index_buffer(gizmo_indices);
    const gl::buffer gizmo_vertex_buffer(gizmo_positions);
    gl::vertex_array gizmo_vao;
    gizmo_vao.set_element_buffer(gizmo_index_buffer);
    gizmo_vao.add_binding(gl::vertex_attribute_binding(0, gizmo_vertex_buffer, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0, false));
    gl::graphics_pipeline gizmo_pipeline;
    gizmo_pipeline.use_stages(new gl::shader(gl::shader_root / "gizmo/gizmo.vert"), new gl::shader(gl::shader_root / "gizmo/gizmo.frag"));

    res::image logo = res::load_svg_rasterized("../res/ui/logo.svg", 1.0f);
    auto ictex = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    ictex->storage_2d(logo.width, logo.height, GL_RGBA8);
    ictex->assign_2d(GL_RGBA, GL_UNSIGNED_BYTE, logo.data.get());

    gl::graphics_pipeline bilateral_pipeline;
    bilateral_pipeline.use_stages(new gl::shader(gl::shader_root / "postprocess/screen.vert"), new gl::shader(gl::shader_root / "postprocess/filter/bilateral.frag"));
    gl::vertex_array pp_vao;

    while (main_window->update())
    {
        ctrl.update(cam, *main_window, main_window->delta_time());

        const glm::mat4 camera_matrix = glm::mat4(glm::mat3(inverse(cam.view()))) * inverse(cam.projection());

        static int size = 4;
        static int frame = 0;
        static int max_samples = 4;
        static int sample_blend_offset = 0;
        static bool show_grid = true;
        static bool show_gizmo = true;
        static bool show_renderchunk = true;
        static bool show_bfilter = true;
        static float gauss_sigma = 6.f;
        static float gauss_bsigma = 0.1f;

        const int size_x = pp_trace->work_group_sizes()[0] * size;
        const int size_y = pp_trace->work_group_sizes()[0] * size;
        const int count_x = ceil(static_cast<float>(resolution.x) / size_x);
        const int count_y = ceil(static_cast<float>(resolution.y) / size_y);

        res::transform trafo = meshes.get_by_index(0).second.front()->model;
        int selected;
        double mx, my;
        glfwGetCursorPos(*main_window, &mx, &my);
        glm::vec2 uv = (glm::vec2(mx, my) / glm::vec2(resolution) - 0.5f) * 2.f;
        uv.y = -uv.y;
        const glm::vec3 direction = glm::vec3(camera_matrix * glm::vec4(uv, 0, 1));
        const glm::vec3 origin = cam.transform.position;
        const glm::vec3 inv_origin = inverse(glm::translate(trafo.position)) * glm::vec4(origin, 1);
        const glm::vec3 inv_dir = inverse(transpose(inverse(glm::translate(trafo.position)))) * glm::vec4(direction, 0);
        const float grip_width = 0.1f;
        if (!(moving + 1) && intersect_bounds(inv_origin, inv_dir, glm::vec3(-grip_width, -grip_width, 0.f), glm::vec3(grip_width, grip_width, 1.f), 100000000.f))
        {
            selected = 0;
        }
        else if (!(moving + 1) && intersect_bounds(inv_origin, inv_dir, glm::vec3(-grip_width, 0.f, -grip_width), glm::vec3(grip_width, 1.f, grip_width), 100000000.f))
        {
            selected = 1;
        }
        else if (!(moving + 1) && intersect_bounds(inv_origin, inv_dir, glm::vec3(0.f, -grip_width, -grip_width), glm::vec3(1.f, grip_width, grip_width), 100000000.f))
        {
            selected = 2;
        }
        else
        {
            selected = moving;
        }

        if (selected + 1 && glfwGetMouseButton(*main_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            moving = selected;
            const int mv = selected == 0 ? 2 : (selected == 1 ? 1 : 0);
            const glm::vec3 plane_orig = trafo.position;
            glm::vec3 plane_norm(0.f);
            plane_norm[(mv + 1) % 3] = 1;
            trafo.position[(mv) % 3] = abs(dot(plane_norm, direction)) > 1e-6 ?
                (origin + direction * abs(dot(plane_orig - origin, plane_norm) / dot(plane_norm, direction)))[(mv) % 3] - 0.5f :
                trafo.position[(mv) % 3];
        }
        else if (moving + 1)
        {
            moving = -1;
        }

        double t = 0.0;
        while (t < main_window->get_swap_delay() / 2.f)
        {
            query_time.begin();
            pp_trace->bind();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = *target_image;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<int>("sample_blend_offset") = sample_blend_offset;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<int>("max_samples") = max_samples;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::vec3>("camera_position") = cam.transform.position;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("camera_matrix") = camera_matrix;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("cubemap") = sampler->sample_texture(cubemap);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("meshes") = meshes_buffer->address();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<int>("num_meshes") = meshes_buffer->size() / sizeof(mesh);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::ivec2>("offset") = glm::ivec2(size_x * (frame % count_x), size_y * (frame / count_x));
            pp_trace->dispatch(size_x, size_y);
            frame = (frame + 1) % (count_x * count_y);
            query_time.end();
            t += query_time.get_uint64() / 1'000'000'000.0;
        }

        target_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        target_framebuffer->bind();
        bilateral_pipeline.bind();
        pp_vao.bind();
        bilateral_pipeline.stage(gl::shader_type::fragment)->get_uniform<float>("gauss_bsigma") = gauss_bsigma;
        bilateral_pipeline.stage(gl::shader_type::fragment)->get_uniform<uint64_t>("src_textures[0]") = sampler->sample_texture(target_textures[0]);
        bilateral_pipeline.stage(gl::shader_type::fragment)->get_uniform<float>("gauss_sigma") = show_bfilter ? gauss_sigma : 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        target_framebuffer->unbind();

        target_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT2 });
        main_renderer->bind();
        bilateral_pipeline.bind();
        pp_vao.bind();
        bilateral_pipeline.stage(gl::shader_type::fragment)->get_uniform<uint64_t>("src_textures[0]") = sampler->sample_texture(target_textures[1]);
        bilateral_pipeline.stage(gl::shader_type::fragment)->get_uniform<float>("gauss_sigma") = 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        main_renderer->draw(main_window->delta_time(), target_framebuffer);

        target_framebuffer->read_from_attachment(GL_COLOR_ATTACHMENT2);
        target_framebuffer->blit(nullptr, gl::framebuffer::blit_rect{ 0, 0, resolution.x, resolution.y },
            gl::framebuffer::blit_rect{ 0, 0, resolution.x, resolution.y }, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        if (show_renderchunk)
        {
            glLineWidth(2.f);
            pp_chunk.bind();
            vao.bind();
            pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("inv_resolution") = 1.f / glm::vec2(resolution);
            pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("offset") = glm::vec2(size_x * (frame % count_x), size_y * (frame / count_x));
            pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("size") = glm::vec2(size_x, size_y);
            glDrawArrays(GL_LINE_STRIP, 0, 5);
        }

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        gizmo_pipeline.bind();
        gizmo_vao.bind();
        gizmo_pipeline.stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("model_view_projection") = cam.projection() * cam.view() * glm::translate(trafo.position);
        gizmo_pipeline.stage(gl::shader_type::fragment)->get_uniform<int>("hovered") = selected;
        if (show_gizmo)
            glDrawElements(GL_TRIANGLES, 3 * cube_icount, GL_UNSIGNED_SHORT, nullptr);

        if (show_grid)
        {
            for (auto&& p : meshes)
            {
                glLineWidth(0.1f);
                glPolygonMode(GL_FRONT, GL_LINE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                for (auto&& s : p.second)
                {
                    gizmo_pipeline.stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("model_view_projection") = cam.projection() * cam.view() * s->model;
                    gizmo_pipeline.stage(gl::shader_type::fragment)->get_uniform<int>("hovered") = -2;
                    p.first->draw();
                }
                glPolygonMode(GL_FRONT, GL_FILL);
                glDisable(GL_BLEND);
            }
        }
        glDisable(GL_DEPTH_TEST);

        meshes.get_by_index(0).second.front()->model = static_cast<glm::mat4>(trafo);
        meshes.get_by_index(0).second.front()->inv_model = inverse(static_cast<glm::mat4>(trafo));
        ImGui::Begin("Window");
        ImGui::Image(reinterpret_cast<ImTextureID>(sampler->sample_texture(ictex)), ImVec2(logo.width, logo.height));
        ImGui::Value("Frametime", static_cast<float>(1'000 * main_window->delta_time()));
        ImGui::DragInt("Size", &size, 0.1f, 1.f, 100.f);
        ImGui::DragInt("Max. Samples", &max_samples, 0.01f, 1.f, 100.f);
        ImGui::DragInt("Sample Blend Offset", &sample_blend_offset, 0.01f, 1.f, 100.f);
        ImGui::Checkbox("Show Mesh", &show_grid);
        ImGui::Checkbox("Show Gizmo", &show_gizmo);
        ImGui::Checkbox("Show Chunk", &show_renderchunk);
        ImGui::Checkbox("Enable Filtering", &show_bfilter);
        if (show_bfilter)
        {
            ImGui::DragFloat("Gauss Sigma", &gauss_sigma);
            ImGui::DragFloat("Gauss B-Sigma", &gauss_bsigma);
        }

        static bool with_bg_alpha = true;
        ImGui::Checkbox("Enable Background Alpha", &with_bg_alpha);
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0)))
        {
            res::stbi_data tex_data(malloc(resolution.x * resolution.y * 4 * sizeof(float)));
            target_textures[2]->get_texture_data(GL_RGBA, GL_FLOAT, resolution.x * resolution.y * 4 * sizeof(float), tex_data.get());

            glm::vec4* ic = static_cast<glm::vec4*>(tex_data.get());
            std::vector<glm::u8vec4> convert(resolution.x * resolution.y);
            for (int x = 0; x < resolution.x; ++x)
            {
                for (int y = 0; y < resolution.y; ++y)
                {
                    convert[y * resolution.x + x] = clamp(ic[(resolution.y - 1 - y) * resolution.x + x] * 255.f, glm::vec4(0, 0, 0, with_bg_alpha ? 0 : 255.f), glm::vec4(255.f));
                }
            }

            constexpr const char *extensions[1] = {
                "*.png"
            };
            if (const auto dst = tinyfd_saveFileDialog("Save output", "../res/", 1, extensions, "*.png"))
                stbi_write_png(dst, resolution.x, resolution.y, 4, convert.data(), 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save2", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            res::stbi_data tex_data(malloc(resolution.x * resolution.y * 4 * sizeof(float)));
            target_textures[1]->get_texture_data(GL_RGBA, GL_FLOAT, resolution.x * resolution.y * 4 * sizeof(float), tex_data.get());

            glm::vec4* ic = static_cast<glm::vec4*>(tex_data.get());
            std::vector<glm::u8vec4> convert(resolution.x * resolution.y);
            for (int x = 0; x < resolution.x; ++x)
            {
                for (int y = 0; y < resolution.y; ++y)
                {
                    convert[y * resolution.x + x] = clamp(ic[(resolution.y - 1 - y) * resolution.x + x] * 255.f, glm::vec4(0, 0, 0, with_bg_alpha ? 0 : 255.f), glm::vec4(255.f));
                }
            }

            constexpr const char *extensions[1] = {
                "*.png"
            };
            if (const auto dst = tinyfd_saveFileDialog("Save output", "../res/", 1, extensions, "*.png"))
                stbi_write_png(dst, resolution.x, resolution.y, 4, convert.data(), 0);
        }
        static bool combine_mesh = true;
        ImGui::Checkbox("Combine mesh when loading", &combine_mesh);
        if (ImGui::Button("Open", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            constexpr const char *fs[6] = {
                "*.dae", "*.fbx", "*.obj", "*.stl", "*.ply", "*.blend"
            };
            if (const auto src = tinyfd_openFileDialog("Open Mesh", "../res/", 6, fs, "mesh", false))
            {
                meshes.clear();
                load_mesh(src, true);
            }
        }

        for (int i = 0; i < material_buffer->size() / sizeof(material); ++i)
        {
            const auto id = ("Material " + std::to_string(i));
            if (ImGui::CollapsingHeader(id.c_str()))
            {
                ImGui::PushID(id.c_str());
                auto&& item = material_buffer->data_as<material>() + i;
                ImGui::DragFloat("Roughness", &item->roughness_sqrt, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("Glass", &item->glass, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("IOR", &item->ior, 0.01f, 0.f, 100.f);
                ImGui::DragFloat("Ex. Coeff.", &item->extinction_coefficient, 0.01f, 0.f, 100.f);
                ImGui::Spacing();
                ImGui::ColorEdit3("Color", &item->base_color[0]);
                ImGui::ColorEdit3("Reflection Tint", &item->reflection_tint[0]);
                ImGui::ColorEdit3("Glass Tint", &item->glass_tint[0]);
                ImGui::ColorEdit3("Glass Scatter Tint", &item->glass_scatter_color[0]);
                ImGui::Spacing();
                ImGui::DragFloat("Glass Density", &item->glass_density, 0.01f, 0.f, 100.f);
                ImGui::DragFloat("Glass Density Falloff", &item->glass_density_falloff, 0.01f, 0.f, 100.f);
                ImGui::PopID();
            }
        }

        ImGui::End();
    }

    return 0;
}