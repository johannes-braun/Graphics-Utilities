#include "xyzw.hpp"

namespace jpu::impl
{
    JPU_BVH_INL xyzw::xyzw(float f) : x(f), y(f), z(f), w(f)
    {}

    JPU_BVH_INL xyzw::xyzw() : xyzw(0)
    {}

    JPU_BVH_INL xyzw::xyzw(float x, float y) : x(x), y(y), z(0), w(0)
    {}

    JPU_BVH_INL xyzw::xyzw(float x, float y, float z) : x(x), y(y), z(z), w(0)
    {}

    JPU_BVH_INL xyzw::xyzw(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
    {}

    JPU_BVH_INL float& xyzw::operator[](size_t i)
    {
        return data[i];
    }

    JPU_BVH_INL const float& xyzw::operator[](size_t i) const
    {
        return data[i];
    }

    JPU_BVH_INL xyzw xyzw::operator+(const xyzw& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }

    JPU_BVH_INL xyzw xyzw::operator-(const xyzw& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }

    JPU_BVH_INL xyzw xyzw::operator*(const xyzw& rhs) const
    {
        return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
    }

    JPU_BVH_INL xyzw xyzw::operator/(const xyzw& rhs) const
    {
        return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
    }

    JPU_BVH_INL xyzw::xyzw(const xyzw& o) : xyzw(o.x, o.y, o.z, o.w)
    {}

    JPU_BVH_INL xyzw::xyzw(xyzw&& o) noexcept : xyzw(o.x, o.y, o.z, o.w)
    {}

    JPU_BVH_INL xyzw& xyzw::operator=(const xyzw& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    JPU_BVH_INL xyzw& xyzw::operator=(xyzw&& rhs) noexcept
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }
}