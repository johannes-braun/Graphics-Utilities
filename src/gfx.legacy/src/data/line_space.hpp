#pragma once

#include "bvh.hpp"
#include <array>
#include <atomic>
#include <memory>
#include <gfx.math/geometry.hpp>

namespace gfx
{
    class line_space
    {
    public:
        struct line { int triangle; };

        line_space(int x, int y, int z);
        void generate(const bvh<3>& bvh);
        void generate(const bvh<3>& bvh, bounds3f bounds);
        const std::array<std::array<std::vector<line>, 6>, 6>& storage() const noexcept;

        int size_x() const noexcept;
        int size_y() const noexcept;
        int size_z() const noexcept;
        bool empty() const noexcept;
        const bounds3f& bounds() const noexcept;

    private:
        int _subdivision_x;
        int _subdivision_y;
        int _subdivision_z;
        bool _empty;
        bounds3f _bounds;

        std::array<std::array<std::vector<line>, 6>, 6> _storages;
    };
}