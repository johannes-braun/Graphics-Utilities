#include <random>
#include <jpu/memory>
#include <jpu/data>
#include "io/window.hpp"
#include "opengl/framebuffer.hpp"
#include "io/camera.hpp"
#include "res/geometry.hpp"
#include "jpu/impl/geometry/bvh/bvh.hpp"
#include "res/image.hpp"

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::compute_pipeline>> compute_pipelines;
jpu::ref_ptr<gl::texture> target_texture;

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

    target_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    target_texture->storage_2d(1280, 720, GL_RGBA16F);
    const gl::image target_image(target_texture, 0, false, 0, GL_RGBA16F, GL_READ_WRITE);

    gl::framebuffer blit_framebuffer;
    blit_framebuffer.attach(GL_COLOR_ATTACHMENT0, target_texture);

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


    jpu::ref_ptr<gl::texture> random_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    random_texture->storage_2d(512, 512, GL_RGBA16F, 1);
    std::vector<float> random_pixels(512 * 512 * 4);
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    std::generate(random_pixels.begin(), random_pixels.end(),[&] { return dist(gen); });
    random_texture->assign_2d(GL_RGBA, GL_FLOAT, random_pixels.data());

    // Test out mesh
    const auto geometry = res::load_geometry("../res/bunny_decimated.dae");
    const std::vector<res::vertex> vertices = std::move(geometry.meshes.get_by_index(0).vertices);
    std::vector<uint32_t> indices = std::move(geometry.meshes.get_by_index(0).indices);
    jpu::bvh<3> bvh;
    bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    gl::buffer vertex_buffer(vertices);
    gl::buffer index_buffer(indices);
    gl::buffer bvh_buffer(bvh.pack());

    while (main_window->update())
    {
        ctrl.update(cam, *main_window, main_window->delta_time());

        ImGui::Begin("Window");
        ImGui::Value("Frametime", static_cast<float>(1'000 * main_window->delta_time()));
        ImGui::End();

        pp_trace->bind();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("target_image") = target_image;
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("random_texture") = random_texture->address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<float>("random_gen") = dist(gen);
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uint64_t>("cubemap") = sampler->sample_texture(cubemap);
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("vertices") = vertex_buffer.address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("elements") = index_buffer.address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<uintptr_t>("bvh") = bvh_buffer.address();
        pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::vec3>("camera_position") = cam.transform.position;
        pp_trace->stage(gl::shader_type::compute)->get_uniform<glm::mat4>("camera_matrix") = glm::mat4(glm::mat3(inverse(cam.view()))) * inverse(cam.projection());
        pp_trace->dispatch(1280, 720);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        blit_framebuffer.read_from_attachment(GL_COLOR_ATTACHMENT0);
        blit_framebuffer.blit(nullptr, gl::framebuffer::blit_rect{ 0, 0, 1280, 720 }, gl::framebuffer::blit_rect{ 0, 0, 1280, 720 }, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    return 0;
}