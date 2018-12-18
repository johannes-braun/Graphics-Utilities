#define MYGL_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <mygl/mygl.hpp>
#include <thread>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "camera.hpp"
#include "gfx.core/log.hpp"
#include "globals.hpp"
#include "input_glfw.hpp"
#include "prototype.hpp"
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.file/file.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "shaders/shaders.hpp"

namespace impl::opengl {
ecs_state_t* _current_state;
void         run();
struct mesh_info
{};
}    // namespace impl::opengl
std::thread run_opengl(ecs_state_t& ecs_state)
{
    impl::opengl::_current_state = &ecs_state;
    return std::thread([] { impl::opengl::run(); });
}
namespace impl::opengl {
void run()
{
    GLFWwindow* opengl_window = [] {
        std::unique_lock lock(_current_state->glfw_mutex);
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        return glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr);
    }();

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

    gfx::gl::instance_system<mesh_info> instances({});
    graphics_list.add(instances);

    gfx::glfw_input_system  input(opengl_window);
    gfx::user_camera_system cam_system(input);
    gfx::ecs::system_list   inputs_list;
    inputs_list.add(input);
    inputs_list.add(cam_system);

    gfx::scene_file model("models/dragon.obj");
    model.mesh.collapse();
    gfx::gl::unique_prototype bunny = instances.get_instantiator().allocate_prototype_unique("Bunny", model.mesh);

    auto user_entity = _current_state->ecs.create_entity_shared(gfx::transform_component {glm::vec3{0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});

    glEnable(GL_MULTISAMPLE);
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);

    mygl::shader vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(gfx::spirv::vkgl::shaders::gl_vs_vert),
                   gfx::u32(std::size(gfx::spirv::vkgl::shaders::gl_vs_vert) * sizeof(uint32_t)));
    glSpecializeShader(vertex_shader, "main", 0, nullptr, nullptr);

    mygl::shader fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, std::data(gfx::spirv::vkgl::shaders::gl_fs_frag),
                   gfx::u32(std::size(gfx::spirv::vkgl::shaders::gl_fs_frag) * sizeof(uint32_t)));
    glSpecializeShader(fragment_shader, "main", 0, nullptr, nullptr);

    mygl::shader_program program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

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

    gfx::gl::buffer<gfx::camera_matrices> camera_buffer({gfx::camera_matrices {}});

    glClearDepthf(0.f);
    glfwMakeContextCurrent(nullptr);
    glfwSwapInterval(0);
    gfx::timed_while::duration opengl_combined_delta = 0s;
    int                   opengl_frames         = 0;
    gfx::worker                opengl_graphics_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        opengl_combined_delta += delta;
        ++opengl_frames;
        if (opengl_combined_delta > 1s)
        {
            gfx::ilog("OPENGL") << std::to_string(opengl_frames / opengl_combined_delta.count()) << "fps";
            opengl_frames         = 0;
            opengl_combined_delta = 0s;
        }
        _current_state->ecs.update(delta, graphics_list);
        if (!init.exchange(true)) glfwMakeContextCurrent(opengl_window);
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

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
        glBindVertexArray(vao);
        glBindVertexBuffer(0, instances.get_mesh_allocator().vertex_buffer().get_buffer(), 0, sizeof(gfx::vertex3d));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instances.get_mesh_allocator().index_buffer().get_buffer());

        void* indices = reinterpret_cast<void*>(uint64_t(bunny->meshes[0]->base->_base_index) * sizeof(uint64_t));
        glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, bunny->meshes[0]->base->_index_count, GL_UNSIGNED_INT, indices, 1,
                                                      bunny->meshes[0]->base->_base_vertex, 0);

        glfwSwapBuffers(opengl_window);
        return self.value_after(true, update_time_graphics);
    });

    gfx::timed_while::run([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        _current_state->ecs.update(delta, inputs_list);
        glfwPollEvents();
        return self.value_after(!glfwWindowShouldClose(opengl_window), update_time_inputs);
    });

    opengl_graphics_worker.stop_and_wait();
}
}    // namespace impl::opengl