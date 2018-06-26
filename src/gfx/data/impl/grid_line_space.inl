#pragma once

namespace gfx
{
    grid_line_space::grid_line_space(int count_x, int count_y, int count_z, int size_x, int size_y, int size_z)
        : _count_x(count_x), _count_y(count_y), _count_z(count_z), _line_spaces(count_x * count_y * count_z, gfx::line_space(size_x, size_y, size_z))
    {

    }

    void grid_line_space::generate(const bvh<3>& bvh)
    {
        _bounds = bvh.get_bounds();
        const glm::vec3 qsize = _bounds.size() / glm::vec3{ _count_x, _count_y, _count_z };

        for (int z = 0; z < _count_z; ++z) for (int y = 0; y < _count_y; ++y) for (int x = 0; x < _count_x; ++x)
        {
            int index = z * _count_y * _count_x + y * _count_x + x;
            gfx::line_space& ls = _line_spaces[index];

            bounds3f lsb;
            lsb.min = _bounds.min + glm::vec3(x, y, z) * qsize;
            lsb.max = lsb.min + qsize;
            ls.generate(bvh, lsb);

            if(grid_updated) grid_updated(float(z * _count_y * _count_x + y * _count_x + x) / size());
        }
    }

    const std::vector<gfx::line_space>& grid_line_space::line_spaces() const noexcept
    {
        return _line_spaces;
    }

    int grid_line_space::size_x() const noexcept
    {
        return _count_x;
    }

    int grid_line_space::size_y() const noexcept
    {
        return _count_y;
    }

    int grid_line_space::size_z() const noexcept
    {
        return _count_z;
    }

    int grid_line_space::size() const noexcept
    {
        return _count_x * _count_y * _count_z;
    }

    const bounds3f& grid_line_space::bounds() const noexcept
    {
        return _bounds;
    }

    gfx::line_space& grid_line_space::at(int position)
    {
        return _line_spaces[position];
    }

    const gfx::line_space& grid_line_space::at(int position) const
    {
        return _line_spaces[position];
    }

    gfx::line_space& grid_line_space::operator[](int position)
    {
        return _line_spaces[position];
    }

    const gfx::line_space& grid_line_space::operator[](int position) const
    {
        return _line_spaces[position];
    }
}