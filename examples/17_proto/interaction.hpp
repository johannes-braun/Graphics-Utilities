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

class interaction
{
public:
	virtual void interact(double delta, gfx::ecs::component_base** interactor, gfx::ecs::component_base** interactee) = 0;

	const std::vector<gfx::ecs::id_t>& interactor_types() const noexcept { return _interactor_types; }
	const std::vector<gfx::ecs::id_t>& interactee_types() const noexcept { return _interactee_types; }
protected:
    void add_interactor_type(gfx::ecs::id_t id)
    {
		_interactor_types.emplace_back(id);
    }
	void add_interactee_type(gfx::ecs::id_t id)
	{
		_interactee_types.emplace_back(id);
	}

private:
	std::vector<gfx::ecs::id_t> _interactor_types;
	std::vector<gfx::ecs::id_t> _interactee_types;
};

class interaction_processor : public gfx::ecs::listener
{
public:
    interaction_processor(gfx::ecs::ecs& ecs);

    void on_add(gfx::ecs::entity e) override;
    void on_remove(gfx::ecs::entity e) override;
    void on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;
    void on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id) override;

	void process(double delta);

    void add_interaction(interaction& i);

private:
	struct entity_internal
	{
		gfx::ecs::entity_handle handle;
		std::vector<uint32_t> interactors;
		std::vector<uint32_t> interactees;

		bool operator==(const entity_internal& e) const noexcept { return handle == e.handle; }
		bool operator<(const entity_internal& b) const noexcept { return handle < b.handle; };
	};

	void add_entity(gfx::ecs::entity e);
	void clear_entities();
	void compute_interactions(entity_internal& e, uint32_t interaction_index);

	gfx::ecs::ecs& _ecs;
	uint32_t _max_variance_axis = 0;
    std::vector<entity_internal> _entities;
    std::vector<gfx::ecs::entity_handle> _entities_removed;
    std::vector<gfx::ecs::entity_handle> _entities_updated;
	std::vector<interaction*> _interactions;
};
