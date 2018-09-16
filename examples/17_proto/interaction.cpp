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

interaction::interaction()
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
    const int axis = max_variance_axis();
}

void interaction::clear_entities()
{
    if (_entities_removed.empty()) return;
    std::set_difference(std::execution::seq, _entity_handles.begin(), _entity_handles.end(), _entities_removed.begin(),
                        _entities_removed.end(), _entity_handles.begin());
    _entity_handles.resize(_entity_handles.size() - _entities_removed.size());
	_entities_removed.clear();
}

int interaction::max_variance_axis()
{
	return 0;
}
