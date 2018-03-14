#include "io/window.hpp"

#include <random>

#include <glm/ext.hpp>
#include <jpu/memory.hpp>
#include <jpu/data.hpp>
#include <jpu/geometry.hpp>

#include "scene.hpp"

#include "opengl/buffer.hpp"
#include "opengl/shader.hpp"
#include "opengl/pipeline.hpp"
#include "opengl/texture.hpp"
#include "opengl/framebuffer.hpp"

#include "io/camera.hpp"

#include "res/image.hpp"
#include "res/audio.hpp"
#include "res/geometry.hpp"

#include "openal/context.hpp"
#include "openal/device.hpp"
#include "openal/source.hpp"
#include "openal/buffer.hpp"
#include "openal/listener.hpp"

#include "framework/renderer.hpp"

#include <stb_image.h>
#include "framework/gizmo.hpp"

jpu::ref_ptr<io::window> main_window;
std::unique_ptr<gfx::renderer> main_renderer;

bool is_fullscreen = false;

int current_samples;
glm::ivec2 full_resolution;
glm::ivec2 quarter_resolution;

void resize(const int width, const int height, const int samples, const bool fullscreen)
{
    if (width * height * samples == 0)
        return;

    const bool fullscreen_changed = is_fullscreen != fullscreen;

    full_resolution = { width, height };
    quarter_resolution = full_resolution / 4;
    current_samples = samples;
    is_fullscreen = fullscreen;

    if (fullscreen_changed)
    {
        if (is_fullscreen)
        {
            io::monitor primary_monitor;
            main_window->set_monitor(primary_monitor);
        }
        else
        {
            main_window->set_monitor({});
        }
    }

    if(main_renderer)
        main_renderer->resize(width, height, samples);
    else
        main_renderer = std::make_unique<gfx::renderer>(full_resolution.x, full_resolution.y, current_samples);
    glViewportIndexedf(0, 0, 0, float(width), float(height));
    glScissorIndexed(0, 0, 0, width, height);
}


struct bli
{
    int a;
    int b;
    float c;
};

