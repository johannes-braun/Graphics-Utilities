#define MYGL_IMPLEMENTATION

#include "opengl.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "scene.hpp"
#include "shaders/def.hpp"
#include <gfx.core/log.hpp>
#include <gfx.core/worker.hpp>
#include <gfx.ecs.defaults2/movement.hpp>
#include <gfx.ecs.defaults2/prototype_proxies.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.file/file.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <random>
#include <thread>

struct state_t
{
    mygl::shader_program program;
};

void check_linkage(mygl::shader_program program)
{
    int isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        int maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

        // The program is useless now. So delete it.
        glDeleteProgram(program);

        std::cerr << infoLog.data();
        // Provide the infolog in whatever manner you deem best.
        // Exit with failure.
    }
}

void opengl_app::on_run()
{
    state_t s;
    state            = &s;
    const auto getst = [&]() -> state_t& { return *static_cast<state_t*>(state); };

    gfx::ecs::ecs        ecs;
    gfx::opengl::context context(panel.winId(), {{gfx::opengl::GL_CONTEXT_FLAGS_ARB, gfx::opengl::GL_CONTEXT_DEBUG_BIT_ARB}},
                                 {{gfx::opengl::GL_SAMPLE_BUFFERS_ARB, true}, {gfx::opengl::GL_SAMPLES_ARB, 8}});

    std::atomic_bool      init = false;
    gfx::ecs::system_list graphics_list;

    glDebugMessageCallback(
        [](gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char* message,
           const void* userParam) {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH: std::cout << message; break;
            case GL_DEBUG_SEVERITY_MEDIUM: gfx::wlog << message; break;
            case GL_DEBUG_SEVERITY_LOW: gfx::ilog << message; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: gfx::dlog << message; break;
            }
        },
        nullptr);

    gfx::opengl::instance_proxy<def::mesh_info> opengl_proxy;
    gfx::instance_system<def::mesh_info>        instances(opengl_proxy,
                                                   DEF_use_rt_shadows ? gfx::mesh_allocator_flag::use_bvh : gfx::mesh_allocator_flag {});
    graphics_list.add(instances);
    user_data = &instances;

    gfx::movement_system movement;
    graphics_list.add(movement);

    gfx::user_camera_system cam_system(*_input.get());
    gfx::ecs::system_list   inputs_list;
    inputs_list.add(*_input.get());
    inputs_list.add(cam_system);

    std::vector<mygl::texture> textures;
    std::vector<mygl::sampler> samplers;

    mygl::sampler sampler;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 16.f);

    const auto make_texture_id = [&](const gfx::image_file& t) {
        assert(t.channels == 1 || t.channels == 4);
        const int id = textures.size();
        switch (t.channels)
        {
        case 1:
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &textures.emplace_back());
            const int swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
            glTextureParameteriv(textures.back(), GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            glTextureStorage2D(textures.back(), std::int32_t(1 + floor(log2(std::max(t.width, t.height)))), GL_R8, t.width, t.height);
            glTextureSubImage2D(textures.back(), 0, 0, 0, t.width, t.height, GL_RED, GL_UNSIGNED_BYTE, t.bytes());
            glGenerateTextureMipmap(textures.back());
        }
        break;
        case 4:
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &textures.emplace_back());
            glTextureStorage2D(textures.back(), std::int32_t(1 + floor(log2(std::max(t.width, t.height)))), GL_RGBA8, t.width, t.height);
            glTextureSubImage2D(textures.back(), 0, 0, 0, t.width, t.height, GL_RGBA, GL_UNSIGNED_BYTE, t.bytes());
            glGenerateTextureMipmap(textures.back());
        }
        break;
        default: return -1;
        }
        return id;
    };

    std::vector<gfx::ecs::shared_entity> mesh_entities =
        scene::scene_manager_t::get_mesh_entities(ecs, instances, scene::current_scene(), make_texture_id);

    samplers.resize(textures.size());
    std::fill(samplers.begin(), samplers.end(), sampler);
    auto user_entity = ecs.create_entity_shared(gfx::transform_component {glm::vec3 {0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});

    glEnable(GL_MULTISAMPLE);
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);

    texture_count = textures.size();
    create_program();

    shaders_lib.load("shaders_gl");
    const auto* const vs_shadow_shader = gfx::import_shader(shaders_lib, "shaders/gl_vs_shadow.vert");
    std::array        spec_constant_ids {def::constant_id_texture_count};
    std::array        spec_constant_values {std::uint32_t(textures.size())};
    mygl::shader      shadow_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &shadow_vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(*vs_shadow_shader),
                   gfx::u32(std::size(*vs_shadow_shader) * sizeof(uint32_t)));
    shaders_lib.unload();

    glSpecializeShader(shadow_vertex_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    mygl::shader_program shadow_program = glCreateProgram();
    glAttachShader(shadow_program, shadow_vertex_shader);
    glLinkProgram(shadow_program);
    check_linkage(shadow_program);
    glProgramUniform1i(getst().program, glGetProgramResourceLocation(getst().program, GL_UNIFORM, "shadow_map"), def::shadow_map_binding);

    mygl::texture shadow_map;
    glCreateTextures(GL_TEXTURE_2D, 1, &shadow_map);
    glTextureStorage2D(shadow_map, 1, GL_DEPTH32F_STENCIL8, 1024, 1024);

    mygl::sampler shadow_sampler;
    glCreateSamplers(1, &shadow_sampler);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(shadow_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    mygl::framebuffer shadow_map_framebuffer;
    glCreateFramebuffers(1, &shadow_map_framebuffer);
    glNamedFramebufferTexture(shadow_map_framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, shadow_map, 0);

    const auto                                pos  = glm::vec3(3, 22, 3);
    const auto                                dir  = normalize(glm::vec3(0) - pos);
    const auto                                quat = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    gfx::transform_component                  light_transform(pos, {1, 1, 1}, quat);
    gfx::projection_component                 light_projection(glm::radians(60.f), 1, 1, 0.1f, 100.f);
    gfx::ecs::shared_entity                   light_entity = ecs.create_entity_shared(light_transform, light_projection);
    gfx::opengl::buffer<gfx::camera_matrices> light_camera_buffer({*gfx::get_camera_info(*light_entity)});

    mygl::vertex_array vao;
    glCreateVertexArrays(1, &vao);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, offsetof(gfx::vertex3d, position));
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal));
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribBinding(vao, 2, 0);

    gfx::opengl::buffer<gfx::camera_matrices> camera_buffer({gfx::camera_matrices {}});

    constexpr int max_frames = 2;
    mygl::sync    fences[max_frames] {};
    int           current_frame = 0;

    struct query_handler
    {
        query_handler(std::vector<std::uint64_t>& times, std::shared_mutex& timer_mtx)
              : _queries(_stamp_id_count)
              , _stamp_times(times)
              , _stamp_times_buf(_stamp_id_count)
              , _timer_mtx(timer_mtx)
              , _stamp_locks(_stamp_id_count, true)
        {
            glCreateQueries(GL_TIMESTAMP, _stamp_id_count, _queries.data());
        }
        ~query_handler() { glDeleteQueries(_stamp_id_count, _queries.data()); }
        void capture(stamp_id id)
        {
            if (_stamp_locks[id])
            {
                glQueryCounter(_queries[id], GL_TIMESTAMP);
                _stamp_locks[id] = false;
            }
        }

        void update_if_available()
        {
            for (int i = 0; i < _stamp_id_count; ++i)
            {
                if (int available = 0; (glGetQueryObjectiv(_queries[i], GL_QUERY_RESULT_AVAILABLE, &available), available))
                {
                    std::uint64_t stamp = 0;
                    glGetQueryObjectui64v(_queries[i], GL_QUERY_RESULT_NO_WAIT, &stamp);
                    _stamp_times_buf[i] = stamp;
                    ++_available_count;
                }
            }
            if (_available_count >= _stamp_id_count)
            {
                _available_count = 0;
                std::unique_lock lock(_timer_mtx);
                std::swap(_stamp_times, _stamp_times_buf);
                for (auto& l : _stamp_locks) l = true;
            }
        }

    private:
        std::vector<mygl::query>    _queries;
        std::basic_string<bool>     _stamp_locks;
        std::vector<std::uint64_t>  _stamp_times_buf;
        std::vector<std::uint64_t>& _stamp_times;
        int                         _available_count = 0;
        std::shared_mutex&          _timer_mtx;
    };

    query_handler timers(_stamp_times, _stamp_time_mutex);

    glClearDepthf(0.f);
    context.set_swap_interval(0);
    gfx::opengl::context::clear_current();
    gfx::worker opengl_graphics_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        update_frametime(std::chrono::duration_cast<std::chrono::nanoseconds>(delta));
        if (!init.exchange(true)) context.make_current();

        timers.update_if_available();
        timers.capture(stamp_id_begin);
        if (glIsSync(fences[current_frame]))
        {
            glClientWaitSync(fences[current_frame], GL_SYNC_FLUSH_COMMANDS_BIT, std::numeric_limits<std::uint64_t>::max());
            glDeleteSync(fences[current_frame]);
        }
        if (_input->key_down(gfx::key::kb_p))
        {
            glFinish();
            create_program();
        }

        ecs.update(delta, graphics_list);

        /* Render Shadow Map */ {
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
            glDepthFunc(GL_GEQUAL);
            glDepthRangef(0.f, 1.f);
            glClearNamedFramebufferfi(shadow_map_framebuffer, GL_DEPTH_STENCIL, 0, 0.f, 0);
            glViewportIndexedf(0, 0, 0, 1024, 1024);
            glScissorIndexed(0, 0, 0, 1024, 1024);
            glUseProgram(shadow_program);
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, light_camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
            glBindVertexArray(vao);
            glBindVertexBuffer(0, opengl_proxy.vertex_buffer().get_buffer(), 0, sizeof(gfx::vertex3d));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_proxy.index_buffer().get_buffer());
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, opengl_proxy.instances_mapped().get_buffer(), 0,
                              opengl_proxy.instances_mapped().size() * sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, opengl_proxy.instances_mapped().get_buffer());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, opengl_proxy.instances_mapped().size(),
                                        sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));
            glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero());
        }

        timers.capture(stamp_id_shadowmap);

        glm::vec4 clear_color(0.3f, 0.5f, 0.9f, 1.f);
        glClearNamedFramebufferfv(mygl::framebuffer::zero(), GL_COLOR, 0, glm::value_ptr(clear_color));
        glClearNamedFramebufferfi(mygl::framebuffer::zero(), GL_DEPTH_STENCIL, 0, 1.f, 0);

        int width = panel.size().width(), height = panel.size().height();

        user_entity->get<gfx::projection_component>()->perspective().negative_y    = true;
        user_entity->get<gfx::projection_component>()->perspective().inverse_z     = false;
        user_entity->get<gfx::projection_component>()->perspective().screen_width  = width;
        user_entity->get<gfx::projection_component>()->perspective().screen_height = height;
        const gfx::camera_matrices cam                                             = *gfx::get_camera_info(*user_entity);
        glNamedBufferSubData(camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices), &cam);

        glUseProgram(getst().program);

        glViewportIndexedf(0, 0, 0, width, height);
        glDepthFunc(GL_LEQUAL);
        glDepthRangef(0.f, 1.f);
        glScissorIndexed(0, 0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);    // What the actual f*?

        // bind all textures
        glBindTextures(def::texture_binding_all, std::int32_t(textures.size()), textures.data());
        glBindSamplers(def::texture_binding_all, std::int32_t(samplers.size()), samplers.data());

        glBindTextureUnit(def::shadow_map_binding, shadow_map);
        glBindSampler(def::shadow_map_binding, shadow_sampler);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::shadow_cam_binding, light_camera_buffer.get_buffer(), 0,
                          light_camera_buffer.size() * sizeof(gfx::camera_matrices));

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));

        if constexpr (DEF_use_rt_shadows)
        {
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_vertex, opengl_proxy.vertex_buffer().get_buffer(), 0,
                              opengl_proxy.vertex_buffer().size() * sizeof(gfx::vertex3d));
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_element, opengl_proxy.index_buffer().get_buffer(), 0,
                              opengl_proxy.index_buffer().size() * sizeof(gfx::index32));
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_bvh, opengl_proxy.bvh_buffer().get_buffer(), 0,
                              opengl_proxy.bvh_buffer().size() * sizeof(gfx::bvh<3>::node));
        }

        glBindVertexArray(vao);
        glBindVertexBuffer(0, opengl_proxy.vertex_buffer().get_buffer(), 0, sizeof(gfx::vertex3d));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_proxy.index_buffer().get_buffer());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, opengl_proxy.instances_mapped().get_buffer(), 0,
                          opengl_proxy.instances_mapped().size() * sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, opengl_proxy.instances_mapped().get_buffer());
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, opengl_proxy.instances_mapped().size(),
                                    sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));

        timers.capture(stamp_id_render);

        fences[current_frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, {});
        current_frame         = (current_frame + 1) % max_frames;
        opengl_proxy.swap();
        context.swap_buffers();
        return self.value_after(true, update_time_graphics);
    });

    QTimer* fixed_updater;
    run_in_gui([&] {
        fixed_updater = new QTimer(&panel);
        QObject::connect(fixed_updater, &QTimer::timeout, [&] { ecs.update(update_time_inputs, inputs_list); });
        fixed_updater->start(update_time_inputs.count());
    });

    gfx::timed_while::run(
        [&](gfx::timed_while& self, gfx::timed_while::duration delta) { return self.value_after(panel.isVisible(), update_time_inputs); });

    // run_in_gui([&] { fixed_updater->stop(); });

    opengl_graphics_worker.stop_and_wait();
    fixed_updater->stop();
}

