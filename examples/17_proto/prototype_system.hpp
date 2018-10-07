#pragma once

#include "interaction.hpp"
#include "prototype.hpp"
#include <gfx/gfx.hpp>

struct prototype_component : gfx::ecs::component<prototype_component>
{
    prototype* proto   = nullptr;
    bool       visible = true;
    glm::vec4  color   = {1.f, 1.f, 1.f, 1.f};
};

class prototype_system : public gfx::ecs::system
{
public:
    prototype_system(prototype_manager& manager) : _manager(manager)
    {
        add_component_type<prototype_component>();
        add_component_type<gfx::transform_component>();
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        auto& proto = components[0]->as<prototype_component>();
        auto& trans = components[1]->as<gfx::transform_component>();

        if (proto.proto && proto.visible) _manager.enqueue(proto.proto, trans.value, proto.color);
    }

private:
    prototype_manager& _manager;
};