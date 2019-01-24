#define MYGL_IMPLEMENTATION

#include "opengl.hpp"
#include "def.hpp"
#include "globals.hpp"
#include "input/camera.hpp"
#include "opengl_blur.hpp"
#include "opengl_cutoff.hpp"
#include "opengl_overlay.hpp"
#include "scene/scene.hpp"
#include <gfx.core/log.hpp>
#include <gfx.core/worker.hpp>
#include <gfx.ecs.components/basic.hpp>
#include <gfx.ecs.components/movement.hpp>
#include <gfx.ecs.components/prototype_proxies.hpp>
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

class bouncy_system : public gfx::ecs::system
{
    mutable std::mt19937                  gen;
    std::uniform_real_distribution<float> dist{-1, 1};

    bool               _explode      = false;
    bool               _explode_lock = false;
    gfx::input_system* _inp;

public:
    bouncy_system(gfx::input_system& i) : _inp(&i)
    {
        add_component_type<gfx::movement_component>();
        add_component_type<gfx::transform_component>();
        add_component_type<gfx::instance_component<def::mesh_info>>();
    }

    void pre_update() override
    {
        if (_inp->key_down(gfx::key::kb_z))
        {
            if (!_explode_lock)
            {
                _explode      = true;
                _explode_lock = true;
            }
        }
        else
        {
            _explode_lock = false;
        }
    }

    void update(duration_type delta, gfx::ecs::component_base** components) const override
    {
        auto& mc = components[0]->as<gfx::movement_component>();
        auto& tc = components[1]->as<gfx::transform_component>();
        auto& ic = components[2]->as<gfx::instance_component<def::mesh_info>>();

        const auto min = glm::vec3(tc.matrix() * glm::vec4(ic.handle->meshes[0].value().base->_bounds.min, 1));

        if (mc.gravity != 0 && min.y < 0.f)
        {
            tc.position.y -= min.y;
            mc.impulse.y = -mc.impulse.y;
        }

        if (mc.gravity != 0 && min.x < -50.f)
        {
            tc.position.x += -50.f - min.x;
            mc.impulse.x = -mc.impulse.x;
        }
        else if (mc.gravity != 0 && min.x > 50.f)
        {
            tc.position.x += 50.f - min.x;
            mc.impulse.x = -mc.impulse.x;
        }
        if (mc.gravity != 0 && min.z < -50.f)
        {
            tc.position.z += -50.f - min.z;
            mc.impulse.z = -mc.impulse.z;
        }
        else if (mc.gravity != 0 && min.z > 50.f)
        {
            tc.position.z += 50.f - min.z;
            mc.impulse.z = -mc.impulse.z;
        }

        if (_explode)
        {
            mc.impulse = {dist(gen) * 200, dist(gen) * 200 + 100, dist(gen) * 200};
            mc.gravity = 9.81f;
        }
    }

    void post_update() override { _explode = false; }

private:
};

