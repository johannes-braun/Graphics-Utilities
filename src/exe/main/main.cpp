#include <random>
#include <numeric>

#include <glm/ext.hpp>
#include <jpu/memory>
#include <jpu/data>
#include <jpu/geometry>
#include <jpu/log>

#include "geo/vertex.hpp"
#include "geo/transform.hpp"
#include "geo/scene.hpp"

#include "opengl/vertex_array.hpp"
#include "opengl/buffer.hpp"
#include "opengl/shader.hpp"
#include "opengl/pipeline.hpp"
#include "opengl/texture.hpp"
#include "opengl/framebuffer.hpp"

#include "io/camera.hpp"
#include "io/gui.hpp"
#include "io/window.hpp"

#include "res/image.hpp"
#include "res/audio.hpp"
#include "res/geometry.hpp"

#include "openal/context.hpp"
#include "openal/device.hpp"
#include "openal/source.hpp"
#include "openal/buffer.hpp"
#include "openal/listener.hpp"

#include <stb_image.h>

jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

jpu::ref_ptr<io::window> main_window;

bool is_fullscreen = false;
bool is_iconified = false;

jpu::ref_ptr<gl::framebuffer> blur_framebuffer;
jpu::ref_ptr<gl::framebuffer> framebuffer;
jpu::ref_ptr<gl::framebuffer> resolve_framebuffer;
std::array<jpu::ref_ptr<gl::texture>, 2> quarter_size_attachments;
std::array<jpu::ref_ptr<gl::texture>, 4> full_size_attachments;
std::array<jpu::ref_ptr<gl::texture>, 2> msaa_attachments;

int current_samples;
glm::ivec2 full_resolution;
glm::ivec2 quarter_resolution;

void resize(const int width, const int height, const int samples, const bool fullscreen)
{
    if (width * height * samples == 0)
        return;

    const bool samples_changed = (current_samples != samples);
    const bool resolution_changed = full_resolution != glm::ivec2{ width, height };
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

    if (resolution_changed)
    {
        quarter_size_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        quarter_size_attachments[0]->storage_2d(quarter_resolution.x, quarter_resolution.y, GL_R11F_G11F_B10F, 1);
        blur_framebuffer->attach(GL_COLOR_ATTACHMENT0, quarter_size_attachments[0]);

        quarter_size_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        quarter_size_attachments[1]->storage_2d(quarter_resolution.x, quarter_resolution.y, GL_R11F_G11F_B10F, 1);
        blur_framebuffer->attach(GL_COLOR_ATTACHMENT1, quarter_size_attachments[1]);

        full_size_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        full_size_attachments[0]->storage_2d(full_resolution.x, full_resolution.y, GL_R11F_G11F_B10F, 3);
        resolve_framebuffer->attach(GL_COLOR_ATTACHMENT0, full_size_attachments[0]);

        full_size_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        full_size_attachments[1]->storage_2d(full_resolution.x, full_resolution.y, GL_R11F_G11F_B10F, 1);
        resolve_framebuffer->attach(GL_COLOR_ATTACHMENT1, full_size_attachments[1]);

        full_size_attachments[2] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        full_size_attachments[2]->storage_2d(full_resolution.x, full_resolution.y, GL_R11F_G11F_B10F, 1);
        resolve_framebuffer->attach(GL_COLOR_ATTACHMENT2, full_size_attachments[2]);

        full_size_attachments[3] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        full_size_attachments[3]->storage_2d(full_resolution.x, full_resolution.y, GL_R11F_G11F_B10F, 1);
        resolve_framebuffer->attach(GL_COLOR_ATTACHMENT3, full_size_attachments[3]);
    }

    if (resolution_changed || samples_changed)
    {
        msaa_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::multisample_2d);
        msaa_attachments[0]->storage_2d_multisample(full_resolution.x, full_resolution.y, samples, GL_RGBA16F);
        framebuffer->attach(GL_COLOR_ATTACHMENT0, msaa_attachments[0]);

        msaa_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::multisample_2d);
        msaa_attachments[1]->storage_2d_multisample(full_resolution.x, full_resolution.y, samples, GL_RGBA16F);
        framebuffer->attach(GL_COLOR_ATTACHMENT1, msaa_attachments[1]);

        framebuffer->use_renderbuffer_multisample(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, full_resolution.x, full_resolution.y, samples);
    }
}

