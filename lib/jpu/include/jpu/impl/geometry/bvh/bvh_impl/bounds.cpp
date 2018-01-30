#include "bounds.hpp"

namespace jpu::impl
{
    JPU_BVH_INL bounds::bounds(): min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::lowest())
    {
    }

    JPU_BVH_INL bounds::bounds(xyzw min, xyzw max): min(min), max(max)
    {
    }

    JPU_BVH_INL bounds::bounds(const bounds& o): bounds(o.min, o.max)
    {
    }

    JPU_BVH_INL bounds::bounds(bounds&& o) noexcept: bounds(o.min, o.max)
    {
    }

    JPU_BVH_INL bounds& bounds::operator=(const bounds& rhs)
    {
        min = rhs.min;
        max = rhs.max;
        return *this;
    }

    JPU_BVH_INL bounds& bounds::operator=(bounds&& rhs) noexcept
    {
        min = rhs.min;
        max = rhs.max;
        return *this;
    }

    JPU_BVH_INL xyzw& bounds::operator[](size_t i)
    {
        return data[i];
    }

    JPU_BVH_INL const xyzw& bounds::operator[](size_t i) const
    {
        return data[i];
    }

    JPU_BVH_INL xyzw bounds::size() const
    {
        return xyzw(max.x - min.x, max.y - min.y, max.z - min.z, max.w - min.w);
    }

    JPU_BVH_INL xyzw bounds::center() const
    {
        return xyzw((max.x + min.x) / 2.f,
                    (max.y + min.y) / 2.f,
                    (max.z - min.z) / 2.f,
                    (max.w - min.w) / 2.f);
    }

    JPU_BVH_INL float bounds::surface() const
    {
        const auto s = size();
        return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z);
    }

    JPU_BVH_INL bool bounds::empty() const
    {
        constexpr auto eps = std::numeric_limits<float>::epsilon();
        const auto s = size();
        return (s.x <= eps) || (s.y <= eps) || (s.z <= eps);
    }

    JPU_BVH_INL void bounds::expand(xyzw other)
    {
        min = xyzw(std::min(min.x, other.x), std::min(min.y, other.y),
                   std::min(min.z, other.z), std::min(min.w, other.w));
        max = xyzw(std::max(max.x, other.x), std::max(max.y, other.y),
                   std::max(max.z, other.z), std::max(max.w, other.w));
    }

    JPU_BVH_INL void bounds::expand(const bounds& other)
    {
        min = xyzw(std::min(min.x, other.min.x), std::min(min.y, other.min.y),
                   std::min(min.z, other.min.z), std::min(min.w, other.min.w));
        max = xyzw(std::max(max.x, other.max.x), std::max(max.y, other.max.y),
                   std::max(max.z, other.max.z), std::max(max.w, other.max.w));
    }
}