void opengl_app::on_run()
{
    state_t s;
    state            = &s;
    const auto getst = [&]() -> state_t& { return *static_cast<state_t*>(state); };

    gfx::ecs::ecs        ecs;
    gfx::opengl::context context(panel.winId(), {{gfx::opengl::GL_CONTEXT_FLAGS_ARB, 0 /*gfx::opengl::GL_CONTEXT_DEBUG_BIT_ARB*/}},
                                 {{gfx::opengl::GL_SAMPLE_BUFFERS_ARB, true}, {gfx::opengl::GL_SAMPLES_ARB, 8}});

    gfx::opengl::context physics_context(nullptr, {{gfx::opengl::GL_CONTEXT_FLAGS_ARB, 0 /*gfx::opengl::GL_CONTEXT_DEBUG_BIT_ARB*/}}, {},
                                         &context);

    context.make_current();
    gfx::ecs::system_list physics_list;
    gfx::movement_system  movements;
    bouncy_system         bounce(*_input.get());
    physics_list.add(movements);
    physics_list.add(bounce);

    std::atomic_bool      init = false;
    gfx::ecs::system_list graphics_list;

    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
                                                   DEF_use_rt_shadows ? gfx::mesh_allocator_flag::use_bvh : gfx::mesh_allocator_flag{});
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
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 16.f);

    int width = panel.size().width(), height = panel.size().height();

    mygl::framebuffer main_fb;
    glCreateFramebuffers(1, &main_fb);

    struct sample_count
    {
        enum
        {
            x1  = 1,
            x2  = 2,
            x4  = 4,
            x8  = 8,
            x16 = 16
        };
    };

    mygl::texture att_msaa;
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &att_msaa);
    glTextureStorage2DMultisample(att_msaa, sample_count::x8, GL_RGBA16F, width, height, true);
    mygl::texture att_depth;
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &att_depth);
    glTextureStorage2DMultisample(att_depth, sample_count::x8, GL_DEPTH32F_STENCIL8, width, height, true);

    glNamedFramebufferTexture(main_fb, GL_COLOR_ATTACHMENT0, att_msaa, 0);
    glNamedFramebufferTexture(main_fb, GL_DEPTH_STENCIL_ATTACHMENT, att_depth, 0);
    glNamedFramebufferDrawBuffer(main_fb, GL_COLOR_ATTACHMENT0);

    mygl::framebuffer resolve_fb;
    glCreateFramebuffers(1, &resolve_fb);
    mygl::texture att_resolve;
    glCreateTextures(GL_TEXTURE_2D, 1, &att_resolve);
    glTextureStorage2D(att_resolve, 1, GL_RGBA16F, width, height);
    glNamedFramebufferTexture(resolve_fb, GL_COLOR_ATTACHMENT0, att_resolve, 0);
    glNamedFramebufferDrawBuffer(resolve_fb, GL_COLOR_ATTACHMENT0);

    const auto make_texture_id = [&](const gfx::image_file& t) {
        assert(t.channels == 1 || t.channels == 4);
        const int id = static_cast<int>(textures.size());
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


    const gfx::scene_file particle_scene("broken_torus.dae");
    struct particle_info
    {
        glm::vec3 position{0, 0, 0};
        float     life{0};
        glm::vec3 velocity{0, 0, 0};
        float     _p{0};
    };
    gfx::opengl::mapped<particle_info> particles(20'000);
    gfx::shared_prototype              particle_proto = instances.get_instantiator().allocate_prototype("Particle", particle_scene.mesh);

    shaders_lib.load("vk_and_gl.particle_shaders_gl");
    mygl::shader pcomp = glCreateShader(GL_COMPUTE_SHADER);
    mygl::shader pvert = glCreateShader(GL_VERTEX_SHADER);
    mygl::shader pfrag = glCreateShader(GL_FRAGMENT_SHADER);
    const auto   psrc  = gfx::import_shader(shaders_lib, "shaders/particle.comp");
    const auto   pvsrc = gfx::import_shader(shaders_lib, "shaders/particles_simple.vert");
    const auto   pfsrc = gfx::import_shader(shaders_lib, "shaders/particles_simple.frag");
    glShaderBinary(1, &pcomp, GL_SHADER_BINARY_FORMAT_SPIR_V, psrc->data(), psrc->size() * sizeof(glm::uint32_t));
    glSpecializeShader(pcomp, "main", 0, nullptr, nullptr);
    glShaderBinary(1, &pvert, GL_SHADER_BINARY_FORMAT_SPIR_V, pvsrc->data(), pvsrc->size() * sizeof(glm::uint32_t));
    glSpecializeShader(pvert, "main", 0, nullptr, nullptr);
    glShaderBinary(1, &pfrag, GL_SHADER_BINARY_FORMAT_SPIR_V, pfsrc->data(), pfsrc->size() * sizeof(glm::uint32_t));
    glSpecializeShader(pfrag, "main", 0, nullptr, nullptr);

    mygl::shader_program particle_program = glCreateProgram();
    glAttachShader(particle_program, pcomp);
    glLinkProgram(particle_program);
    check_linkage(particle_program);

    mygl::shader_program particle_render_program = glCreateProgram();
    glAttachShader(particle_render_program, pvert);
    glAttachShader(particle_render_program, pfrag);
    glLinkProgram(particle_render_program);
    check_linkage(particle_render_program);
    shaders_lib.unload();

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;


    for (auto& entity : mesh_entities)
    {
        gfx::movement_component mc;
        mc.movement_drag = 0;
        mc.rotation_drag = 0;
        entity->add(mc);
    }

    samplers.resize(textures.size());
    std::fill(samplers.begin(), samplers.end(), sampler);
    auto user_entity = ecs.create_entity_shared(gfx::transform_component{glm::vec3{0, 0, 4}, glm::vec3(3)}, gfx::projection_component{},
                                                gfx::grabbed_cursor_component{}, gfx::camera_controls{});

    glEnable(GL_MULTISAMPLE);
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);

    texture_count = textures.size();
    create_program();

    shaders_lib.load("vk_and_gl.shaders_gl");
    const auto* const vs_shadow_shader = gfx::import_shader(shaders_lib, "shaders/gl_vs_shadow.vert");
    std::array        spec_constant_ids{def::constant_id_texture_count};
    std::array        spec_constant_values{std::uint32_t(textures.size())};
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

    gfx::opengl::buffer<gfx::camera_matrices> camera_buffer({gfx::camera_matrices{}});

    constexpr int max_frames = 2;
    mygl::sync    fences[max_frames]{};
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

    glm::ivec2     blur_size(width, height);
    glblur_pass    hblur(blur_size, 0, 15);
    glblur_pass    vblur(blur_size, 1, 15);
    glcutoff_pass  cutoff(blur_size);
    gloverlay_pass overlay({width, height});

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
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);    // What the actual f*?
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
        glBindFramebuffer(GL_FRAMEBUFFER, main_fb);
        glClearNamedFramebufferfv(main_fb, GL_COLOR, 0, glm::value_ptr(clear_color));
        glClearNamedFramebufferfi(main_fb, GL_DEPTH_STENCIL, 0, 1.f, 0);

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

        glUseProgram(particle_render_program);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, particles.get_buffer(), 0, particles.size() * sizeof(particle_info));
        glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES, particle_proto->meshes[0]->base->_index_count, GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(particle_proto->meshes[0]->base->_base_index * sizeof(gfx::index32)), particles.size(),
            particle_proto->meshes[0]->base->_base_vertex);
        glUseProgram(particle_program);
        glProgramUniform1f(particle_program, 0, delta.count());
        glProgramUniform1f(particle_program, 1, dist(gen));
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, particles.get_buffer(), 0, particles.size() * sizeof(particle_info));
        glDispatchCompute(((particles.size() + 255) / 256), 1, 1);

        glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero());
        glBlitNamedFramebuffer(main_fb, resolve_fb, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        cutoff.render(att_resolve);
        hblur.render(cutoff.get_image());
        vblur.render(hblur.get_image());
        overlay.render(att_resolve, vblur.get_image());
        for (int i = 0; i < arbitrary_bloom_count_which_i_don_t_know_the_purpose_of; ++i)
        {
            cutoff.render(overlay.get_image());
            hblur.render(cutoff.get_image());
            vblur.render(hblur.get_image());
            overlay.render(att_resolve, vblur.get_image());
        }

        glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero());
        glBlitNamedFramebuffer(overlay.get_framebuffer(), mygl::framebuffer::zero(), 0, 0, width, height, 0, 0, width, height,
                               GL_COLOR_BUFFER_BIT, GL_NEAREST);

        timers.capture(stamp_id_render);

        fences[current_frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, {});
        current_frame         = (current_frame + 1) % max_frames;
        opengl_proxy.swap();
        context.swap_buffers();
        return self.value_after(true, update_time_graphics);
    });

    bool        phy_init = false;
    gfx::worker physics_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        if (!phy_init)
        {
            physics_context.make_current();
            phy_init = true;
        }
        ecs.update(delta, physics_list);

        return self.value_after(true, update_time_physics);
    });

    QTimer* fixed_updater;
    run_in_gui([&] {
        fixed_updater = new QTimer(&panel);
        QObject::connect(fixed_updater, &QTimer::timeout, [&] { ecs.update(update_time_inputs, inputs_list); });
        fixed_updater->start(update_time_inputs.count());
    });

    gfx::timed_while::run(
        [&](gfx::timed_while& self, gfx::timed_while::duration delta) { return self.value_after(panel.isVisible(), update_time_inputs); });

    opengl_graphics_worker.stop_and_wait();
    physics_worker.stop_and_wait();

    fixed_updater->stop();
}

void opengl_app::create_program()
{
    const auto getst = [&]() -> state_t& { return *static_cast<state_t*>(state); };
    if (glIsProgram(getst().program)) { glDeleteProgram(getst().program); }

    std::array spec_constant_ids{def::constant_id_texture_count};
    std::array spec_constant_values{std::uint32_t(texture_count)};

    shaders_lib.load("vk_and_gl.shaders_gl");
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
