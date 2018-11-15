#pragma once

#include "interaction.hpp"
#include "prototype.hpp"
#include <gfx.ecs.defaults/camera.hpp>

struct prototype_component : gfx::ecs::component<prototype_component>
{
    prototype* proto   = nullptr;
	prototype_mesh_property properties[prototype::max_submeshes];
};

class prototype_system : public gfx::ecs::system
{
public:
    prototype_system(prototype_manager& manager) : _manager(manager)
    {
        add_component_type<prototype_component>();
        add_component_type<gfx::transform_component>();
    }

    void update(duration_type delta, gfx::ecs::component_base** components) const override
    {
        auto& proto = components[0]->as<prototype_component>();
        auto& trans = components[1]->as<gfx::transform_component>();

        if (proto.proto) _manager.enqueue(proto.proto, trans.value, proto.properties);
    }

private:
    prototype_manager& _manager;
};