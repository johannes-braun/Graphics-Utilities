#pragma once

#include "glm/glm.hpp"

namespace gfx::ui
{
    struct rect
    {
        glm::vec2 min;
        glm::vec2 max;
        rect& inset(float left, float top, float right, float bottom);
        rect& clip(const rect& other);
        const rect& inset(float left, float top, float right, float bottom) const;
        bool contains(glm::vec2 point) const noexcept;
        bool operator==(const rect& other) const noexcept;
        bool operator!=(const rect& other) const noexcept;
    };
}