#include <random>
#include <jpu/memory>
#include <jpu/data>
#include "io/window.hpp"
#include "opengl/framebuffer.hpp"
#include "io/camera.hpp"
#include "res/geometry.hpp"
#include "jpu/impl/geometry/bvh/bvh.hpp"
#include "res/image.hpp"
#include "opengl/query.hpp"

#include "jpu/impl/log/log.hpp"
#include "tinyfd/tinyfiledialogs.h"
#include "io/gui_impl/gui_none.hpp"
#include "opengl/pipeline.hpp"
#include "opengl/texture.hpp"
#include "opengl/buffer.hpp"
#include "opengl/vertex_array.hpp"
#include "stb_image_write.h"
#include "res/presets.hpp"

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::compute_pipeline>> compute_pipelines;
uint32_t current_target = 0;
jpu::ref_ptr<gl::texture> target_texture;
float dt = 1 / 60.f;

bool intersect_bounds(
    const glm::vec3 origin,
    const glm::vec3 direction,

    const glm::vec3 bounds_min,
    const glm::vec3 bounds_max,
    const float max_distance)
{
    glm::vec3 inv_direction = 1.f / direction;

    //intersections with box planes parallel to x, y, z axis
    glm::vec3 t135 = (bounds_min - origin) * inv_direction;
    glm::vec3 t246 = (bounds_max - origin) * inv_direction;

    glm::vec3 min_values = min(t135, t246);
    glm::vec3 max_values = max(t135, t246);

    float tmin = glm::max(glm::max(min_values.x, min_values.y), min_values.z);
    float tmax = glm::min(glm::min(max_values.x, max_values.y), max_values.z);

    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

int main(int argc, const char** args)
{
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 800, 800, "Grid");
    main_window->load_icon("../res/ui/logo.png");
    main_window->set_cursor(new io::cursor("../res/cursor.png", 0, 0));
    main_window->set_swap_delay(dt);
    gl::setup_shader_paths("../shaders");

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0.8f, 0.9f, 1.f, 0);
    glClearDepth(0);

    target_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_texture->storage_2d(800, 800, GL_RGBA16F);
    const gl::image target_image(target_texture, 0, false, 0, GL_RGBA16F, GL_READ_WRITE);

    gl::framebuffer blit_framebuffer;
    blit_framebuffer.attach(GL_COLOR_ATTACHMENT0, target_texture);

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
    int w, h, c; stbi_info("../res/hdr/posx.hdr", &w, &h, &c);
    cubemap->storage_2d(w, h, GL_R11F_G11F_B10F);
    cubemap->assign_3d(0, 0, 0, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 1, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 2, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 3, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 4, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->assign_3d(0, 0, 5, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap->generate_mipmaps();

    std::mt19937 gen;
    const std::uniform_real_distribution<float> dist(0.f, 1.f);

    // Test out mesh
    auto geometry = res::load_geometry("../res/torusse.dae");
    std::vector<res::vertex> vertices = std::move(geometry.meshes.get_by_index(0).vertices);
    std::vector<uint32_t> indices = std::move(geometry.meshes.get_by_index(0).indices);
    jpu::bvh<3> bvh;
    bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    auto vertex_buffer = jpu::make_ref<gl::buffer>(vertices);
    auto index_buffer = jpu::make_ref<gl::buffer>(indices);
    auto bvh_buffer = jpu::make_ref<gl::buffer>(bvh.pack());
    auto mesh_vao = jpu::make_ref<gl::vertex_array>();
    mesh_vao->set_element_buffer(*index_buffer);
    mesh_vao->add_binding(gl::vertex_attribute_binding(0, *vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false));

    struct material
    {
        glm::vec3 glass_tint{ 0.4, 0.6, 0.1 };
        float roughness_sqrt = 0.6f;
        glm::vec3 reflection_tint{ 1 };
        float glass = 0.0f;
        glm::vec3 base_color{ 0.4, 0.6, 0.1 };
        float ior = 1.5f;
        float extinction_coefficient = 0.01f;

    private:
        float _p[3];
    };
    auto material_buffer = jpu::make_ref<gl::buffer>(sizeof(material), gl::buffer_flag_bits::map_dynamic_persistent);
    *material_buffer->data_as<material>() = material();
    
    gl::query query_time(GL_TIME_ELAPSED);

    const int cube_vcount = static_cast<int>(res::presets::cube::vertices.size());
    const int cube_icount = static_cast<int>(res::presets::cube::indices.size());
    std::vector<glm::vec3> gizmo_positions(3 * cube_vcount);
    std::vector<uint16_t> gizmo_indices(3 * cube_icount);
    const glm::mat4 scale = glm::scale(glm::vec3(0.05f, 0.05f, 0.5f));
    const glm::mat4 translate = glm::translate(glm::vec3(0, 0, 1.f));
    const glm::mat4 rot1 = glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0));
    const glm::mat4 rot2 = glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0));
    res::transform trafo;
    int moving = -1;
    for(int i=0; i < cube_vcount; ++i)
    {
        gizmo_positions[i] = scale * translate * res::presets::cube::vertices[i].position;
        gizmo_positions[i + cube_vcount] = rot1 * scale * translate * res::presets::cube::vertices[i].position;
        gizmo_positions[i + 2 * cube_vcount] = rot2 * scale * translate * res::presets::cube::vertices[i].position;
    }
    for(int i=0; i<cube_icount; ++i)
    {
        gizmo_indices[i] = res::presets::cube::indices[i];
        gizmo_indices[i + cube_icount] = cube_vcount + res::presets::cube::indices[i];
        gizmo_indices[i + 2 * cube_icount] = 2 * cube_vcount + res::presets::cube::indices[i];
    }
    gl::buffer gizmo_index_buffer(gizmo_indices);
    gl::buffer gizmo_vertex_buffer(gizmo_positions);
    gl::vertex_array gizmo_vao;
    gizmo_vao.set_element_buffer(gizmo_index_buffer);
    gizmo_vao.add_binding(gl::vertex_attribute_binding(0, gizmo_vertex_buffer, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0, false));
    gl::graphics_pipeline gizmo_pipeline;
    gizmo_pipeline.use_stages(new gl::shader(gl::shader_root / "gizmo/gizmo.vert"), new gl::shader(gl::shader_root / "gizmo/gizmo.frag"));

    while (main_window->update())
    {
        ctrl.update(cam, *main_window, main_window->delta_time());

        glm::mat4 camera_matrix = glm::mat4(glm::mat3(inverse(cam.view()))) * inverse(cam.projection());

        static int size = 7;
        static int frame = 0;
        static int max_samples = 4;
        static int sample_blend_offset = 0;
        static bool show_grid = true;

        const int size_x = 1 << size;
        const int size_y = 1 << size;
        const int count_x = ceil(800.f / size_x);
        const int count_y = ceil(800.f / size_y);

        int selected = -1;
        double mx, my;
        glfwGetCursorPos(*main_window, &mx, &my);
        glm::vec2 uv = (glm::vec2(mx, my) / glm::vec2(800, 800) - 0.5f) * 2.f;
        uv.y = - uv.y;
        glm::vec3 direction = glm::vec3(camera_matrix * glm::vec4(uv, 0, 1));
        glm::vec3 origin = cam.transform.position;
        glm::vec3 inv_origin = inverse(static_cast<glm::mat4>(trafo)) * glm::vec4(origin, 1);
        glm::vec3 inv_dir = inverse(transpose(inverse(static_cast<glm::mat4>(trafo)))) * glm::vec4(direction, 0);
        const float grip_width = 0.1f;
        if(!(moving+1) && intersect_bounds(inv_origin, inv_dir, glm::vec3(-grip_width, -grip_width, 0.f), glm::vec3(grip_width, grip_width, 1.f), 100000000.f))
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

        if(selected+1 && glfwGetMouseButton(*main_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            moving = selected;
            int mv = selected==0 ? 2 : (selected == 1 ? 1 : 0);
            glm::vec3 plane_orig = trafo.position;
            glm::vec3 plane_norm(0.f);
            plane_norm[(mv+1)%3] = 1;
            trafo.position[(mv) % 3] = abs(dot(plane_norm, direction)) > 1e-6 ?
                (origin + direction * abs(dot(plane_orig - origin, plane_norm) / dot(plane_norm, direction)))[(mv) % 3] - 0.5f :
                trafo.position[(mv) % 3];
        }
        else if(moving + 1)
        {
            moving = -1;
        }

        float t = 0.f;
        while (t < dt)
        {
            query_time.begin();
            pp_trace->bind();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<int>("sample_blend_offset") = sample_blend_offset;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<int>("max_samples") = max_samples;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::vec3>("camera_position") = cam.transform.position;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("camera_matrix") = camera_matrix;
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("model") = static_cast<glm::mat4>(trafo);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("inv_model") = inverse(static_cast<glm::mat4>(trafo));
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("cubemap") = sampler->sample_texture(cubemap);
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("vertices") = vertex_buffer->address();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("elements") = index_buffer->address();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("bvh") = bvh_buffer->address();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("materials") = material_buffer->address();
            pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::ivec2>("offset") = glm::ivec2(size_x * (frame % count_x), size_y * (frame / count_x));
            pp_trace->dispatch(size_x, size_y);
            frame = (frame + 1) % (count_x * count_y);
            query_time.end();
            t += query_time.get_uint64() / 1'000'000'000.f;
        }

        blit_framebuffer.read_from_attachment(GL_COLOR_ATTACHMENT0);
        blit_framebuffer.blit(nullptr, gl::framebuffer::blit_rect{ 0, 0, 800, 800 }, gl::framebuffer::blit_rect{ 0, 0, 800, 800 }, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glLineWidth(2.f);
        pp_chunk.bind();
        vao.bind();
        pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("inv_resolution") = 1.f / glm::vec2(800, 800);
        pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("offset") = glm::vec2(size_x * (frame % count_x), size_y * (frame / count_x));
        pp_chunk.stage(gl::shader_type::vertex)->get_uniform<glm::vec2>("size") = glm::vec2(size_x, size_y);
        glDrawArrays(GL_LINE_STRIP, 0, 5);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        gizmo_pipeline.bind();
        gizmo_vao.bind();
        gizmo_pipeline.stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("model_view_projection") = cam.projection() * cam.view() * static_cast<glm::mat4>(trafo);
        gizmo_pipeline.stage(gl::shader_type::fragment)->get_uniform<int>("hovered") = selected;
        glDrawElements(GL_TRIANGLES, 3 * cube_icount, GL_UNSIGNED_SHORT, nullptr);

        if (show_grid)
        {
            glLineWidth(0.1f);
            glPolygonMode(GL_FRONT, GL_LINE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            gizmo_pipeline.stage(gl::shader_type::fragment)->get_uniform<int>("hovered") = -2;
            mesh_vao->bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
            glPolygonMode(GL_FRONT, GL_FILL);
            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
        }

        ImGui::Begin("Window");
        ImGui::Value("Frametime", static_cast<float>(1'000 * main_window->delta_time()));
        ImGui::DragInt("Size", &size, 0.1f, 3.f, 10.f);
        ImGui::DragInt("Max. Samples", &max_samples, 0.01f, 1.f, 100.f);
        ImGui::DragInt("Sample Blend Offset", &sample_blend_offset, 0.01f, 1.f, 100.f);
        ImGui::Checkbox("Show Mesh", &show_grid);
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0)))
        {
            res::stbi_data tex_data(malloc(800 * 800 * 4 * sizeof(float)));
            target_texture->get_texture_data(GL_RGBA, GL_FLOAT, 800 * 800 * 4 * sizeof(float), tex_data.get());

            glm::vec4* ic = static_cast<glm::vec4*>(tex_data.get());
            std::vector<glm::u8vec4> convert(800 * 800);
            for (int x = 0; x < 800; ++x)
            {
                for (int y = 0; y < 800; ++y)
                {
                    convert[y * 800 + x] = glm::clamp(ic[(800 - 1 - y) * 800 + x] * 255.f, glm::vec4(0), glm::vec4(255.f));
                }
            }

            constexpr const char *f2s[1] = {
                "*.png"
            };
            if (const auto dst = tinyfd_saveFileDialog("Save output", "../res/", 1, f2s, "*.png"))
                stbi_write_png(dst, 800, 800, 4, convert.data(), 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Open", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            constexpr const char *fs[5] = {
                "*.dae", "*.fbx", "*.obj", "*.stl", "*.ply"
            };
            if (const auto src = tinyfd_openFileDialog("Open Mesh", "../res/", 5, fs, "mesh", false))
            {
                geometry = res::load_geometry(src);
                vertices = std::move(geometry.meshes.get_by_index(0).vertices);
                indices = std::move(geometry.meshes.get_by_index(0).indices);
                bvh = jpu::bvh<3>();
                bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
                vertex_buffer = jpu::make_ref<gl::buffer>(vertices);
                index_buffer = jpu::make_ref<gl::buffer>(indices);
                bvh_buffer = jpu::make_ref<gl::buffer>(bvh.pack());
                mesh_vao = jpu::make_ref<gl::vertex_array>();
                mesh_vao->set_element_buffer(*index_buffer);
                mesh_vao->add_binding(gl::vertex_attribute_binding(0, *vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false));
            }
        }

        if(ImGui::CollapsingHeader("Material"))
        {
            auto&& item = material_buffer->data_as<material>();
            ImGui::DragFloat("Roughness", &item->roughness_sqrt, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("Glass", &item->glass, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("IOR", &item->ior, 0.01f, 0.f, 100.f);
            ImGui::DragFloat("Ex. Coeff.", &item->extinction_coefficient, 0.01f, 0.f, 100.f);
            ImGui::Spacing();
            ImGui::ColorEdit3("Color", &item->base_color[0]);
            ImGui::ColorEdit3("Reflection Tint", &item->reflection_tint[0]);
            ImGui::ColorEdit3("Glass Tint", &item->glass_tint[0]);
        }

        ImGui::End();
    }

    return 0;
}