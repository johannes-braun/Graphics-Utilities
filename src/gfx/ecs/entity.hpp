#pragma once

#include "component.hpp"
#include <memory>
#include <vector>

namespace gfx {
inline namespace v1 {
namespace ecs {
using entity_info    = std::vector<std::pair<id_t, uint32_t>>;
using indexed_entity = std::pair<uint32_t, entity_info>;

namespace traits {
template<typename T>
struct is_component : std::is_convertible<std::decay_t<T>&, component_base&>
{};

template<typename T>
struct is_component<component<T>> : std::true_type
{};

template<typename T>
using enable_if_component_t = std::enable_if_t<is_component<T>::value>;
}    // namespace traits

class ecs;
class listener;
class entity
{
    friend class ecs;
    friend struct entity_deleter;
	friend class listener;

public:
    entity() = default;

    template<typename... Component>
    void add(const Component&... component);
    template<typename Component, typename... Components>
    bool remove();
    template<typename Component>
    Component* get();

    operator entity_handle() const noexcept;
    operator bool() const noexcept;

private:
    entity(ecs* e, entity_handle hnd);

    ecs*          _ecs    = nullptr;
    entity_handle _handle = nullptr;
};

struct entity_deleter
{
    void operator()(entity* e) const;
};
using unique_entity = std::unique_ptr<entity, entity_deleter>;
}    // namespace ecs
}    // namespace v1
}    // namespace gfx
