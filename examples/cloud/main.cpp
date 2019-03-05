#include <GLFW/glfw3.h>
#define MYGL_IMPLEMENTATION
#include "gfx.core/worker.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "input/input_glfw.hpp"
#include <gfx.ecs.components/basic.hpp>
#include <gfx.ecs.components/movement.hpp>
#include <gfx.ecs.components/prototype_proxies.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.file/file.hpp>
#include <gfx.math/datastructure/basic_bvh.hpp>
#include <mygl/mygl.hpp>
#include <random>

struct mesh_info
{};

template<typename T>
struct shape_component : gfx::ecs::component<shape_component<T>>, T
{
    using T::T;
};

struct sphere_t
{
    float radius = 1.f;
};

class collision_system : public gfx::ecs::system
{
    gfx::bvh3d*                           _bvh;
    std::vector<gfx::ecs::shared_entity>* _spheres;

public:
    collision_system(gfx::bvh3d& bvh, std::vector<gfx::ecs::shared_entity>& spheres) : _bvh(&bvh), _spheres(&spheres)
    {
        add_component_type<gfx::transform_component>();
        add_component_type<shape_component<sphere_t>>();
        add_component_type<gfx::movement_component>();
    }

    void update(duration_type delta, gfx::ecs::component_base** components) const override
    {
        auto& tc = components[0]->as<gfx::transform_component>();
        auto& sc = components[1]->as<shape_component<sphere_t>>();
        auto& mc = components[2]->as<gfx::movement_component>();

        const gfx::bounds3f bounds(tc.position - sc.radius, tc.position + sc.radius);

        glm::vec3 impulse = glm::vec3(0);

        _bvh->visit_conditional([&](const gfx::bvh3d::node& n) { return bounds.overlaps(n.aabb); },
                                [&](const gfx::bvh3d::node& a, const gfx::bvh3d::node& b) {
                                    const glm::vec3 da = bounds.center() - a.aabb.center();
                                    const glm::vec3 db = bounds.center() - b.aabb.center();
                                    return dot(da, da) < dot(db, db);
                                },
                                [&](const gfx::bvh3d::node& leaf) {
                                    const auto  index     = leaf.child_left / 2;
                                    const auto& other     = _spheres->at(index);
                                    const auto& other_pos = other->get<gfx::transform_component>()->position;
                                    const auto& other_rad = other->get<shape_component<sphere_t>>()->radius;

                                    const glm::vec3 outwards = tc.position - other_pos;
                                    if (other->get<gfx::transform_component>() == &tc) return false;

                                    const auto lo = length(outwards);
                                    if (lo < other_rad + sc.radius)
                                    { 
                                        const auto no  = normalize(outwards);
                                        const auto val = mc.impulse - dot(mc.impulse, no) * no - (lo - (other_rad + sc.radius))*no / delta.count();
                                        impulse += val;
                                    }
                                    return false;
                                });

        if (tc.position.y <= 0) { 
            impulse.y = -mc.impulse.y - tc.position.y / delta.count();
        }

        if (impulse != glm::vec3(0, 0, 0))
            mc.impulse = impulse;
    }
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    const auto w = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(w);
    mygl::load();

    glDebugMessageCallback([](gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char* message,
                              const void* userParam) { std::cout << message << '\n'; },
                           nullptr);

