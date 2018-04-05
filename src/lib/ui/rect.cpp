#include "rect.hpp"

namespace gfx::ui
{
    rect& rect::inset(float left, float top, float right, float bottom)
    {
        min += glm::vec2{ left, bottom };
        max -= glm::vec2{ right, top };
        return *this;
    }

    rect& rect::clip(const rect& other)
    {
        min = clamp(min, other.min, other.max);
        max = clamp(max, other.min, other.max);
        return *this;
    }

    const rect& rect::inset(float left, float top, float right, float bottom) const
    {
        rect r = *this;
        return r.inset(left, top, right, bottom);
    }

    bool rect::contains(glm::vec2 point) const noexcept
    {
        return clamp(point, min, max) == point;
    }

    bool rect::operator==(const rect& other) const noexcept
    {
        return min == other.min && max == other.max;
    }

    bool rect::operator!=(const rect& other) const noexcept
    {
        return !(min == other.min && max == other.max);
    }
}