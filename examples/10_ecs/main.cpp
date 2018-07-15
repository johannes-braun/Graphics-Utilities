#include <gfx/gfx.hpp>

struct transformation : gfx::ecs::component<transformation>
{
    gfx::transform transform;
};

struct rigid_body : gfx::ecs::component<rigid_body>
{
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float     mass = 1.f;
};

struct s1 : gfx::ecs::system
{
    s1()
    {
        add_component_type(rigid_body::id);
        add_component_type(transformation::id);
    }

    virtual void update(double delta, gfx::ecs::component_base** components) const
    {
        rigid_body&     rb    = components[0]->as<rigid_body>();
        transformation& trafo = components[1]->as<transformation>();

        rb.velocity += delta * glm::vec3(0, -9.81f, 0) * rb.mass + delta * rb.acceleration;

        const auto next          = trafo.transform.position + rb.velocity;
        trafo.transform.position = next;
        if (next.y < 0) {
            rb.velocity                = -rb.velocity;
            trafo.transform.position.y = 0;
        }
    }
};

int main()
{
    gfx::ecs::ecs    ecs;
    gfx::ecs::unique_entity entity = ecs.create_entity_unique();
    entity->add(transformation(), rigid_body());
    entity->get<transformation>()->transform.position = {0, 20, 0};

    s1                    gravity;
    gfx::ecs::system_list physics;
    physics.add(gravity);

    while (true) {
        auto n = std::chrono::steady_clock::now();
        gfx::ilog << to_string(entity->get<transformation>()->transform.position);

        ecs.update(0.01, physics);

        using namespace std::chrono_literals;
        while (std::chrono::steady_clock::now() - n < 10ms)
            ;
    }
}