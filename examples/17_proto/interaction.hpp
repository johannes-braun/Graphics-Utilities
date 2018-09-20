#pragma once

#include "gfx/ecs/ecs.hpp"
#include <gfx/gfx.hpp>

struct transform_component : gfx::ecs::component<transform_component>
{
    gfx::transform value;
};

struct collider_component : gfx::ecs::component<collider_component>
{
	gfx::bounds3f aabb;
};

class interaction : public gfx::ecs::listener
{
public:
    interaction(gfx::ecs::ecs& ecs);

    void on_add(gfx::ecs::entity e) override;
    void on_remove(gfx::ecs::entity e) override;
    void on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;
    void on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;

	void process(double delta);

private:
	void clear_entities();

	gfx::ecs::ecs& _ecs;
	uint32_t _max_variance_axis = 0;
    std::vector<gfx::ecs::entity_handle> _entity_handles;
    std::vector<gfx::ecs::entity_handle> _entities_removed;
	std::vector<gfx::ecs::entity_handle> _entities_sorted;
};
