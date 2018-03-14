#include <random>
#include <jpu/memory.hpp>
#include <jpu/data.hpp>
#include <jpu/log.hpp>
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
#include "stb_image_write.h"
#include "res/presets.hpp"
#include "framework/renderer.hpp"
#include <GLFW/glfw3.h>
#include "framework/gizmo.hpp"

glm::ivec2 resolution{ 1280, 720 };
jpu::ref_ptr<io::window> main_window;
std::unique_ptr<gfx::renderer> main_renderer;

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

        const std::vector<uint8_t> packed_bvh = bvh.pack();
        _vertex_buffer = gl::buffer<res::vertex>(mesh.vertices.begin(), mesh.vertices.end());
        _index_buffer = gl::buffer<uint32_t>(indices.begin(), indices.end());
        _bvh_buffer = gl::buffer<gl::byte>(packed_bvh.begin(), packed_bvh.end());
        _transform = mesh.transform;
    }

    mesh_proxy(const std::vector<res::mesh>& meshes)
    {
        std::vector<uint32_t> indices;
        std::vector<res::vertex> vertices;

        int base_offset = 0;
        int index_offset = 0;
        for (int i = 0; i < meshes.size(); ++i)
        {
            indices.resize(meshes[i].indices.size() + indices.size());
            vertices.resize(meshes[i].vertices.size() + vertices.size());

            const glm::mat4 model = static_cast<glm::mat4>(meshes[i].transform);
            const glm::mat4 normal_matrix = transpose(inverse(model));

#pragma omp parallel for
            for (int vtx = 0; vtx < meshes[i].vertices.size(); ++vtx)
            {
                vertices[base_offset + vtx].position = model * glm::vec4(meshes[i].vertices[vtx].position, 1);
                vertices[base_offset + vtx].normal = normalize(normal_matrix * glm::vec4(meshes[i].vertices[vtx].normal, 0));
                vertices[base_offset + vtx].uv = meshes[i].vertices[vtx].uv;
                vertices[base_offset + vtx].meta = i;   // mesh material sub-index-offset
            }

#pragma omp parallel for
            for (int idx = 0; idx < meshes[i].indices.size(); ++idx)
            {
                indices[idx + index_offset] = meshes[i].indices[idx] + base_offset;
            }
            index_offset += int(meshes[i].indices.size());
            base_offset += int(meshes[i].vertices.size());
        }

        jpu::bvh<3> bvh;
        bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);


        std::vector<res::vertex> opt_vertices(indices.size());
#pragma omp parallel for
        for (int i = 0; i < opt_vertices.size(); ++i)
        {
            opt_vertices[i] = vertices[indices[i]];
            indices[i] = i;
        }

        const std::vector<uint8_t> packed_bvh = bvh.pack();
        _vertex_buffer = gl::buffer<res::vertex>(opt_vertices.begin(), opt_vertices.end());
        _index_buffer = gl::buffer<uint32_t>(indices.begin(), indices.end());
        _bvh_buffer = gl::buffer<gl::byte>(packed_bvh.begin(), packed_bvh.end());

        _transform = glm::mat4(1.f);
    }

    void draw(const gl::pipeline& p) const
    {
        p.bind_attribute(0, _vertex_buffer, 3, GL_FLOAT, offsetof(res::vertex, position));
        p.draw(GL_TRIANGLES, _index_buffer, GL_UNSIGNED_INT);
    }

private:
    gl::buffer<res::vertex> _vertex_buffer;
    gl::buffer<uint32_t> _index_buffer;
    gl::buffer<gl::byte> _bvh_buffer;
    res::transform _transform;
};

struct material
{
    alignas(16) struct {
        glm::vec3 scatter_color{ 1, 1, 1 };
        float transmissiveness{ 0 };
        glm::vec3 density_color{ 1, 1, 1 };
        float density{ 0 };
        glm::vec3 reflection_color{ 1, 1, 1 };
        float density_falloff{ 1 };

        float roughness{ 0.95f };
    } ground;

    float extinction{ 1 };
    float ior{ 1.5f };

