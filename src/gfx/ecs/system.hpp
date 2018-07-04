#pragma once

#include "component.hpp"

namespace gfx::ecs
{
class system_base
{
public:
    virtual ~system_base() = default;
    system_base(const std::vector<uint32_t>& types) : _component_types(types) {}
    virtual void update(double delta, component_base** components) const {}
    const std::vector<uint32_t>& types() const { return _component_types; }
private:
    std::vector<uint32_t> _component_types;
};
} // namespace gfx::ecs