#pragma once

#include "gfx/ecs/ecs.hpp"
#include <gfx/gfx.hpp>

struct transform_component : gfx::ecs::component<transform_component>
{
    gfx::transform value;
};

struct collider_component : gfx::ecs::component<collider_component>
{
    
};

class interaction : public gfx::ecs::listener
{
public:
    interaction();

    void on_add(gfx::ecs::entity e) override;
    void on_remove(gfx::ecs::entity e) override;
    void on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;
    void on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;

private:
    std::vector<gfx::ecs::entity_handle> _entity_handles;
    std::vector<gfx::ecs::entity_handle> _entities_removed;
};
