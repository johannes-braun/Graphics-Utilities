#pragma once

#include "bvh.hpp"
#include <array>
#include <atomic>
#include <memory>

namespace gfx
{
    using line_space_bounds = bvh<3>::bounds;

    class line_space
    {
    public:
        struct line
        {
            int triangle;
        };

        line_space(int x, int y, int z);
        void generate(const bvh<3>& bvh);
        void generate(const bvh<3>& bvh, line_space_bounds bounds);
        const std::array<std::array<std::vector<line>, 6>, 6>& storage() const noexcept;

        int size_x() const noexcept;
        int size_y() const noexcept;
        int size_z() const noexcept;
        bool empty() const noexcept;

    private:
        int _subdivision_x;
        int _subdivision_y;
        int _subdivision_z;
        bool _empty;

        std::array<std::array<std::vector<line>, 6>, 6> _storages;
    };
}

#include "linespace.inl"