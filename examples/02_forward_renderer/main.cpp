#include "light.hpp"
#include "mesh.hpp"
#include <execution>
#include <gfx/gfx.hpp>
#include <numeric>
#include <random>

std::unique_ptr<gl::framebuffer> main_framebuffer;

void build_framebuffer(int width, int height)
{
    main_framebuffer = std::make_unique<gl::framebuffer>();
    main_framebuffer->at(GL_COLOR_ATTACHMENT0) =
            std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_RGBA16F);
    main_framebuffer->at(GL_DEPTH_ATTACHMENT) =
            std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_DEPTH32F_STENCIL8, 1);
}
void dbg(GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char* message, const void* userParam)
{
    // log_i << message;
}

int main()
{
    gfx::context_options options;
    options.debug               = true;
    options.window_title        = "[GL] Forward Renderer";
    options.window_width        = 1280;
    options.window_height       = 720;
    options.framebuffer_samples = 8;
    auto context                = gfx::context::create(options);
    context->make_current();

    gfx::imgui gui;
    context->key_callback.add([](GLFWwindow*, int k, int sc, int ac, int md) {
        if(k == GLFW_KEY_F5 && ac == GLFW_PRESS)
            gl::pipeline::reload_all();
    });
    glfwSwapInterval(0);

    glDebugMessageCallback(&dbg, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);

    gfx::camera            camera;
    gfx::camera_controller controller;

    gfx::mesh_holder large_meshes;
    gfx::mesh_holder small_meshes;

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    size_t     vertices  = 0;
    size_t     indices   = 0;
    const auto load_mesh = [&](const auto& filename) {
        const gfx::scene_file file(filename);
        size_t                small_vertex_count = 0;
        size_t                large_vertex_count = 0;
        size_t                small_index_count  = 0;
        size_t                large_index_count  = 0;
        size_t                small_mesh_count   = 0;
        size_t                large_mesh_count   = 0;
        for(const auto& mesh : file.meshes)
        {
            if(mesh.indices.size() > 128)
            {
                large_vertex_count += mesh.vertices.size();
                large_index_count += mesh.indices.size();
                ++large_mesh_count;
            }
            else
            {
                small_vertex_count += mesh.vertices.size();
                small_index_count += mesh.indices.size();
                ++small_mesh_count;
            }

            vertices += mesh.vertices.size();
            indices += mesh.indices.size();
        }
        for(const auto& mesh : file.meshes)
        {
            gfx::mesh_holder& meshes = [&]() -> gfx::mesh_holder& { return mesh.indices.size() > 128 ? large_meshes : small_meshes; }();

            gfx::mesh_instance instance  = meshes.create_mesh(mesh.vertices, mesh.indices);
            gfx::transform     transform = mesh.transform;
            transform.scale              = glm::vec3(0.01f);
            instance.model_matrix        = transform;
            instance.color               = glm::vec3(dist(gen), dist(gen), dist(gen)) * glm::vec3(1.f);

            float roughness             = dist(gen);
            float metal                 = dist(gen);
            instance.packed_rough_metal = glm::packUnorm2x16(glm::vec2(roughness, metal));

            meshes.update_mesh(instance);
        }
    };
    load_mesh("sponza/sponza.obj");

    gfx::ilog << "Loaded " << vertices << " vertices with " << indices << " indices.";

    gl::buffer<gfx::camera::data> matrix_buffer(camera.info(), GL_DYNAMIC_STORAGE_BIT);

    gl::pipeline mesh_pipeline;
    mesh_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("02_forward_renderer/mesh.vert");
    mesh_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/mesh.frag");

    gl::pipeline mesh_shadow_pipeline;
    mesh_shadow_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("02_forward_renderer/mesh_shadow.vert");
    mesh_shadow_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/mesh_shadow.frag");

    std::vector<gfx::light> lights(4);
    lights[0].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(4, 24, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[0].color                = glm::vec4(1.f, 0.6f, 0.4f, 15.f);
    lights[1].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(4, 20, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[1].color                = glm::vec4(0.3f, 0.7f, 1.f, 15.f);
    lights[2].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(-7, 8, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[2].color                = glm::vec4(0.4f, 0.8f, 0.1f, 13.f);
    lights[3].color                = glm::vec4(0.9f, 0.8f, 0.7f, 10.f);

    gl::buffer<gfx::light::info> light_buffer(
            lights.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    light_buffer.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

    build_framebuffer(1280, 720);

    gl::texture          noise_texture(GL_TEXTURE_3D, 128, 128, 128, GL_R16F, 1);
    gl::compute_pipeline simplex_pipeline(std::make_shared<gl::shader>("02_forward_renderer/simplex.comp"));
    gl::sampler          noise_sampler;
    gl::buffer<float>    time({static_cast<float>(glfwGetTime())},
                              GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    time.map(GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    struct qmap : std::map<std::string, qmap>
    {
        struct timer_query : gl::query
        {
            timer_query()
                    : gl::query(GL_TIME_ELAPSED)
            {
            }
        };
        std::map<std::string, timer_query> queries;
    } queries;

    while(context->run())
    {
        gui.new_frame();

        gl::framebuffer::zero().clear(0, {0.003f, 0.203f, 0.004f, 1.f});
        gl::framebuffer::zero().clear(0.f, 0);

        light_buffer.reserve(lights.size());
        int enabled_lights = 0;
        for(int i = 0; i < lights.size(); ++i)
            if(lights[i].enabled)
                light_buffer[enabled_lights++] = lights[i].make_info();
        light_buffer.synchronize();
        glFinish();

        queries["scene"]["lights"].queries["shadowmap"].start();
        for(auto& l : lights)
        {
            if(l.begin_shadowmap())
            {
                large_meshes.cull();
                mesh_shadow_pipeline.bind();
                small_meshes.render();
                large_meshes.render();
            }
        }
        queries["scene"]["lights"].queries["shadowmap"].finish();

        queries["scene"]["lights"].queries["update"].start();
        lights[1].map_camera.transform_mode = inverse(
                glm::lookAt(glm::vec3(7 * glm::sin(static_cast<float>(glfwGetTime())), 20, 7 * glm::cos(static_cast<float>(glfwGetTime()))),
                            glm::vec3(0, 0, 0),
                            glm::vec3(0, 1, 0)));
        lights[3].map_camera.transform_mode.position =
                glm::lerp(lights[3].map_camera.transform_mode.position, camera.transform_mode.position, static_cast<float>(10.f * context->delta()));
        lights[3].map_camera.transform_mode.rotation =
                glm::slerp(lights[3].map_camera.transform_mode.rotation, camera.transform_mode.rotation, static_cast<float>(10.f * context->delta()));
        queries["scene"]["lights"].queries["update"].finish();

        queries["scene"]["camera"].queries["update"].start();
        glViewport(0, 0, 1280, 720);
        controller.update(camera);
        matrix_buffer[0].view       = inverse(camera.transform_mode.matrix());
        matrix_buffer[0].projection = camera.projection_mode;
        matrix_buffer[0].position   = camera.transform_mode.position;
        matrix_buffer.synchronize();
        queries["scene"]["camera"].queries["update"].finish();

        queries["scene"].queries["culling"].start();
        gl::framebuffer::zero().bind();
        matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        if(enabled_lights != 0)
            light_buffer.bind(GL_SHADER_STORAGE_BUFFER, 0, 0, enabled_lights);
        else
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mygl::buffer::zero);
        large_meshes.cull();
        queries["scene"].queries["culling"].finish();

        queries["scene"].queries["render"].start();
        mesh_pipeline.bind();
        small_meshes.render();
        large_meshes.render();
        queries["scene"].queries["render"].finish();

        queries["simplex"].queries["update"].start();
        time[0] = static_cast<float>(glfwGetTime());
        time.bind(GL_UNIFORM_BUFFER, 5);
        noise_sampler.bind(15);
        noise_texture.bind(15);
        noise_texture.bind_image(0, GL_WRITE_ONLY);
        simplex_pipeline.dispatch(128, 128, 128);
        queries["simplex"].queries["update"].finish();

        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_MenuBar);
        enum class tab
        {
            timings,
            lights,
            io
        };
        static tab current_tab = tab::timings;
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Timings", current_tab != tab::timings))
            {
                current_tab = tab::timings;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Lights", current_tab != tab::lights))
            {
                current_tab = tab::lights;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("IO", current_tab != tab::io))
            {
                current_tab = tab::io;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if(current_tab == tab::timings)
        {
            const auto qprinter = [](const qmap& map) -> void {
                auto gentimes = [](const qmap& m, auto& gt) -> std::vector<float> {
                    std::vector<float> v;
                    for(const auto& x : m)
                    {
                        const auto vrt = gt(x.second, gt);
                        v.insert(v.end(), vrt.begin(), vrt.end());
                    }
                    if(!m.queries.empty())
                    {
                        for(const auto& qp : m.queries)
                            v.emplace_back(static_cast<float>(qp.second.get<uint64_t>() / 1'000'000.0));
                    }
                    return v;
                };

                auto qprint_impl = [&gentimes](const std::string& lbl, const qmap& m, auto& impl) -> void {
                    if(!m.empty())
                    {
                        for(const auto& x : m)
                        {
                            std::vector<float> times = gentimes(x.second, gentimes);
                            const float        t     = std::reduce(std::execution::par, times.begin(), times.end(), 0.f);

                            const bool tn = ImGui::TreeNode(x.first.c_str());
                            ImGui::SameLine(0, 0);
                            ImGui::Text((": " + std::to_string(t)).c_str());
                            if(tn)
                            {
                                if(!x.second.queries.empty())
                                {
                                    for(const auto& qp : x.second.queries)
                                        ImGui::Value(qp.first.c_str(), static_cast<float>(qp.second.get<uint64_t>() / 1'000'000.0));
                                }
                                impl(x.first, x.second, impl);
                                ImGui::TreePop();
                            }
                        }
                    }
                };

                qprint_impl("Timings", map, qprint_impl);
            };

            qprinter(queries);
        }
        else if(current_tab == tab::lights)
        {
            for(int i = 0; i < lights.size(); ++i)
            {
                const std::string id = "light[" + std::to_string(i) + "]";
                ImGui::DragFloat4(id.c_str(), glm::value_ptr(lights[i].color), 0.001f, 0.f, 1000.f);
                ImGui::Checkbox((id + ".use_shadowmap").c_str(), &lights[i].use_shadowmap);
                ImGui::Checkbox((id + ".enabled").c_str(), &lights[i].enabled);

                if(i != 0)
                    ImGui::Spacing();
            }
        }
        else if(current_tab == tab::io)
        {
            if(ImGui::Button("Load into..."))
            {
                auto files = gfx::file::open_dialog_multi("Open Scene", "./");
                if(!files.empty())
                {
                    for(const auto& file : files)
                        load_mesh(file.path);
                }
            }
        }
        ImGui::End();
        gui.render();
    }
}