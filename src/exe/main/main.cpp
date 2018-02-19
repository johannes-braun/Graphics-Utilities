#include "io/window.hpp"

#include <random>
#include <numeric>

#include <glm/ext.hpp>
#include <jpu/memory>
#include <jpu/data>
#include <jpu/geometry>
#include <jpu/log>

#include "scene.hpp"

#include "opengl/vertex_array.hpp"
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

jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

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

    main_renderer->resize(width, height, samples);
}

int main(int count, const char** arguments)
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    main_renderer = std::make_unique<gfx::renderer>(full_resolution.x, full_resolution.y, current_samples);
    resize(1280, 720, 1 << 2, false);

    glfwSwapInterval(1);
    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
        {
            main_renderer->reload_pipelines();
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
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
        resize(w, h, current_samples, is_fullscreen);
    });



    auto graphics_pipeline = graphics_pipelines.push("Default Graphics Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    graphics_pipeline->use_stages(
        jpu::make_ref<gl::shader>(gl::shader_root / "gbuffer/gbuffer.vert"),
        jpu::make_ref<gl::shader>(gl::shader_root / "gbuffer/gbuffer.frag")
    );

    auto cylinder = res::load_geometry("../res/bunny.dae");
    jpu::bvh<3> obj_bvh;
    obj_bvh.assign_to(cylinder.meshes.get_by_index(0).indices, cylinder.meshes.get_by_index(0).vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    const auto index_buffer = jpu::make_ref<gl::buffer>(cylinder.meshes.get_by_index(0).indices);
    const auto vertex_buffer = jpu::make_ref<gl::buffer>(cylinder.meshes.get_by_index(0).vertices);
    const auto bvh_buffer = jpu::make_ref<gl::buffer>(obj_bvh.node_count * sizeof(obj_bvh.nodes[0]) + sizeof(obj_bvh.attributes), gl::buffer_flag_bits::dynamic_storage);
    bvh_buffer->assign(&obj_bvh.attributes, 1);
    bvh_buffer->assign(obj_bvh.data(), obj_bvh.node_count, sizeof(obj_bvh.attributes));

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

    const auto mesh_buffer = jpu::make_ref<gl::buffer>(sizeof(mesh), gl::buffer_flag_bits::map_dynamic_persistent);
    mesh_buffer->at<mesh>(0).num_vertices = cylinder.meshes.get_by_index(0).vertices.size();
    mesh_buffer->at<mesh>(0).num_elements = cylinder.meshes.get_by_index(0).indices.size();
    mesh_buffer->at<mesh>(0).elements = index_buffer->address();
    mesh_buffer->at<mesh>(0).vertices = vertex_buffer->address();
    mesh_buffer->at<mesh>(0).data = bvh_buffer->address();
    mesh_buffer->bind(8, GL_SHADER_STORAGE_BUFFER);

    auto vertex_vao = jpu::make_ref<gl::vertex_array>();
    vertex_vao->add_bindings({
        gl::vertex_attribute_binding(0, *vertex_buffer, 0, 4, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false),
        gl::vertex_attribute_binding(1, *vertex_buffer, 0, 4, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, uv), false),
        gl::vertex_attribute_binding(2, *vertex_buffer, 0, 4, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, normal), true)
        });
    vertex_vao->set_element_buffer(*index_buffer);

    const auto generic_vao = jpu::make_ref<gl::vertex_array>();

    const auto scene_buffer = jpu::make_ref<gl::buffer>(sizeof(geo::scene), gl::buffer_flag_bits::map_dynamic_persistent);
    scene_buffer->bind(0, GL_UNIFORM_BUFFER);

    res::transform transform;
    transform.position = glm::vec3(4.f, 4.f, 4.f);

    auto cubemap_pipeline = graphics_pipelines.push("Cubemap Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    cubemap_pipeline->use_stages(
        jpu::make_ref<gl::shader>(gl::shader_root / "cubemap/cubemap.vert"),
        jpu::make_ref<gl::shader>(gl::shader_root / "cubemap/cubemap.frag")
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0, 0, 0, 0);
    glClearDepth(0);

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

    const auto load_texture = [&](auto path, auto internal_format, auto format, auto type)
    {
        auto image = res::load_image(path, type == GL_FLOAT ? res::image_type::signed_float : res::image_type::unsigned_byte,
            format == GL_RED ? res::image_components::grey : (format == GL_RG ? res::image_components::grey_alpha : (format == GL_RGB ? res::image_components::rgb : res::image_components::rgb_alpha)));
        auto texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        texture->storage_2d(image.width, image.height, internal_format);
        texture->assign_2d(format, type, image.data.get());
        return texture;
    };

    auto texture_col    = load_texture("../res/bricks/brick.png", GL_RGBA8, GL_RGBA, GL_FLOAT);
    auto texture_dis    = load_texture("../res/bricks/brick_bump.png", GL_R32F, GL_RED, GL_FLOAT);
    auto texture_nor    = load_texture("../res/bricks/brick_normal.png", GL_RGB16F, GL_RGB, GL_FLOAT);
    auto logo           = load_texture("../res/ui/logo.png", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    auto ic_image       = load_texture("../res/ui/icons/ic_image.png", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    io::camera cam;
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

        ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Image(reinterpret_cast<ImTextureID>(sampler->sample_texture(logo)), ImVec2(32, 32));
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
            if (ImGui::Button("Reload All", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0)))
                for (auto&& p : graphics_pipelines)
                    p->reload_stages();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
            ImGui::PushStyleColor(ImGuiCol_Button, 0xffaa5511);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xffff8620);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xffbb3311);
            if (ImGui::Button("Force Reload All", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                for (auto&& p : graphics_pipelines)
                    p->reload_stages(true);
            ImGui::PopStyleColor(4);
        }

        ImGui::End();

        cam_controller.update(cam, *main_window, main_window->delta_time());
        listener->set_position(cam.transform.position);
        listener->set_orientation(cam.transform.rotation);

        auto&& scene = scene_buffer->at<geo::scene>(0);
        scene.set_view(cam.view());
        scene.set_projection(cam.projection());
        scene.set_time(glfwGetTime());
        glFinish();

        main_renderer->bind();

        cubemap_pipeline->bind();
        cubemap_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("cubemap_matrix") =
            glm::mat4(glm::mat3(scene.get_inv_view())) * scene.get_inv_projection();
        cubemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("map") = sampler->sample_texture(cubemap);
        cubemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::vec4>("tint") = glm::vec4(0.9f, 0.96f, 1.f, 1.f);
        generic_vao->bind();
        int mask;
        glGetIntegerv(GL_DEPTH_WRITEMASK, &mask);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDepthMask(mask);

        //transform.rotation *= glm::angleAxis(glm::radians<float>(30.f * static_cast<float>(win->delta_time())), glm::vec3(0, 1, 0));
        graphics_pipeline->bind();
        graphics_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("model_matrix") = transform;
        graphics_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("normal_matrix") = glm::mat4(glm::mat3(glm::transpose(glm::inverse(static_cast<glm::mat4>(transform)))));
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("my_texture") = sampler->sample_texture(texture_col);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("texture_depth") = sampler->sample_texture(texture_dis);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("texture_normal") = sampler->sample_texture(texture_nor);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("cube_map") = sampler->sample_texture(cubemap);
        vertex_vao->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cylinder.meshes.get_by_index(0).indices.size()), GL_UNSIGNED_INT, nullptr);

        main_renderer->draw(main_window->delta_time());
    }
}

