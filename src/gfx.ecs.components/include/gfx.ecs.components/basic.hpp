#pragma once

#include <gfx.ecs/component.hpp>
#include <gfx.math/geometry.hpp>

namespace gfx
{
    inline namespace v1
    {
        using transform_component = ecs::simple_component<transform>;
        using projection_component = ecs::simple_component<projection>;
    }
}