int main(int count, const char** arguments)
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");

    blur_framebuffer = jpu::make_ref<gl::framebuffer>();
    framebuffer = jpu::make_ref<gl::framebuffer>();
    resolve_framebuffer = jpu::make_ref<gl::framebuffer>();
    resize(1280, 720, 1 << 2, false);

    glfwSwapInterval(1);
    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
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

    glfwSetWindowIconifyCallback(*main_window, [](GLFWwindow*, int ico) {
        is_iconified = static_cast<bool>(ico);
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

    geo::transform transform;
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
    sampler->set(GL_TEXTURE_MAX_ANISOTROPY, 16);
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

    auto texture_col    = res::load_texture("../res/bricks/brick.png", GL_RGBA8, GL_RGBA, GL_FLOAT);
    auto texture_dis    = res::load_texture("../res/bricks/brick_bump.png", GL_R32F, GL_RED, GL_FLOAT);
    auto texture_nor    = res::load_texture("../res/bricks/brick_normal.png", GL_RGB16F, GL_RGB, GL_FLOAT);
    auto logo           = res::load_texture("../res/ui/logo.png", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    auto ic_image       = res::load_texture("../res/ui/icons/ic_image.png", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    auto random_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    random_texture->storage_2d(512, 512, GL_RGBA16F, 1);
    std::vector<float> random_pixels(512 * 512 * 4);
    std::generate(random_pixels.begin(), random_pixels.end(), 
        [gen = std::mt19937(), dist = std::uniform_real_distribution<float>(0.f, 1.f)]() mutable { return dist(gen); });
    random_texture->assign_2d(GL_RGBA, GL_FLOAT, random_pixels.data());

    io::Camera cam;
    uint32_t temporal_target = GL_COLOR_ATTACHMENT0;
    uint32_t temporal_buffer = GL_COLOR_ATTACHMENT2;
    uint32_t other_temporal_buffer = GL_COLOR_ATTACHMENT3;

    const auto luminance_compute_shader = jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/tonemap/sample_luminance.comp");
    const auto luminance_compute_pipeline = jpu::make_ref<gl::compute_pipeline>(luminance_compute_shader.get());
    const auto sample_buffer = jpu::make_ref<gl::buffer>(std::array<float, 32>{}, gl::buffer_flag_bits::map_dynamic_persistent_read);
    sample_buffer->bind(1, GL_SHADER_STORAGE_BUFFER);
    float luma = 1;

    const auto screen_vertex_shader = jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/screen.vert");

    auto tonemap_pipeline = graphics_pipelines.push("Tonemap Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    tonemap_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/const_multiply.frag"));
    auto blur_pipeline = graphics_pipelines.push("Blur Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    blur_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/blur.frag"));
    auto fxaa_pipeline = graphics_pipelines.push("FXAA Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    fxaa_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/fxaa.frag"));
    auto flare_pipeline = graphics_pipelines.push("Flare Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    flare_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/flare.frag"));
    auto bright_spots_pipeline = graphics_pipelines.push("Bright Spots Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    bright_spots_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/bright_spots.frag"));
    auto add_pipeline = graphics_pipelines.push("Add Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    add_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/add.frag"));
    auto ssao_pipeline = graphics_pipelines.push("SSAO Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    ssao_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/ssao.frag"));

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
    source->set(AL_GAIN, 1.f);
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
        while (is_iconified)
            glfwPollEvents();

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

        cam.update(main_window->delta_time());
        listener->set_position(cam.transform.position);
        listener->set_orientation(cam.transform.rotation);

        auto&& scene = scene_buffer->at<geo::scene>(0);
        scene.set_view(cam.viewMatrix());
        scene.set_projection(cam.projectionMatrix());
        scene.set_time(glfwGetTime());
        glFinish();

        framebuffer->bind();
        framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubemap_pipeline->bind();
        cubemap_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("cubemap_matrix") =
            glm::mat4(glm::mat3(scene.get_inv_view())) * scene.get_inv_projection();
        cubemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::samplerCube>("map") = sampler->sample_texture(cubemap);
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
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("my_texture") = sampler->sample_texture(texture_col);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("texture_depth") = sampler->sample_texture(texture_dis);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("texture_normal") = sampler->sample_texture(texture_nor);
        graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::samplerCube>("cube_map") = sampler->sample_texture(cubemap);
        vertex_vao->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cylinder.meshes.get_by_index(0).indices.size()), GL_UNSIGNED_INT, nullptr);
        framebuffer->unbind();

        framebuffer->read_from_attachment(GL_COLOR_ATTACHMENT0);
        resolve_framebuffer->draw_to_attachments({ temporal_target });
        framebuffer->blit(resolve_framebuffer,
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y },
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y }, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        constexpr auto adaption_speed = 0.4f;
        constexpr auto brightness = 1.8f;

        const auto new_luma = std::accumulate(sample_buffer->data_as<float>(), sample_buffer->data_as<float>() + 32, 0.f) * (1 / 32.f);
        luma = glm::mix(glm::mix(luma, new_luma, adaption_speed * main_window->delta_time()), new_luma, adaption_speed * main_window->delta_time());
        const auto half_inv_luma = 0.5f / luma;
        const auto luminance = half_inv_luma * (half_inv_luma * brightness + 1.f) / (half_inv_luma + 1.f);
        luminance_compute_pipeline->bind();
        luminance_compute_pipeline->stage(gl::shader_type::compute)->get_uniform<gl::sampler2D>("src_texture") = sampler->sample_texture(full_size_attachments[0]);
        luminance_compute_pipeline->dispatch(1);

        generic_vao->bind();
        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ temporal_target });
        tonemap_pipeline->bind();
        tonemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(full_size_attachments[0]);
        tonemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor") = luminance;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();
        full_size_attachments[0]->generate_mipmaps();

        blur_framebuffer->bind();
        blur_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        glViewport(0, 0, quarter_resolution.x, quarter_resolution.y);
        bright_spots_pipeline->bind();
        bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(full_size_attachments[0]);
        bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("threshold_lower") = 1.0f;
        bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("threshold_higher") = 1.75f;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        blur_framebuffer->unbind();

        blur_framebuffer->bind();
        blur_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        blur_pipeline->bind();
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(quarter_size_attachments[0]);
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("axis") = 0;
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("level") = 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        blur_framebuffer->unbind();

        blur_framebuffer->bind();
        blur_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(quarter_size_attachments[1]);
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("axis") = 1;
        blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("level") = 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        blur_framebuffer->unbind();

        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        glViewport(0, 0, full_resolution.x, full_resolution.y);
        flare_pipeline->bind();
        flare_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(quarter_size_attachments[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();

        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ temporal_target });
        add_pipeline->bind();
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(full_size_attachments[0]);
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[1]") = sampler->sample_texture(full_size_attachments[1]);
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_one") = 1.f;
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_two") = 0.3f;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();

        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        ssao_pipeline->bind();
        ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("albedo_texture") = sampler->sample_texture(full_size_attachments[0]);
        ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("random_texture") = sampler->sample_texture(random_texture);
        ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("normal_depth_texture") = msaa_attachments[0]->address();
        ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("time") = glfwGetTime();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();

        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ temporal_target });
        fxaa_pipeline->bind();
        fxaa_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(full_size_attachments[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();

        resolve_framebuffer->bind();
        resolve_framebuffer->draw_to_attachments({ other_temporal_buffer });
        add_pipeline->bind();
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = sampler->sample_texture(full_size_attachments[temporal_target - GL_COLOR_ATTACHMENT0]);
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[1]") = sampler->sample_texture(full_size_attachments[temporal_buffer - GL_COLOR_ATTACHMENT0]);
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_one") = 0.7f;
        add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_two") = 0.3f;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        resolve_framebuffer->unbind();
        std::swap(temporal_buffer, other_temporal_buffer);

        resolve_framebuffer->read_from_attachment(temporal_buffer);
        resolve_framebuffer->draw_to_attachments({ other_temporal_buffer });
        resolve_framebuffer->blit(resolve_framebuffer, 
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y }, 
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y }, 
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        resolve_framebuffer->blit(nullptr, 
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y }, 
            gl::framebuffer::blit_rect{ 0, 0, full_resolution.x, full_resolution.y }, 
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

