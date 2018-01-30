#pragma once

#ifndef JPU_BVH_INL
#define JPU_BVH_INL
#endif

namespace jpu::impl
{
    struct xyzw
    {
        union
        {
            float data[4];
            struct { float x, y, z, w; };
        };

        xyzw(float f);
        xyzw();
        xyzw(float x, float y);
        xyzw(float x, float y, float z);
        xyzw(float x, float y, float z, float w);
        float& operator[](size_t i);
        const float& operator[](size_t i) const;

        xyzw operator+(const xyzw& rhs) const;
        xyzw operator-(const xyzw& rhs) const;
        xyzw operator*(const xyzw& rhs) const;
        xyzw operator/(const xyzw& rhs) const;

        xyzw(const xyzw& o);
        xyzw(xyzw&& o) noexcept;
        xyzw& operator=(const xyzw& rhs);
        xyzw& operator=(xyzw&& rhs) noexcept;
    };
}