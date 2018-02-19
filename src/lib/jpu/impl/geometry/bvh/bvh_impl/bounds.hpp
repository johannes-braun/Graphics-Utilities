#pragma once

#include "xyzw.hpp"

#ifndef JPU_BVH_INL
#define JPU_BVH_INL
#endif

namespace jpu::impl
{
    struct bounds
    {
        union
        {
            xyzw data[2];
            struct { xyzw min, max; };
        };

        bounds();
        bounds(xyzw min, xyzw max);

        bounds(const bounds& o);
        bounds(bounds&& o) noexcept;
        bounds& operator=(const bounds& rhs);
        bounds& operator=(bounds&& rhs) noexcept;

        xyzw& operator[](size_t i);
        const xyzw& operator[](size_t i) const;
        xyzw size() const;

        xyzw center() const;

        float surface() const;

        bool empty() const;

        void expand(xyzw other);

        void expand(const bounds& other);
    };
}