void opengl_app::create_program()
{
    const auto getst = [&]() -> state_t& { return *static_cast<state_t*>(state); };
    if (glIsProgram(getst().program)) { glDeleteProgram(getst().program); }

    std::array spec_constant_ids {def::constant_id_texture_count};
    std::array spec_constant_values {std::uint32_t(texture_count)};

    shaders_lib.load("shaders_gl");
    auto vs_shader = gfx::import_shader(shaders_lib, "shaders/gl_vs.vert");
    auto fs_shader = gfx::import_shader(shaders_lib, "shaders/gl_fs.frag");

    mygl::shader vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(*vs_shader),
                   gfx::u32(std::size(*vs_shader) * sizeof(uint32_t)));
    glSpecializeShader(vertex_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    mygl::shader fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(*fs_shader),
                   gfx::u32(std::size(*fs_shader) * sizeof(uint32_t)));
    glSpecializeShader(fragment_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    getst().program = glCreateProgram();
    glAttachShader(getst().program, vertex_shader);
    glAttachShader(getst().program, fragment_shader);
    glLinkProgram(getst().program);
    check_linkage(getst().program);

    for (int i = 0; i < texture_count; ++i)
    {
        auto loc = glGetProgramResourceLocation(getst().program, GL_UNIFORM, ("all_textures[" + std::to_string(i) + "]").c_str());
        glProgramUniform1i(getst().program, loc, i);
    }
    shaders_lib.unload();
}
