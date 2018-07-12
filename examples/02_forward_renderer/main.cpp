#define GFX_EXPOSE_APIS
#include "light.hpp"
#include "mesh.hpp"
#include <array>
#include <execution>
#include <gfx/gfx.hpp>
#include <map>
#include <numeric>
#include <opengl/opengl.hpp>
#include <random>
#include <vector>

std::unique_ptr<gl::framebuffer> main_framebuffer;

void build_framebuffer(int width, int height)
{
    main_framebuffer = std::make_unique<gl::framebuffer>();
    main_framebuffer->at(GL_COLOR_ATTACHMENT0) =
        std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_RGBA16F);
    main_framebuffer->at(GL_DEPTH_ATTACHMENT) =
        std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_DEPTH32F_STENCIL8, 1);
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
        if (k == GLFW_KEY_F5 && ac == GLFW_PRESS) gl::pipeline::reload_all();
    });
    glfwSwapInterval(0);

    gfx::camera            camera;
    gfx::camera_controller controller;

    gfx::mesh_holder large_meshes;
    gfx::mesh_holder small_meshes;

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    size_t                                        vertices = 0;
    size_t                                        indices  = 0;
    std::vector<std::optional<gfx::device_image>> diffuse_textures;
    std::vector<std::optional<gfx::device_image>> bump_textures;
    gfx::sampler                                  sampler;
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::repeat);
    const auto load_mesh = [&](const auto& filename) {
        const gfx::scene_file file(filename);
        size_t                small_vertex_count = 0;
        size_t                large_vertex_count = 0;
        size_t                small_index_count  = 0;
        size_t                large_index_count  = 0;
        size_t                small_mesh_count   = 0;
        size_t                large_mesh_count   = 0;
        for (const auto& mesh : file.meshes)
        {
            if (mesh.indices.size() > 128)
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
        for (const auto& mat : file.materials)
        {
            if (mat.texture_diffuse.bytes())
            {
                gfx::host_image hi(gfx::rgba8unorm, mat.texture_diffuse);
                hi.flip_vertically();
                diffuse_textures.emplace_back(hi);
            }
            else
                diffuse_textures.emplace_back();
            if (mat.texture_bump.bytes())
            {
                gfx::host_image hi(gfx::rgba8unorm, mat.texture_bump);
                hi.flip_vertically();
                bump_textures.emplace_back(hi);
            }
            else
                bump_textures.emplace_back();
        }
        for (const auto& mesh : file.meshes)
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
            if (diffuse_textures[mesh.material_index])
            {
                instance.diffuse_texture = glGetTextureSamplerHandleARB(*diffuse_textures[mesh.material_index], sampler);
                if (!glIsTextureHandleResidentARB(instance.diffuse_texture)) glMakeTextureHandleResidentARB(instance.diffuse_texture);
            }
            if (bump_textures[mesh.material_index])
            {
                instance.bump_texture = glGetTextureSamplerHandleARB(*bump_textures[mesh.material_index], sampler);
                if (!glIsTextureHandleResidentARB(instance.bump_texture)) glMakeTextureHandleResidentARB(instance.bump_texture);
            }
            meshes.update_mesh(instance);
        }
    };
    load_mesh("sponza/sponza.obj");

    gfx::ilog << "Loaded " << vertices << " vertices with " << indices << " indices.";

    gfx::host_buffer<gfx::camera::data> matrix_buffer{camera.info()};

    gl::pipeline mesh_pipeline;
    mesh_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("02_forward_renderer/mesh.vert");
    mesh_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/mesh_tmp.frag");

    gl::pipeline cubemap_pipeline;
    cubemap_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("02_forward_renderer/mesh_unprojected.vert");
    cubemap_pipeline[GL_GEOMETRY_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/to_cubemap.geom");
    cubemap_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/mesh.frag");

    gl::pipeline mesh_shadow_pipeline;
    mesh_shadow_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("02_forward_renderer/mesh_shadow.vert");
    mesh_shadow_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("02_forward_renderer/mesh_shadow.frag");

    std::vector<gfx::light> lights(4);
    lights[0].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(9, 24, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[0].color                     = glm::vec4(1, 1, 1, 22);
    lights[1].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(8, 20, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[1].color                     = glm::vec4(1, 1, 1, 15);
    lights[2].map_camera.transform_mode = inverse(glm::lookAt(glm::vec3(-7, 8, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[2].color                     = glm::vec4(1, 1, 1, 10);
    lights[3].color                     = glm::vec4(1, 1, 1, 1);

    gfx::host_buffer<gfx::light::info> light_buffer(lights.size());
    gfx::host_buffer<float>            time_buffer({static_cast<float>(glfwGetTime())});

    build_framebuffer(1280, 720);

    gl::texture          noise_texture(GL_TEXTURE_3D, 128, 128, 128, GL_R16F, 1);
    gl::compute_pipeline simplex_pipeline(std::make_shared<gl::shader>("02_forward_renderer/simplex.comp"));
    gl::sampler          noise_sampler;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    struct qmap : std::map<std::string, qmap>
    {
        struct timer_query : gl::query
        {
            timer_query() : gl::query(GL_TIME_ELAPSED) {}
        };
        std::map<std::string, timer_query> queries;
    } queries;

    struct aligned_mat4
    {
        glm::mat4 matrix;
    };

    constexpr uint32_t count = 8;

    gfx::host_buffer<aligned_mat4>       cubemap_infos(count * count * count);
    std::array<gfx::host_buffer<int>, 6> cubemap_layers{gfx::host_buffer<int>(1), gfx::host_buffer<int>(1), gfx::host_buffer<int>(1),
                                                        gfx::host_buffer<int>(1), gfx::host_buffer<int>(1), gfx::host_buffer<int>(1)};
    int                                  i = 0;
    for (auto&& l : cubemap_layers) l[0] = i++;

    constexpr uint32_t map_size = 8;

    struct tex
    {
        tex()
              : cubemap_target(gfx::img_type::image2d, gfx::rgba16f, gfx::extent{map_size, map_size, 6}, 2)
              , cubemap_depth(gfx::img_type::image2d, gfx::d32f, gfx::extent{map_size, map_size, 6}, 1)
              , cubemap_view(gfx::imgv_type::image_cube, cubemap_target.pixel_format(), cubemap_target, 1, 1, 0, 6)
        {
            glNamedFramebufferTexture(cubemap_framebuffer, GL_COLOR_ATTACHMENT0, cubemap_target, 0);
            glNamedFramebufferTexture(cubemap_framebuffer, GL_DEPTH_ATTACHMENT, cubemap_depth, 0);
        }

        gfx::device_image cubemap_target;
        gfx::device_image cubemap_depth;
        gfx::image_view   cubemap_view;
        gl::framebuffer   cubemap_framebuffer;
    };

    light_buffer.resize(lights.size());
    int enabled_lights = 0;
    for (int i = 0; i < lights.size(); ++i)
        if (lights[i].enabled) light_buffer[enabled_lights++] = lights[i].make_info();
    glFinish();
    for (auto& l : lights)
    {
        if (l.begin_shadowmap())
        {
            large_meshes.cull();
            mesh_shadow_pipeline.bind();
            small_meshes.render();
            large_meshes.render();
        }
    }
    if (enabled_lights != 0)
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, light_buffer, 0, enabled_lights * sizeof(gfx::light::info));
    else
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mygl::buffer::zero);

    gl::query qq(GL_TIME_ELAPSED);

    std::vector<tex> cmts(count * count * count);

    for (int x = 0; x < count; ++x)
        for (int y = 0; y < count; ++y)
            for (int z = 0; z < count; ++z)
            {
                cubemap_infos[count * count * z + count * y + x].matrix = glm::translate(-glm::vec3(3.f * x - count / 2, 3.f * y,
                                              3.f * z - count / 2));    // x - count / 2, y - count / 2, z - count / 2));
            }

    qq.start();
    matrix_buffer[0].do_cull = false;
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    small_meshes.cull();
    large_meshes.cull();
    cubemap_pipeline.bind();
    glViewport(0, 0, map_size, map_size);

    for (int pos = 0; pos < count * count * count; ++pos)
    {
        cmts[pos].cubemap_framebuffer.bind();
        cmts[pos].cubemap_framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        cmts[pos].cubemap_framebuffer.clear(0, {0.f, 0.f, 0.f, 1.f});
        cmts[pos].cubemap_framebuffer.clear(0.f, 0);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, cubemap_infos, pos * sizeof(aligned_mat4), sizeof(aligned_mat4));
        for (int i = 0; i < 6; ++i)
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, cubemap_layers[i]);
            small_meshes.render();
            large_meshes.render();
        }
        glFinish();
        glGenerateTextureMipmap(cmts[pos].cubemap_target);
    }
    glViewport(0, 0, 1280, 720);
    matrix_buffer[0].do_cull = true;
    gl::framebuffer::zero().bind();
    qq.finish();

    gfx::host_buffer<uint64_t> cubemaps(count * count * count);
    for (auto i = 0ull; i < cmts.size(); ++i)
    {
        cubemaps[i] = glGetTextureSamplerHandleARB(cmts[i].cubemap_view, sampler);
        glMakeTextureHandleResidentARB(cubemaps[i]);
    }
    // gfx::device_buffer<uint64_t> cubemaps_device(gfx::buffer_usage::storage, cubemaps);

    gfx::ilog << (count * count * count) << " Probes took " << qq.get<uint64_t>() << "ns";

    while (context->run())
    {
        gui.new_frame();
        gl::framebuffer::zero().bind();
        gl::framebuffer::zero().clear(0, {0.003f, 0.203f, 0.004f, 1.f});
        gl::framebuffer::zero().clear(0.f, 0);

        light_buffer.resize(lights.size());
        int enabled_lights = 0;
        for (int i = 0; i < lights.size(); ++i)
            if (lights[i].enabled) light_buffer[enabled_lights++] = lights[i].make_info();
        glFinish();

        queries["scene"]["lights"].queries["shadowmap"].start();
        for (auto& l : lights)
        {
            if (l.begin_shadowmap())
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
                        glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
        lights[3].map_camera.transform_mode.position = glm::lerp(
            lights[3].map_camera.transform_mode.position, camera.transform_mode.position, static_cast<float>(10.f * context->delta()));
        lights[3].map_camera.transform_mode.rotation = glm::slerp(
            lights[3].map_camera.transform_mode.rotation, camera.transform_mode.rotation, static_cast<float>(10.f * context->delta()));
        queries["scene"]["lights"].queries["update"].finish();

        queries["scene"]["camera"].queries["update"].start();
        glViewport(0, 0, 1280, 720);
        controller.update(camera);
        matrix_buffer[0].view       = inverse(camera.transform_mode.matrix());
        matrix_buffer[0].projection = camera.projection_mode;
        matrix_buffer[0].position   = camera.transform_mode.position;
        queries["scene"]["camera"].queries["update"].finish();

        queries["scene"].queries["culling"].start();
        gl::framebuffer::zero().bind();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrix_buffer);
        if (enabled_lights != 0)
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, light_buffer, 0, enabled_lights * sizeof(gfx::light::info));
        else
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mygl::buffer::zero);
        large_meshes.cull();
        queries["scene"].queries["culling"].finish();

        queries["scene"].queries["render"].start();
        mesh_pipeline.bind();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, cubemaps);
        small_meshes.render();
        large_meshes.render();
        queries["scene"].queries["render"].finish();

        queries["simplex"].queries["update"].start();
        time_buffer[0] = static_cast<float>(glfwGetTime());
        glBindBufferBase(GL_UNIFORM_BUFFER, 5, time_buffer);
        noise_sampler.bind(15);
        noise_texture.bind(15);
        noise_texture.bind_image(0, GL_WRITE_ONLY);
        // simplex_pipeline.dispatch(128, 128, 128);
        queries["simplex"].queries["update"].finish();

        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_MenuBar);
        enum class tab
        {
            timings,
            lights,
            io
        };
        static tab current_tab = tab::timings;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Timings", current_tab != tab::timings))
            {
                current_tab = tab::timings;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Lights", current_tab != tab::lights))
            {
                current_tab = tab::lights;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("IO", current_tab != tab::io))
            {
                current_tab = tab::io;
                ImGui::CloseCurrentPopup();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (current_tab == tab::timings)
        {
            const auto qprinter = [](const qmap& map) -> void {
                auto gentimes = [](const qmap& m, auto& gt) -> std::vector<float> {
                    std::vector<float> v;
                    for (const auto& x : m)
                    {
                        const auto vrt = gt(x.second, gt);
                        v.insert(v.end(), vrt.begin(), vrt.end());
                    }
                    if (!m.queries.empty())
                    {
                        for (const auto& qp : m.queries) v.emplace_back(static_cast<float>(qp.second.get<uint64_t>() / 1'000'000.0));
                    }
                    return v;
                };

                auto qprint_impl = [&gentimes](const std::string& lbl, const qmap& m, auto& impl) -> void {
                    if (!m.empty())
                    {
                        for (const auto& x : m)
                        {
                            std::vector<float> times = gentimes(x.second, gentimes);
                            const float        t     = std::reduce(std::execution::par, times.begin(), times.end(), 0.f);

                            const bool tn = ImGui::TreeNode(x.first.c_str());
                            ImGui::SameLine(0, 0);
                            ImGui::Text((": " + std::to_string(t)).c_str());
                            if (tn)
                            {
                                if (!x.second.queries.empty())
                                {
                                    for (const auto& qp : x.second.queries)
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
        else if (current_tab == tab::lights)
        {
            for (int i = 0; i < lights.size(); ++i)
            {
                const std::string id = "light[" + std::to_string(i) + "]";
                ImGui::DragFloat4(id.c_str(), glm::value_ptr(lights[i].color), 0.001f, 0.f, 1000.f);
                ImGui::Checkbox((id + ".use_shadowmap").c_str(), &lights[i].use_shadowmap);
                ImGui::Checkbox((id + ".enabled").c_str(), &lights[i].enabled);

                if (i != 0) ImGui::Spacing();
            }
        }
        else if (current_tab == tab::io)
        {
            if (ImGui::Button("Load into..."))
            {
                auto files = gfx::file::open_dialog_multi("Open Scene", "./");
                if (!files.empty())
                {
                    for (const auto& file : files) load_mesh(file.path);
                }
            }
        }
        ImGui::End();
        gui.render();
    }
}