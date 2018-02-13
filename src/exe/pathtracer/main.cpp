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

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::compute_pipeline>> compute_pipelines;
uint32_t current_target = 0;
jpu::ref_ptr<gl::texture> target_textures[2];
jpu::ref_ptr<gl::texture> rad_texture;

int main(int argc, const char** args)
{
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "Grid");
    main_window->load_icon("../res/ui/logo.png");
    main_window->set_cursor(new io::cursor("../res/cursor.png", 0, 0));
    main_window->limit_framerate(120.f);
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

    target_textures[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_textures[0]->storage_2d(1280, 720, GL_RGBA16F);
    const gl::image target_image_01(target_textures[0], 0, false, 0, GL_RGBA16F, GL_READ_WRITE);
    target_textures[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_textures[1]->storage_2d(1280, 720, GL_RGBA16F);
    const gl::image target_image_02(target_textures[1], 0, false, 0, GL_RGBA16F, GL_READ_WRITE);
    rad_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    rad_texture->storage_2d(1280, 720, GL_RGBA16F);
    const gl::image rad_image(rad_texture, 0, false, 0, GL_RGBA16F, GL_READ_WRITE);

    gl::framebuffer blit_framebuffer;
    blit_framebuffer.attach(GL_COLOR_ATTACHMENT0, target_textures[0]);
    blit_framebuffer.attach(GL_COLOR_ATTACHMENT1, target_textures[1]);

    gl::buffer ray_buffer(1280 * 720 * 3 * sizeof(glm::vec4));
    const auto pp_generate = compute_pipelines.push("Generate", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/generate.comp")));
    const auto pp_traverse = compute_pipelines.push("Traverse", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/traverse.comp")));
    const auto pp_shade = compute_pipelines.push("Shade", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/shade.comp")));
    const auto pp_copy_back = compute_pipelines.push("Copy Back", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/copy_back.comp")));

    const auto pp_trace = compute_pipelines.push("Trace", jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "pathtracer/trace.comp")));

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
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    // Test out mesh
    const auto geometry = res::load_geometry("../res/bunny.dae");
    const std::vector<res::vertex> vertices = std::move(geometry.meshes.get_by_index(0).vertices);
    std::vector<uint32_t> indices = std::move(geometry.meshes.get_by_index(0).indices);
    jpu::bvh<3> bvh;
    bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    gl::buffer vertex_buffer(vertices);
    gl::buffer index_buffer(indices);
    gl::buffer bvh_buffer(bvh.pack());

    gl::query query_generate(GL_TIME_ELAPSED);
    gl::query query_traverse(GL_TIME_ELAPSED);
    gl::query query_shade(GL_TIME_ELAPSED);
    gl::query query_copy_back(GL_TIME_ELAPSED);

    while (main_window->update())
    {
        ctrl.update(cam, *main_window, main_window->delta_time());

        glm::mat4 camera_matrix = glm::mat4(glm::mat3(inverse(cam.view()))) * inverse(cam.projection());

       /* ray_buffer.bind(0, GL_SHADER_STORAGE_BUFFER);

        query_generate.begin();
        pp_generate->bind();
        pp_generate->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image_02;
        pp_generate->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
        pp_generate->stage(gl::shader_type::compute)->get_uniform<glm::vec3>("camera_position") = cam.transform.position;
        pp_generate->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("camera_matrix") = camera_matrix;
        pp_generate->dispatch(1280 * 720);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        query_generate.end();

        glm::vec4 clear{ 1 };
        glClearTexImage(*rad_texture, 0, GL_RGBA, GL_FLOAT, &clear);

        for (int bounce = 0; bounce < 4; ++bounce)
        {
            query_traverse.begin();
            pp_traverse->bind();
            pp_traverse->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("vertices") = vertex_buffer.address();
            pp_traverse->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("elements") = index_buffer.address();
            pp_traverse->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("bvh") = bvh_buffer.address();
            pp_traverse->dispatch(1280 * 720);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            query_traverse.end();

            query_shade.begin();
            pp_shade->bind();
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image_02;
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uint64_t>("rad_image") = rad_image;
            pp_shade->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uint64_t>("cubemap") = sampler->sample_texture(cubemap);
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("vertices") = vertex_buffer.address();
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("elements") = index_buffer.address();
            pp_shade->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("bvh") = bvh_buffer.address();
            pp_shade->dispatch(1280 * 720);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            query_shade.end();
        }

        query_copy_back.begin();
        pp_copy_back->bind();
        pp_copy_back->stage(gl::shader_type::compute)->get_uniform<uint64_t>("src_image") = target_image_02;
        pp_copy_back->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image_01;
        pp_copy_back->dispatch(1280, 720);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        query_copy_back.end();*/

        pp_trace->bind();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image_01;
        pp_trace->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
        pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::vec3>("camera_position") = cam.transform.position;
        pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("camera_matrix") = camera_matrix;
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("cubemap") = sampler->sample_texture(cubemap);
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("vertices") = vertex_buffer.address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("elements") = index_buffer.address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("bvh") = bvh_buffer.address();
        pp_trace->dispatch(1280, 720);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        blit_framebuffer.read_from_attachment(GL_COLOR_ATTACHMENT0);
        blit_framebuffer.blit(nullptr, gl::framebuffer::blit_rect{ 0, 0, 1280, 720 }, gl::framebuffer::blit_rect{ 0, 0, 1280, 720 }, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        ImGui::Begin("Window");
        ImGui::Value("Frametime", static_cast<float>(1'000 * main_window->delta_time()));
        ImGui::Value("Generate", query_generate.get_uint64() / 1'000'000.f);
        ImGui::Value("Traverse", query_traverse.get_uint64() / 1'000'000.f);
        ImGui::Value("Shade", query_shade.get_uint64() / 1'000'000.f);
        ImGui::Value("Copy Back", query_copy_back.get_uint64() / 1'000'000.f);
        if(ImGui::Button("Open"))
        {
            constexpr const char *fs[2] = {
                "*.jpg", "*.png"
            };
            log_i << tinyfd_openFileDialog("Test", "../res", 2, fs, "Hm", false);
        }
        ImGui::End();
    }

    return 0;
}