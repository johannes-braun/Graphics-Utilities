#define MYGL_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <mygl/mygl.hpp>
#include <thread>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "camera.hpp"
#include "gfx.core/log.hpp"
#include "gfx.ecs.defaults2/movement.hpp"
#include "globals.hpp"
#include "input_glfw.hpp"
#include "opengl.hpp"
//#include "prototype.hpp"
#include "scene.hpp"
#include "shaders/def.hpp"
#include "shaders/shaders.hpp"
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs.defaults2/prototype_proxies.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.file/file.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <random>

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
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // The program is useless now. So delete it.
        glDeleteProgram(program);

        std::cerr << infoLog.data();
        // Provide the infolog in whatever manner you deem best.
        // Exit with failure.
    }
}

void opengl_app::on_run()
{
    gfx::ecs::ecs ecs;
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    GLFWwindow* opengl_window = glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr);

    glfwMakeContextCurrent(opengl_window);
    mygl::load(reinterpret_cast<mygl::loader_function>(&glfwGetProcAddress));
    std::atomic_bool      init = false;
    gfx::ecs::system_list graphics_list;

    glDebugMessageCallback(
        [](gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char* message,
           const void* userParam) {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH: gfx::elog << message; break;
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

    gfx::movement_system movement;
    graphics_list.add(movement);

    gfx::glfw_input_system  input(opengl_window);
    gfx::user_camera_system cam_system(input);
    gfx::ecs::system_list   inputs_list;
    inputs_list.add(input);
    inputs_list.add(cam_system);

    const auto& scene = scene::current_scene();

    std::vector<gfx::ecs::unique_entity> mesh_entities;

    std::vector<def::mesh_info> mesh_infos;
    std::vector<mygl::texture>  textures;
    std::vector<mygl::sampler>  samplers;

    mygl::sampler sampler;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 16.f);

    for (size_t i = 0; i < scene.materials.size(); ++i)
    {
        def::mesh_info& info = mesh_infos.emplace_back();
        info.diffuse.color   = 255 * clamp(scene.materials[i].color_diffuse, 0.f, 1.f);

        info.diffuse.texture_id = -1;
        if (scene.materials[i].texture_diffuse.bytes())
        {
            info.diffuse.texture_id = textures.size();

            const auto& t = scene.materials[i].texture_diffuse;
            glCreateTextures(GL_TEXTURE_2D, 1, &textures.emplace_back());
            glTextureStorage2D(textures.back(), std::int32_t(1 + floor(log2(std::max(t.width, t.height)))), GL_RGBA8, t.width, t.height);
            glTextureSubImage2D(textures.back(), 0, 0, 0, t.width, t.height, GL_RGBA, GL_UNSIGNED_BYTE, t.bytes());
            glGenerateTextureMipmap(textures.back());
        }
        if (scene.materials[i].texture_bump.bytes())
        {
            info.bump_map_texture_id = textures.size();

            const auto& t = scene.materials[i].texture_bump;
            glCreateTextures(GL_TEXTURE_2D, 1, &textures.emplace_back());
            const int swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
            glTextureParameteriv(textures.back(), GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            glTextureStorage2D(textures.back(), std::int32_t(1 + floor(log2(std::max(t.width, t.height)))), GL_R8, t.width, t.height);
            glTextureSubImage2D(textures.back(), 0, 0, 0, t.width, t.height, GL_RED, GL_UNSIGNED_BYTE, t.bytes());
            glGenerateTextureMipmap(textures.back());
        }
    }
    samplers.resize(textures.size());
    std::fill(samplers.begin(), samplers.end(), sampler);

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;
    for (size_t i = 0; i < scene.mesh.geometries.size(); ++i)
    {
        gfx::shared_mesh      mesh  = instances.get_mesh_allocator().allocate_mesh(scene.mesh, scene.mesh.geometries[i]);
        gfx::shared_prototype proto = instances.get_instantiator().allocate_prototype("sponza_" + std::to_string(i), {&mesh, 1});

        gfx::instance_component<def::mesh_info> instance_component(std::move(proto), mesh_infos[scene.mesh_material_indices.at(i)]);
        gfx::transform_component                transform_component = scene.mesh.geometries[i].transformation.matrix();
        mesh_entities.emplace_back(ecs.create_entity_unique(instance_component, transform_component));
    }

    auto user_entity = ecs.create_entity_shared(gfx::transform_component {glm::vec3 {0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});

    glEnable(GL_MULTISAMPLE);
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);

    std::array spec_constant_ids {def::constant_id_texture_count};
    std::array spec_constant_values {std::uint32_t(textures.size())};

    mygl::shader vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(gfx::spirv::vkgl::shaders::gl_vs_vert),
                   gfx::u32(std::size(gfx::spirv::vkgl::shaders::gl_vs_vert) * sizeof(uint32_t)));
    glSpecializeShader(vertex_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    mygl::shader fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(gfx::spirv::vkgl::shaders::gl_fs_frag),
                   gfx::u32(std::size(gfx::spirv::vkgl::shaders::gl_fs_frag) * sizeof(uint32_t)));
    glSpecializeShader(fragment_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    mygl::shader_program program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    check_linkage(program);

    for (int i = 0; i < textures.size(); ++i)
        glProgramUniform1i(program, glGetProgramResourceLocation(program, GL_UNIFORM, ("all_textures[" + std::to_string(i) + "]").c_str()),
                           i);









    mygl::shader shadow_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &shadow_vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(gfx::spirv::vkgl::shaders::gl_vs_shadow_vert),
                   gfx::u32(std::size(gfx::spirv::vkgl::shaders::gl_vs_shadow_vert) * sizeof(uint32_t)));
    glSpecializeShader(shadow_vertex_shader, "main", std::uint32_t(spec_constant_ids.size()), spec_constant_ids.data(),
                       spec_constant_values.data());

    mygl::shader_program shadow_program = glCreateProgram();
    glAttachShader(shadow_program, shadow_vertex_shader);
    glLinkProgram(shadow_program);
    check_linkage(shadow_program);
    glProgramUniform1i(program, glGetProgramResourceLocation(program, GL_UNIFORM, "shadow_map"), def::shadow_map_binding);

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

    const auto pos  = glm::vec3(3, 22, 3);
    const auto dir  = normalize(glm::vec3(0) - pos);
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

    glClearDepthf(0.f);
    glfwMakeContextCurrent(nullptr);
    glfwSwapInterval(0);
    gfx::timed_while::duration opengl_combined_delta = 0s;
    int                        opengl_frames         = 0;
    gfx::worker                opengl_graphics_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        opengl_combined_delta += delta;
        ++opengl_frames;
        if (opengl_combined_delta > 1s)
        {
            gfx::ilog("OPENGL") << std::to_string(opengl_frames / opengl_combined_delta.count()) << "fps";
            opengl_frames         = 0;
            opengl_combined_delta = 0s;
        }
        if (!init.exchange(true)) glfwMakeContextCurrent(opengl_window);

        ecs.update(delta, graphics_list);
        opengl_proxy.swap();

        glm::vec4 clear_color(0.3f, 0.5f, 0.9f, 1.f);
        glClearNamedFramebufferfv(mygl::framebuffer::zero(), GL_COLOR, 0, glm::value_ptr(clear_color));
        glClearNamedFramebufferfi(mygl::framebuffer::zero(), GL_DEPTH_STENCIL, 0, 0.f, 0);

        int width, height;
        glfwGetFramebufferSize(opengl_window, &width, &height);

        user_entity->get<gfx::projection_component>()->perspective().negative_y    = true;
        user_entity->get<gfx::projection_component>()->perspective().inverse_z     = false;
        user_entity->get<gfx::projection_component>()->perspective().screen_width  = width;
        user_entity->get<gfx::projection_component>()->perspective().screen_height = height;
        const gfx::camera_matrices cam                                             = *gfx::get_camera_info(*user_entity);
        glNamedBufferSubData(camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices), &cam);

        glUseProgram(program);

        glViewportIndexedf(0, 0, 0, width, height);
        glDepthFunc(GL_GEQUAL);
        glDepthRangef(1.f, 0.f);
        glScissorIndexed(0, 0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        // bind all textures
        glBindTextures(def::texture_binding_all, std::int32_t(textures.size()), textures.data());
        glBindSamplers(def::texture_binding_all, std::int32_t(samplers.size()), samplers.data());

        glBindTextureUnit(def::shadow_map_binding, shadow_map);
        glBindSampler(def::shadow_map_binding, shadow_sampler);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::shadow_cam_binding, light_camera_buffer.get_buffer(), 0,
                          light_camera_buffer.size() * sizeof(gfx::camera_matrices));

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
        glBindVertexArray(vao);
        glBindVertexBuffer(0, opengl_proxy.vertex_buffer().get_buffer(), 0, sizeof(gfx::vertex3d));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_proxy.index_buffer().get_buffer());

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, opengl_proxy.instances_buffer().get_buffer(), 0,
                          opengl_proxy.instances_buffer().size() * sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_vertex, opengl_proxy.vertex_buffer().get_buffer(), 0,
                          opengl_proxy.vertex_buffer().size() * sizeof(gfx::vertex3d));
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_element, opengl_proxy.index_buffer().get_buffer(), 0,
                          opengl_proxy.index_buffer().size() * sizeof(gfx::index32));
        if constexpr (DEF_use_rt_shadows)
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, def::buffer_binding_bvh, opengl_proxy.bvh_buffer().get_buffer(), 0,
                              opengl_proxy.bvh_buffer().size() * sizeof(gfx::bvh<3>::node));

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, opengl_proxy.instances_buffer().get_buffer());
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, opengl_proxy.instances_buffer().size(),
                                    sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));

        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
        glDepthRangef(0.f, 1.f);
        glClearNamedFramebufferfi(shadow_map_framebuffer, GL_DEPTH_STENCIL, 0, 0.f, 0);
        glViewportIndexedf(0, 0, 0, 1024, 1024);
        glScissorIndexed(0, 0, 0, 1024, 1024);
        glUseProgram(shadow_program);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, light_camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, opengl_proxy.instances_buffer().size(),
                                    sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));
        glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero());

        glfwSwapBuffers(opengl_window);
        return self.value_after(true, update_time_graphics);
    });

    gfx::timed_while::run([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        ecs.update(delta, inputs_list);
        glfwPollEvents();
        return self.value_after(!glfwWindowShouldClose(opengl_window), update_time_inputs);
    });

    opengl_graphics_worker.stop_and_wait();
}