int main()
{
    gl::shader::set_include_directories("../shaders");

    res::image icon = load_image("../res/ui/logo.png", res::image_type::u8, res::RGBA);
    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::RGBA);

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->set_icon(icon.width, icon.height, icon.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    main_window->set_max_framerate(60.f);
    resize(1280, 720, 1 << 2, false);

    main_window->callbacks->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
        {
            main_renderer->reload_pipelines();
        }
    });
    main_window->callbacks->framebuffer_size_callback.add([](GLFWwindow*, int w, int h) {
        resize(w, h, current_samples, is_fullscreen);
    });

    auto cylinder = res::load_geometry("../res/bunny.dae");

    gl::buffer<uint32_t> index_buffer(cylinder.meshes.get_by_index(0).indices.begin(), cylinder.meshes.get_by_index(0).indices.end(), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    gl::buffer<res::vertex> vertex_buffer(cylinder.meshes.get_by_index(0).vertices.begin(), cylinder.meshes.get_by_index(0).vertices.end());
    index_buffer.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
    jpu::bvh<3> obj_bvh;
    obj_bvh.assign_to(index_buffer, cylinder.meshes.get_by_index(0).vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    index_buffer.flush();
    std::vector<gl::byte> bvh_bytes = obj_bvh.pack();
    gl::buffer<gl::byte> bvh_buffer(bvh_bytes.begin(), bvh_bytes.end());

    struct mesh
    {
        uint32_t num_vertices;
        uint32_t num_elements;

        uintptr_t elements;
        uintptr_t vertices;

        uintptr_t material;
        uintptr_t data;

    private:
        int _padding[2];

    public:
        glm::mat4 transform;
        glm::mat4 inverse_transform;
    };

    gl::buffer<mesh> mesh_buffer(1, GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    mesh_buffer.map(GL_MAP_WRITE_BIT);
    mesh_buffer[0].num_vertices = static_cast<uint32_t>(cylinder.meshes.get_by_index(0).vertices.size());
    mesh_buffer[0].num_elements = static_cast<uint32_t>(cylinder.meshes.get_by_index(0).indices.size());
    mesh_buffer[0].elements = index_buffer.handle();
    mesh_buffer[0].vertices = vertex_buffer.handle();
    mesh_buffer[0].data = bvh_buffer.handle();
    mesh_buffer.bind(GL_SHADER_STORAGE_BUFFER, 8);

    gl::buffer<geo::scene> scene_buffer(1, GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
    scene_buffer.bind(GL_UNIFORM_BUFFER, 0);
    scene_buffer.map(GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

    res::transform transform;
    transform.position = glm::vec3(4.f, 4.f, 4.f);

    gfx::gizmo gizmo;
    gizmo.transform = &transform;
    
    gl::pipeline gp;
    gp[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("gbuffer/gbuffer.vert");
    gp[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("gbuffer/gbuffer.frag");

    gl::pipeline cubemap_pipeline;
    cubemap_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("cubemap/cubemap.vert");
    cubemap_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("cubemap/cubemap.frag");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);

    const gl::sampler sampler;

    int w, h, c; stbi_info("../res/hdr/posx.hdr", &w, &h, &c);
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.generate_mipmaps();

    const auto load_texture = [&](auto path, auto internal_format, auto format, auto type) -> gl::texture
    {
        auto image = res::load_image(path, type == GL_FLOAT ? res::image_type::f32 : res::image_type::u8,
            format == GL_RED ? res::Grey : (format == GL_RG ? res::RG : (format == GL_RGB ? res::RGB : res::RGBA)));
        gl::texture texture(GL_TEXTURE_2D, image.width, image.height, internal_format);
        texture.assign(format, type, image.data.get());
        return texture;
    };

    const auto texture_col    = load_texture("../res/bricks/brick.png", GL_RGBA8, GL_RGBA, GL_FLOAT);
    const auto texture_dis    = load_texture("../res/bricks/brick_bump.png", GL_R32F, GL_RED, GL_FLOAT);
    const auto texture_nor    = load_texture("../res/bricks/brick_normal.png", GL_RGB16F, GL_RGB, GL_FLOAT);
    const auto logo           = load_texture("../res/ui/logo.png", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    io::camera cam;
    cam.inverse_z = true;
    io::default_cam_controller cam_controller;

    // Init audio
    auto device = jpu::make_ref<al::default_device>();
    const auto context = jpu::make_ref<al::context>(device);
    context->make_current();

    // Init listener
    const auto listener = jpu::make_ref<al::listener>();
    listener->make_current();

    // Init buffer & load .ogg
    const auto buffer = jpu::make_ref<al::buffer>();
    auto audio_file = res::load_audio("../res/audio/piano_mono.wav");
    buffer->assign(audio_file.data.get(), audio_file.data_size, audio_file.format, audio_file.sample_rate);

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    // Init sound source and play buffer
    const auto source = jpu::make_ref<al::source>();
    source->set(AL_POSITION, glm::vec3(0, 0, 0));
    source->set(AL_LOOPING, AL_TRUE);
    source->set(AL_PITCH, 1.f);
    source->set(AL_GAIN, 0.4f);
    source->set(AL_MIN_GAIN, 0.f);
    source->set(AL_MAX_GAIN, 1.f);
    source->set(AL_REFERENCE_DISTANCE, 3.f);
    source->set(AL_ROLLOFF_FACTOR, 0.8f);
    source->set(AL_MAX_DISTANCE, 10.f);
    source->set(AL_VELOCITY, glm::vec3(0, 0, 0));
    source->set(AL_BUFFER, *buffer);
    source->play();

    static double time = 0;
    static float frametime = 0;
    static float fps = 0;

    main_renderer->set_clear_color({ 0.1f, 0.1f, 0.1f, 1 });
    while (main_window->update())
    {
        time += main_window->delta_time();
        if (static auto frame = 0; frame++, time >= 1)
        {
            frametime = static_cast<float>(1000 * time / frame);
            fps = 1000.f / frametime;
            time = 0;
            frame = 0;
        }

        {
            ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoTitleBar);
            ImGui::Image(reinterpret_cast<ImTextureID>(sampler.sample(logo)), ImVec2(32, 32));
            ImGui::SameLine();
            const auto cur_pos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cur_pos.x, cur_pos.y + 6));
            ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[3]);
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
            ImGui::Text("Graphics Framework");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
            ImGui::Text("V1.0.0");
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
            ImGui::Text("%.1f fps at %.3f ms/f", fps, frametime);
            ImGui::PopStyleColor();

            ImGui::Spacing();

            ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
            if (bool b = ImGui::CollapsingHeader("Application"); ImGui::PopFont(), b)
            {
                ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
                ImGui::Text("Screen");
                ImGui::PopStyleColor();
                ImGui::PopFont();
                ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
                ImGui::TextWrapped("Settings concerning the screen resolution and framebuffer attachments.");
                ImGui::PopStyleColor();

                static glm::ivec2 res(full_resolution);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth()*0.6f);
                ImGui::DragInt2("##scale_window", &res[0], 1, 0, 8192);
                ImGui::PopItemWidth();
                ImGui::SameLine();
                static bool fscr = is_fullscreen;
                ImGui::Checkbox("Fullscreen", &fscr);

                if (fscr != is_fullscreen || res != full_resolution)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
                    ImGui::TextWrapped("Click \"Apply\" for your changes to take effect.");
                    ImGui::PopStyleColor();
                }

                static int curr_msaa = 2;
                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                if (ImGui::Combo("MSAA", &curr_msaa, "  1x MSAA (OFF) \0 2x MSAA \0 4x MSAA \0 8x MSAA \0 16x MSAA \0"))
                    resize(full_resolution.x, full_resolution.y, 1 << curr_msaa, is_fullscreen);
                ImGui::PopItemWidth();

                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                {
                    resize(full_resolution.x, full_resolution.y, current_samples, fscr);
                    glfwSetWindowSize(*main_window, res.x, res.y);
                }
                ImGui::Spacing();
                ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
                ImGui::Text("General");
                ImGui::PopStyleColor();
                ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
                ImGui::TextWrapped("General application controls.");
                ImGui::PopStyleColor();
                ImGui::PopFont();
                if (ImGui::Button("Quit Application", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                    main_window->close();
            }

            ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
            if (bool b = ImGui::CollapsingHeader("Shaders"); ImGui::PopFont(), b)
            {
               /* if (ImGui::Button("Reload All", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0)))
                    for (auto&& p : graphics_pipelines)
                        p->reload_stages();*/
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
                ImGui::PushStyleColor(ImGuiCol_Button, 0xffaa5511);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xffff8620);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xffbb3311);
                /*if (ImGui::Button("Force Reload All", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                    for (auto&& p : graphics_pipelines)
                        p->reload_stages(true);*/
                ImGui::PopStyleColor(4);
            }

            ImGui::End();
        }

        cam_controller.update(cam, *main_window, main_window->delta_time());
        listener->set_position(cam.transform.position);
        listener->set_orientation(cam.transform.rotation);

        scene_buffer[0].set_view(cam.view());
        scene_buffer[0].set_projection(cam.projection());
        scene_buffer[0].set_time(float(glfwGetTime()));

        main_renderer->bind();

        glDepthMask(false);
        cubemap_pipeline.bind();
        for (int i = 0; i<16; ++i)
            glDisableVertexAttribArray(i);
        cubemap_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("cubemap_matrix") = glm::mat4(glm::mat3(scene_buffer[0].get_inv_view())) * scene_buffer[0].get_inv_projection();
        cubemap_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("map") = sampler.sample(cubemap);
        cubemap_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint") = glm::vec4(0.9f, 0.96f, 1.f, 1.f);
        cubemap_pipeline.draw(GL_TRIANGLES, 3);
        glDepthMask(true);

        gp.bind();
        gp[GL_VERTEX_SHADER]->uniform<glm::mat4>("model_matrix") = transform;
        gp[GL_VERTEX_SHADER]->uniform<glm::mat4>("normal_matrix") = glm::mat4(glm::mat3(transpose(inverse(static_cast<glm::mat4>(transform)))));
        gp[GL_FRAGMENT_SHADER]->uniform<uint64_t>("my_texture") = sampler.sample(texture_col);
        gp[GL_FRAGMENT_SHADER]->uniform<uint64_t>("texture_depth") = sampler.sample(texture_dis);
        gp[GL_FRAGMENT_SHADER]->uniform<uint64_t>("texture_normal") = sampler.sample(texture_nor);
        gp[GL_FRAGMENT_SHADER]->uniform<uint64_t>("cube_map") = sampler.sample(cubemap);
        gp.bind_uniform_buffer(0, scene_buffer);
        gp.bind_attribute(0, vertex_buffer, 3, GL_FLOAT, offsetof(res::vertex, position));
        gp.bind_attribute(1, vertex_buffer, 3, GL_FLOAT, offsetof(res::vertex, normal));
        gp.bind_attribute(2, vertex_buffer, 2, GL_FLOAT, offsetof(res::vertex, uv));
        gp.draw(GL_TRIANGLES, index_buffer, GL_UNSIGNED_INT);

        main_renderer->draw(main_window->delta_time());

        gizmo.update(*main_window, cam.view(), cam.projection());
        gizmo.render();
    }
}