    struct
    {
        float roughness{ 0.1f };
        glm::vec3 reflection_color{ 1, 1, 1 };
    } surface;
};

struct mesh
{
    mesh(mesh_proxy* proxy, const intptr_t material_addr, const glm::mat4 model)
        : vertices(proxy->_vertex_buffer.handle()),
        elements(proxy->_index_buffer.handle()),
        bvh(proxy->_bvh_buffer.handle()),
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
int selected_mesh = 0;

std::unique_ptr<gl::framebuffer> target_framebuffer;
std::array<std::shared_ptr<gl::texture>, 3> target_textures;
std::unique_ptr<gl::image> target_image;

void resize(GLFWwindow*, int w, int h)
{
    resolution = { w, h };
    main_renderer->resize(resolution.x, resolution.y, 8);
    target_textures[0] = std::make_shared<gl::texture>(GL_TEXTURE_2D, resolution.x, resolution.y, GL_RGBA32F);
    target_textures[1] = std::make_shared<gl::texture>(GL_TEXTURE_2D, resolution.x, resolution.y, GL_RGBA16F);
    target_textures[2] = std::make_shared<gl::texture>(GL_TEXTURE_2D, resolution.x, resolution.y, GL_RGBA16F);
    target_image = std::make_unique<gl::image>(*target_textures[0], GL_RGBA32F, GL_READ_WRITE);

    target_framebuffer = std::make_unique<gl::framebuffer>();
    target_framebuffer->emplace(GL_COLOR_ATTACHMENT0, target_textures[0]);
    target_framebuffer->emplace(GL_COLOR_ATTACHMENT1, target_textures[1]);
    target_framebuffer->emplace(GL_COLOR_ATTACHMENT2, target_textures[2]);
    glViewportIndexedf(0, 0, 0, float(resolution.x), float(resolution.y));
}

int main()
{
    gl::shader::set_include_directories("../shaders");

    res::image logo = res::load_svg_rasterized("../res/ui/logo.svg", 1.0f);
    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::RGBA);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    main_window = jpu::make_ref<io::window>(io::api::opengl, resolution.x, resolution.y, "Grid");
    main_window->set_icon(logo.width, logo.height, logo.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    main_window->set_max_framerate(1000.f);
    main_window->callbacks->framebuffer_size_callback.add(&resize);
    main_window->callbacks->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
            return;
      /*  if (key == GLFW_KEY_P && action == GLFW_PRESS)
            for (auto&& pipeline : compute_pipelines)
                pipeline->reload_stages();*/
        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
            ++selected_mesh;
    });
    main_renderer = std::make_unique<gfx::renderer>(resolution.x, resolution.y, 8);
    resize(*main_window, resolution.x, resolution.y);

    io::camera cam;
    io::default_cam_controller ctrl;
    cam.transform.position = glm::vec3(0, 0, 4);

    const gl::sampler sampler;

    std::vector<res::image> cubemap_images;
    cubemap_images.emplace_back(load_image("../res/ven/hdr/posx.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/ven/hdr/negx.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/ven/hdr/posy.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/ven/hdr/negy.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/ven/hdr/posz.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/ven/hdr/negz.hdr", res::image_type::f32, res::RGB));

    const int w = cubemap_images[0].width;
    const int h = cubemap_images[0].height;
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[0].data.get());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[1].data.get());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[2].data.get());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[3].data.get());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[4].data.get());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[5].data.get());
    cubemap.generate_mipmaps();
    cubemap_images.clear();

    std::mt19937 gen;
    const std::uniform_real_distribution<float> dist(0.f, 1.f);

    gl::buffer<material> material_buffer(GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<mesh> meshes_buffer(GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

    const auto load_mesh = [&](const auto& path, const bool combine) {
        auto geometry = res::load_geometry(path);
        material_buffer.resize(geometry.meshes.size());
        meshes_buffer.clear();
        meshes_buffer.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

        if (combine)
        {
            // Combine all vertex data into one object
            meshes[geometry.meshes.id_by_index(0)] = std::make_pair(jpu::make_ref<mesh_proxy>(geometry.meshes.get_by_index(0)), std::vector<mesh*>());
            meshes_buffer.emplace_back(meshes.get_by_index(0).first, material_buffer.handle(), glm::mat4(1.f));
            meshes.get_by_index(0).second.push_back(&meshes_buffer[0]);
        }
        else
        {
            // Keep each object as it's own mesh
            meshes_buffer.reserve(geometry.meshes.size());
            for (int i = 0; i < geometry.meshes.size(); ++i)
            {
                meshes[geometry.meshes.id_by_index(i)] = std::make_pair(jpu::make_ref<mesh_proxy>(geometry.meshes.get_by_index(i)), std::vector<mesh*>());
                meshes_buffer.emplace_back(meshes.get_by_index(i).first, material_buffer.handle() + i * sizeof(material), geometry.meshes.get_by_index(i).transform);
                meshes.get_by_index(i).second.push_back(&(meshes_buffer[i]));
            }
        }
    };
    load_mesh("../res/cube.dae", true);

    gl::query query_trace(GL_TIME_ELAPSED);
    gl::query query_uniform(GL_TIME_ELAPSED);
    gl::query query_post(GL_TIME_ELAPSED);


    gl::compute_pipeline pp_trace(std::make_shared<gl::shader>("pathtracer/trace.comp"));
    
    gl::pipeline pp_chunk;
    pp_chunk[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("pathtracer/chunk.vert");
    pp_chunk[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("pathtracer/chunk.frag");

    gl::pipeline pp_mesh;
    pp_mesh[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("pathtracer/mesh.vert");
    pp_mesh[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("pathtracer/mesh.frag");

    gl::pipeline bilateral_pipeline;
    bilateral_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("postprocess/screen.vert");
    bilateral_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("postprocess/filter/bilateral.frag");

    gfx::gizmo gizmo;
    
    uint32_t bID = glGetUniformBlockIndex(*pp_trace.stage(), "Infos");
    int size = -1; glGetActiveUniformBlockiv(*pp_trace.stage(), bID, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
    int c; glGetActiveUniformBlockiv(*pp_trace.stage(), bID, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &c);
    for (uint32_t i = 0; i < uint32_t(c); ++i)
    {
        int len; glGetActiveUniformsiv(*pp_trace.stage(), 1, &i, GL_UNIFORM_NAME_LENGTH, &len);
        std::string str(len, ' '); glGetActiveUniformName(*pp_trace.stage(), i, len, &len, str.data());
        int offsets; glGetActiveUniformsiv(*pp_trace.stage(), 1, &i, GL_UNIFORM_OFFSET, &offsets);
        int strides; glGetActiveUniformsiv(*pp_trace.stage(), 1, &i, GL_UNIFORM_ARRAY_STRIDE, &strides);
        int sizes; glGetActiveUniformsiv(*pp_trace.stage(), 1, &i, GL_UNIFORM_SIZE, &sizes);
        log_i << str << ": " << offsets << ", " << strides << ", " << sizes;
    }
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    struct pathtracer_info
    {
        alignas(16) glm::ivec2 offset;
        alignas(4)  float random_gen;
        alignas(4)  int num_meshes;
        alignas(16) uint64_t target_image;
        alignas(16) uint64_t cubemap;
        alignas(16) glm::mat4 camera_matrix;
        alignas(16) uint64_t meshes;
        alignas(4)  int sample_blend_offset;
        alignas(4)  float max_bounces;
        alignas(16) glm::vec3 camera_position;
        alignas(4)  int max_samples;
    };

    gl::buffer<pathtracer_info> pathtracer_info_buffer(1, GL_DYNAMIC_STORAGE_BIT);
        
    while (main_window->update())
    {
        ctrl.update(cam, *main_window, main_window->delta_time());

        const glm::mat4 camera_matrix = inverse(cam.projection() * glm::mat4(glm::mat3(cam.view())));

        selected_mesh = selected_mesh % meshes.size();
        res::transform gizmo_transform = meshes.get_by_index(selected_mesh).second.front()->model;
        gizmo.transform = &gizmo_transform;

        static int size = 12;
        static int frame = 0;
        static int max_samples = 2;
        static int sample_blend_offset = 0;
        static bool show_grid = true;
        static bool show_gizmo = true;
        static bool show_renderchunk = true;
        static bool show_bfilter = true;
        static bool enable_continuous_improvement = false;
        static float gauss_sigma = 6.f;
        static float gauss_bsigma = 0.1f;

        const int size_x = pp_trace.group_sizes()[0] * size;
        const int size_y = pp_trace.group_sizes()[0] * size;
        const int count_x = int( ceil(float(resolution.x) / size_x) );
        const int count_y = int( ceil(float(resolution.y) / size_y) );

        query_uniform.start();
        pp_trace.bind_uniform_buffer(0, pathtracer_info_buffer);
        
        pathtracer_info_buffer[0].target_image = *target_image;
        pathtracer_info_buffer[0].random_gen = dist(gen);
        pathtracer_info_buffer[0].sample_blend_offset = sample_blend_offset;
        pathtracer_info_buffer[0].max_samples = max_samples;
        pathtracer_info_buffer[0].camera_position = cam.transform.position;
        pathtracer_info_buffer[0].camera_matrix = camera_matrix;
        pathtracer_info_buffer[0].cubemap = sampler.sample(cubemap);
        pathtracer_info_buffer[0].meshes = meshes_buffer.handle();
        pathtracer_info_buffer[0].num_meshes = int(meshes_buffer.size());
        pathtracer_info_buffer[0].max_bounces = 8;
        pathtracer_info_buffer[0].offset = glm::ivec2(size_x * (frame % count_x), size_y * (frame / count_x));
        pathtracer_info_buffer.synchronize();
        query_uniform.finish();

        query_trace.start();
        pp_trace.dispatch(size_x, size_y);
        frame = (frame + 1) % (count_x * count_y);
        if (frame == 0 && enable_continuous_improvement)
            ++sample_blend_offset;
        query_trace.finish();

        query_post.start();
        main_renderer->bind();
        bilateral_pipeline.bind();
        bilateral_pipeline[GL_FRAGMENT_SHADER]->uniform<float>("gauss_bsigma") = gauss_bsigma;
        bilateral_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("src_textures[0]") = sampler.sample(*target_textures[0]);
        bilateral_pipeline[GL_FRAGMENT_SHADER]->uniform<float>("gauss_sigma") = show_bfilter ? gauss_sigma : 0;
        bilateral_pipeline.draw(GL_TRIANGLES, 3);
        target_framebuffer->set_drawbuffer(GL_COLOR_ATTACHMENT1);
        main_renderer->draw(main_window->delta_time(), *target_framebuffer);

        target_framebuffer->set_readbuffer(GL_COLOR_ATTACHMENT1);
        target_framebuffer->blit(nullptr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        gl::framebuffer::zero().bind();
        query_post.finish();

        if (show_renderchunk)
        {
            glLineWidth(2.f);
            pp_chunk.bind();
            pp_chunk[GL_VERTEX_SHADER]->uniform<glm::vec2>("inv_resolution") = 1.f / glm::vec2(resolution);
            pp_chunk[GL_VERTEX_SHADER]->uniform<glm::vec2>("offset") = glm::vec2(size_x * (frame % count_x), size_y * (frame / count_x));
            pp_chunk[GL_VERTEX_SHADER]->uniform<glm::vec2>("size") = glm::vec2(size_x, size_y);
            pp_chunk.draw(GL_LINE_LOOP, 4);
        }

        if (show_grid)
        {
            for (auto&& p : meshes)
            {
                glLineWidth(0.1f);
                glPolygonMode(GL_FRONT, GL_LINE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                pp_mesh.bind();
                for (auto&& s : p.second)
                {
                    pp_mesh[GL_VERTEX_SHADER]->uniform<glm::mat4>("model_view_projection") = cam.projection() * cam.view() * s->model;
                    p.first->draw(pp_mesh);
                }
                glPolygonMode(GL_FRONT, GL_FILL);
                glDisable(GL_BLEND);
            }
        }

        gizmo.update(*main_window, cam.view(), cam.projection());
        if (show_gizmo) gizmo.render();

        meshes_buffer[selected_mesh].model = static_cast<glm::mat4>(gizmo_transform);
        meshes_buffer[selected_mesh].inv_model = inverse(static_cast<glm::mat4>(gizmo_transform));
        meshes_buffer.synchronize();

        ImGui::Begin("Window");
        ImGui::Value("Frametime", static_cast<float>(1'000 * main_window->delta_time()));
        ImGui::DragInt("Size", &size, 0.1f, 1, 100);
        ImGui::DragInt("Max. Samples", &max_samples, 0.01f, 1, 100);
        ImGui::DragInt("Sample Blend Offset", &sample_blend_offset, 0.01f, 1, 100);
        ImGui::Value("Update", float(query_uniform.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Trace", float(query_trace.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("PostFX", float(query_post.get<uint64_t>() / 1'000'000.0));
        ImGui::Checkbox("Show Mesh", &show_grid);
        ImGui::Checkbox("Show Gizmo", &show_gizmo);
        ImGui::Checkbox("Show Chunk", &show_renderchunk);
        if (ImGui::Checkbox("Continuous Improvement", &enable_continuous_improvement) && enable_continuous_improvement)
            sample_blend_offset = 0;

        ImGui::Checkbox("Enable PostProcess", &main_renderer->enabled);
        ImGui::Checkbox("Enable Filtering", &show_bfilter);
        if (show_bfilter)
        {
            ImGui::DragFloat("Gauss Sigma", &gauss_sigma);
            ImGui::DragFloat("Gauss B-Sigma", &gauss_bsigma);
        }

        static bool with_bg_alpha = true;
        ImGui::Checkbox("Enable Background Alpha", &with_bg_alpha);
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            res::stbi_data tex_data(malloc(resolution.x * resolution.y * 4 * sizeof(float)));
            target_textures[1]->get_data(GL_RGBA, GL_FLOAT, resolution.x * resolution.y * 4 * sizeof(float), tex_data.get());

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
                load_mesh(src, combine_mesh);
            }
        }

        for (int i = 0; i < material_buffer.size(); ++i)
        {
            const auto id = ("Material " + std::to_string(i));
            if (ImGui::CollapsingHeader(id.c_str()))
            {
                ImGui::PushID(id.c_str());
                auto&& item = material_buffer[i];

                ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
                ImGui::Text("Ground");
                ImGui::PopStyleColor();
                ImGui::PopFont();
                ImGui::PushID("Ground");
                ImGui::ColorEdit3("Scatter Color", &item.ground.scatter_color[0]);
                ImGui::ColorEdit3("Density Color", &item.ground.density_color[0]);
                ImGui::ColorEdit3("Reflect Color", &item.ground.reflection_color[0]);
                ImGui::DragFloat("Roughness", &item.ground.roughness, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("Transmissiveness", &item.ground.transmissiveness, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("Density", &item.ground.density, 0.01f, 0.f, 100.f);
                ImGui::DragFloat("Density Falloff", &item.ground.density_falloff, 0.01f, 0.f, 100.f);
                ImGui::Spacing();
                ImGui::PopID();

                ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
                ImGui::Text("Surface");
                ImGui::PopStyleColor();
                ImGui::PopFont();
                ImGui::PushID("Surface");
                ImGui::ColorEdit3("Reflect Color", &item.surface.reflection_color[0]);
                ImGui::DragFloat("Roughness", &item.surface.roughness, 0.01f, 0.f, 1.f);
                ImGui::PopID();

                ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
                ImGui::Text("General");
                ImGui::PopStyleColor();
                ImGui::PopFont();
                ImGui::PushID("General");
                ImGui::DragFloat("Extinction Coeff.", &item.extinction, 0.01f, 0.f, 100.f);
                ImGui::DragFloat("IOR", &item.ior, 0.01f, 0.f, 100.f);
                ImGui::PopID();
                ImGui::PopID();
            }
        }

        ImGui::End();
    }

    return 0;
}