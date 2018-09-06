#pragma once

#include "component.hpp"
#include <algorithm>
#include <execution>
#include <gfx/data/flags.hpp>

namespace gfx {
inline namespace v1 {
namespace ecs {
enum class component_flag : uint32_t
{
    optional = 1 << 0,
};
using component_flags = gfx::flags<uint32_t, component_flag>;

class system_base
{
public:
    virtual ~system_base()                = default;
    system_base()                         = default;
    system_base(const system_base& other) = default;
    system_base(system_base&& other)      = default;
    system_base& operator=(const system_base& other) = default;
    system_base& operator=(system_base&& other) = default;

    virtual void                        update(double delta, component_base** components) const;
    const std::vector<id_t>&            types() const;
    const std::vector<component_flags>& flags() const;

protected:
    void add_component_type(id_t id, component_flags flags = {});

private:
    std::vector<id_t>            _component_types;
    std::vector<component_flags> _component_flags;
};

using system = system_base;

class system_list
{
public:
    void add(system_base& system);
    bool remove(system_base& system);

    system_base&       operator[](uint32_t index);
    const system_base& operator[](uint32_t index) const;
    system_base&       at(uint32_t index);
    const system_base& at(uint32_t index) const;
    size_t             size() const noexcept;

private:
    std::vector<std::reference_wrapper<system_base>> _systems;
};

}    // namespace ecs
}    // namespace v1
}    // namespace gfx