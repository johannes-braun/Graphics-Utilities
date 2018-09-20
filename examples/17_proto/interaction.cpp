#include "interaction.hpp"
#include <functional>

template<typename T, typename S, typename Pred = std::less<T>,
         typename = std::enable_if_t<std::is_convertible_v<std::decay_t<S>, std::decay_t<T>>>>
auto insert_uniquely_sorted(std::vector<T>& vec, const S& item, Pred pred = Pred())
{
    const auto it = std::upper_bound(vec.begin(), vec.end(), item, pred);
    if (it != vec.begin() && *std::prev(it) == item) return vec.end();
    return vec.insert(it, item);
}

interaction::interaction(gfx::ecs::ecs& ecs)
	: _ecs(ecs)
{
    add_component_id(transform_component::id);
    add_component_id(collider_component::id);
}

void interaction::on_add(gfx::ecs::entity e)
{
    insert_uniquely_sorted(_entity_handles, e);
}

void interaction::on_remove(gfx::ecs::entity e)
{
    insert_uniquely_sorted(_entities_removed, e);
}

void interaction::on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id && e.get<collider_component>()) insert_uniquely_sorted(_entity_handles, e);
    if (id == collider_component::id && e.get<transform_component>()) insert_uniquely_sorted(_entity_handles, e);
}

void interaction::on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id || id == collider_component::id) insert_uniquely_sorted(_entities_removed, e);
}

void interaction::process(double delta)
{
    clear_entities();
	_entities_sorted = _entity_handles;
	std::sort(_entities_sorted.begin(), _entities_sorted.end(), [this](const gfx::ecs::entity_handle a, const gfx::ecs::entity_handle b) {
		const auto min1 = _ecs.get_component<collider_component>(a)->aabb.min[_max_variance_axis];
		const auto min2 = _ecs.get_component<collider_component>(b)->aabb.min[_max_variance_axis];
		return min1 < min2;
	});

	glm::vec3 centroid_sum;
	glm::vec3 centroid_sum2;
	for (auto i = 0ull; i < _entities_sorted.size(); ++i)
	{
		const auto& aabb = _ecs.get_component<collider_component>(_entities_sorted[i])->aabb;

		const auto c = aabb.center();
		centroid_sum += c;
		centroid_sum2 += c*c;

		for (auto j = i+1; j < _entities_sorted.size(); ++j)
		{
			const auto& target_aabb = _ecs.get_component<collider_component>(_entities_sorted[j])->aabb;

			if (aabb.max[_max_variance_axis] < target_aabb.min[_max_variance_axis])
				break;

			if (gfx::intersects(aabb, target_aabb))
			{

			}
		}
	}

	centroid_sum /= _entities_sorted.size();
	centroid_sum2 /= _entities_sorted.size();
	const auto variance = centroid_sum2 - (centroid_sum * centroid_sum);

	_max_variance_axis = 0;
	if (variance[_max_variance_axis] < variance[1])
		_max_variance_axis = 1;
	if (variance[_max_variance_axis] < variance[2])
		_max_variance_axis = 2;
}

void interaction::clear_entities()
{
    if (_entities_removed.empty()) return;
    std::set_difference(std::execution::seq, _entity_handles.begin(), _entity_handles.end(), _entities_removed.begin(),
                        _entities_removed.end(), _entity_handles.begin());
    _entity_handles.resize(_entity_handles.size() - _entities_removed.size());
	_entities_removed.clear();
}
