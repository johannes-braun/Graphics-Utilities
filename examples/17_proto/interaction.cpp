#include "interaction.hpp"

void interaction_processor::add_entity(gfx::ecs::entity e)
{
    auto it = std::find_if(_entities.begin(), _entities.end(), [&](const entity_internal& ei) { return ei.handle == e; });
    if (it == _entities.end())
    {
        _entities.push_back({e});
        it = std::prev(_entities.end());
    }
    for (size_t i = 0; i < _interactions.size(); i++) compute_interactions(*it, i);
}

interaction_processor::interaction_processor(gfx::ecs::ecs& ecs) : _ecs(ecs)
{
    _ecs.add_listener(*this);
    add_component_id(transform_component::id);
    add_component_id(collider_component::id);
}

void interaction_processor::on_add(gfx::ecs::entity e)
{
    add_entity(e);
}

void interaction_processor::on_remove(gfx::ecs::entity e)
{
    if (std::find(_entities_removed.begin(), _entities_removed.end(), e) == _entities_removed.end()) _entities_removed.push_back({e});
}

void interaction_processor::on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id && e.get<collider_component>())
        add_entity(e);
    else if (id == collider_component::id && e.get<transform_component>())
        add_entity(e);
    else if (e.get<transform_component>() && e.get<collider_component>())
        if (std::find(_entities_updated.begin(), _entities_updated.end(), e) == _entities_updated.end()) _entities_updated.push_back({e});
}

void interaction_processor::on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id || id == collider_component::id)
        on_remove(e);
    else if (e.get<transform_component>() && e.get<collider_component>())
        if (std::find(_entities_updated.begin(), _entities_updated.end(), e) == _entities_updated.end()) _entities_updated.push_back({e});
}

void interaction_processor::process(double delta)
{
    clear_entities();
    std::sort(_entities.begin(), _entities.end(), [this](const gfx::ecs::entity_handle a, const gfx::ecs::entity_handle b) {
        const auto min1 = _ecs.get_component<collider_component>(a)->aabb.min[_max_variance_axis];
        const auto min2 = _ecs.get_component<collider_component>(b)->aabb.min[_max_variance_axis];
        return min1 < min2;
    });

    glm::vec3 centroid_sum(0.f);
    glm::vec3 centroid_sum2(0.f);
    for (auto i = 0ull; i < _entities.size(); ++i)
    {
        const auto& aabb = _ecs.get_component<collider_component>(_entities[i].handle)->aabb;

        const auto c = aabb.center();
        centroid_sum += c;
        centroid_sum2 += c * c;

        for (auto j = i + 1; j < _entities.size(); ++j)
        {
            const auto& target_aabb = _ecs.get_component<collider_component>(_entities[j].handle)->aabb;

            if (aabb.max[_max_variance_axis] < target_aabb.min[_max_variance_axis]) break;

            if (gfx::intersects(aabb, target_aabb))
            {
                
            }
        }
    }

    centroid_sum /= _entities.size();
    centroid_sum2 /= _entities.size();
    const auto variance = centroid_sum2 - (centroid_sum * centroid_sum);

    _max_variance_axis = 0;
    if (variance[_max_variance_axis] < variance[1]) _max_variance_axis = 1;
    if (variance[_max_variance_axis] < variance[2]) _max_variance_axis = 2;
}

void interaction_processor::add_interaction(interaction& i)
{
    _interactions.push_back(&i);
    const auto index = _interactions.size() - 1;
    for (auto& e : _entities) compute_interactions(e, index);
}

namespace detail {
template<typename T, typename Iterator>
auto unstable_remove(std::vector<T>& vec, Iterator at)
{
    const auto d = std::distance(vec.begin(), at);
    std::iter_swap(vec, std::prev(vec.end()));
    return vec.begin() + d;
}
}    // namespace detail

void interaction_processor::clear_entities()
{
    if (!_entities_removed.empty())
    {
		for (auto it = _entities.begin(); it != _entities.end(); ++it)
		{
            for (auto& r : _entities_removed)
            {
                if (it->handle == r)
                    it = detail::unstable_remove(_entities, it);
				if (it == _entities.end())
					break;
            }
			if (it == _entities.end())
				break;
        }
    }
    if (!_entities_updated.empty())
    {
        for (auto& e : _entities)
        {
            for (auto it = _entities_updated.begin(); it != _entities_updated.end(); ++it)
            {
                if (e.handle == *it)
                {
                    e.interactors.clear();
                    e.interactees.clear();
                    for (size_t k = 0; k < _interactions.size(); k++) { compute_interactions(e, k); }
                    it = detail::unstable_remove(_entities_updated, it);
                }
            }
        }
    }
    _entities.resize(_entities.size() - _entities_removed.size());
    _entities_removed.clear();
    _entities_updated.clear();
}

void interaction_processor::compute_interactions(entity_internal& e, uint32_t interaction_index)
{
    const auto* interaction = _interactions[interaction_index];
    const auto& iactors     = interaction->interactor_types();
    const auto& iactees     = interaction->interactee_types();
    const auto& nulleq      = [&, this](gfx::ecs::id_t id) { return _ecs.get_component(e.handle, id) == nullptr; };
    if (std::find_if(iactors.begin(), iactors.end(), nulleq) == iactors.end()) e.interactors.push_back(interaction_index);
    if (std::find_if(iactees.begin(), iactees.end(), nulleq) == iactees.end()) e.interactees.push_back(interaction_index);
}