    gfx::ecs::ecs ecs;
    auto user_entity = ecs.create_entity_shared(gfx::transform_component {glm::vec3 {0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});
    user_entity->get<gfx::projection_component>()->perspective().negative_y = true;
    gfx::ecs::system_list   main_systems;
    gfx::glfw_input_system  input(w);
    gfx::user_camera_system camera_system(input);

    gfx::opengl::instance_proxy<mesh_info> px;
    gfx::instance_system<mesh_info>        instances(px);

    gfx::ecs::system_list physics_systems;

    auto proto = instances.get_instantiator().allocate_prototype("Sphere", gfx::scene_file("sphere.dae").mesh);

    std::mt19937                          g;
    std::uniform_real_distribution<float> d(0, 1);
    std::vector<gfx::ecs::shared_entity> spheres;
    for (int x = 0; x < 1; ++x)
    {
        for (int y = 0; y < 12; ++y)
        {
            for (int z = 0; z < 1; ++z)
            {
                gfx::transform_component           tc(3 * glm::vec3(x, y, z));
                gfx::movement_component  mc;
                mc.impulse = {d(g) * 20, d(g) * 20 + 10, d(g) * 20};
                mc.gravity = 9.81f;
                mc.movement_drag = 0;
                mc.rotation_drag = 0;
                shape_component<sphere_t>          shape;
                gfx::instance_component<mesh_info> si(proto, {});
                spheres.emplace_back(ecs.create_entity_shared(tc, mc, shape, si));
            }
        }
    }
    std::vector<std::uint32_t> sphere_indices(2*spheres.size());
    std::iota(sphere_indices.begin(), sphere_indices.end(), 0u);
    gfx::bvh3d sphere_bvh(2);

    gfx::movement_system movement;
    collision_system     collisions(sphere_bvh, spheres);
    physics_systems.add(movement);
    physics_systems.add(collisions);

    main_systems.add(input);
    main_systems.add(camera_system);
    main_systems.add(instances);

    gfx::worker physics_worker([&](const gfx::timed_while& self, gfx::worker::duration delta) {
        ecs.update(delta, physics_systems);

        sphere_bvh.build_indexed(sphere_indices.begin(), sphere_indices.end(), [&](std::uint32_t idx) {
            const auto& e = spheres[idx / 2];
            const auto& c = e->get<gfx::transform_component>()->position;
            const auto& r = e->get<shape_component<sphere_t>>()->radius;
            return idx % 2 ? (c - glm::vec3(r, r, r)) : (c + glm::vec3(r, r, r));
        });

        return self.value_after(true, std::chrono::milliseconds(8));
    });

    mygl::vertex_array vao;
    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, offsetof(gfx::vertex3d, position));
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal));
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv));
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribBinding(vao, 2, 0);

    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexArrayAttrib(vao, 3 + i);
        glVertexArrayAttribFormat(vao, 3 + i, 4, GL_FLOAT, false,
                                  offsetof(gfx::opengl::instance_proxy<mesh_info>::basic_instance, transform) + i * sizeof(glm::vec4));
        glVertexArrayAttribBinding(vao, 3 + i, 1);
    }
    glVertexArrayBindingDivisor(vao, 1, 1);

    const char* vss = R"(#version 460 core
    layout(location=0) in vec3 pos; layout(location=1) in vec3 norm; layout(location=2) in vec2 uv;
    layout(location=3) in mat4 mm; uniform mat4 vpm;
    layout(location=0) out vec3 normal;
    void main() { normal = (inverse(transpose(mm))*vec4(norm, 0)).xyz; gl_Position = vpm*mm*vec4(pos, 1); }
    )";
    auto        vs  = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vss, nullptr);
    glCompileShader(vs);

    const char* css = R"(#version 460 core
    layout(location=0) in vec3 normal; layout(location=0) out vec3 color; void main() { color=normal; } )";
    auto        fs  = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &css, nullptr);
    glCompileShader(fs);

    auto pr = glCreateProgram();
    glAttachShader(pr, vs);
    glAttachShader(pr, fs);
    glLinkProgram(pr);

    std::string log(1024, ' ');
    int         l;
    glGetProgramInfoLog(pr, 1024, &l, log.data());
    std::cout << log << '\n';

    const auto loc_vpm = glGetUniformLocation(pr, "vpm");

    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);    // What the actual f*?
    glDepthFunc(GL_GEQUAL);
    glDepthRangef(0.f, 1.f);

    glClearColor(1, 1, 1, 1);
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(w))
    {
        const double delta = glfwGetTime() - last;
        last               = glfwGetTime();
        ecs.update(delta, main_systems);

        gfx::camera_matrices cam = *gfx::get_camera_info(*user_entity);

        glUseProgram(pr);
        glUniformMatrix4fv(loc_vpm, 1, false, value_ptr(cam.projection * cam.view));

        glClearNamedFramebufferfv(mygl::framebuffer::zero(), GL_COLOR, 0, glm::value_ptr(glm::vec4(0.7f, 0.9f, 1.f, 1.f)));
        glClearNamedFramebufferfi(mygl::framebuffer::zero(), GL_DEPTH_STENCIL, 0, 0.f, 0);

        glBindVertexArray(vao);
        glBindVertexBuffer(0, px.vertex_buffer().get_buffer(), 0, sizeof(gfx::vertex3d));
        glBindVertexBuffer(1, px.instances_mapped().get_buffer(), 0, sizeof(decltype(px.instances_mapped()[0])));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, px.index_buffer().get_buffer());
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, px.instances_mapped().get_buffer());
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, px.instances_mapped().size(),
                                    sizeof(gfx::prototype_instantiator<mesh_info>::basic_instance));

        px.swap();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
}