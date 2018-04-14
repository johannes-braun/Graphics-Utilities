#pragma once

#include "line_space.hpp"
#include <functional>

namespace gfx
{
    std::function<void(float)> grid_updated;

    class grid_line_space
    {
    public:
        grid_line_space(int count_x, int count_y, int count_z, int size_x, int size_y, int size_z);

        void generate(const bvh<3>& bvh);
        const std::vector<gfx::line_space>& line_spaces() const noexcept;

        int size_x() const noexcept;
        int size_y() const noexcept;
        int size_z() const noexcept;
        int size() const noexcept;
        const bounds3f& bounds() const noexcept;

        gfx::line_space& at(int position);
        const gfx::line_space& at(int position) const;

        gfx::line_space& operator[](int position);
        const gfx::line_space& operator[](int position) const;

    private:
        int _count_x;
        int _count_y;
        int _count_z;
        bounds3f _bounds;
        std::vector<gfx::line_space> _line_spaces;
    };
}

#include "impl/grid_line_space.inl"