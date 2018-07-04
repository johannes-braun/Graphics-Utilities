#pragma once
#include <cstdint>
#include <vector>
#include <cstddef>

namespace gfx::ecs
{
enum class id_t : uint64_t {};
struct component_base;
using entity_handle                       = void*;
constexpr const entity_handle null_entity = nullptr;
using component_creator_fun               = ptrdiff_t(*)(std::vector<std::byte>& memory, entity_handle entity, component_base* base_component);
using component_deleter_fun               = void(*)(component_base* base_component);

struct component_base
{
    entity_handle entity = null_entity;

    static auto           get_creator(id_t id) { return std::get<component_creator_fun>(_types[static_cast<size_t>(id)]); }
    static auto get_deleter(id_t id) { return std::get<component_deleter_fun>(_types[static_cast<size_t>(id)]); }
    static size_t         type_size(id_t id) { return std::get<size_t>(_types[static_cast<size_t>(id)]); }
    static bool           is_valid(id_t id) { return static_cast<size_t>(id) < _types.size(); }

protected:
    static id_t register_type(component_creator_fun create, component_deleter_fun deleter, size_t size)
    {
        const id_t id{_types.size()};
        _types.emplace_back(create, deleter, size);
        return id;
    }

private:
    static inline std::vector<std::tuple<component_creator_fun, component_deleter_fun, size_t>> _types;
};

template <typename C> ptrdiff_t create(std::vector<std::byte>& memory, entity_handle entity, component_base* base_component)
{
    const ptrdiff_t ptr = memory.size();
    memory.resize(memory.size() + C::size);
    C* component      = new(&memory[ptr]) C(*static_cast<C*>(base_component));
    component->entity = entity;
    return ptr;
}

template <typename C> void destroy(component_base* base_component)
{
    C* component = static_cast<C*>(base_component);
    component->~C();
}

template <typename T> struct component : component_base
{
    using type                   = T;
    static constexpr size_t size = sizeof(type);

    static inline const id_t                   id      = register_type(create<type>, destroy<type>, size);
    static inline component_creator_fun creator = create<type>;
    static inline component_deleter_fun deleter = destroy<type>;
};
} // namespace gfx::ecs