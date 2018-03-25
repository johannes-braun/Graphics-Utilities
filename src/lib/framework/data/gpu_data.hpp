#pragma once

#include "line_space.hpp"
#include "bvh.hpp"

namespace gfx::gpu
{
    struct line_space_data
    {
        gfx::line_space_bounds bounds;
        int x;
        int y;
        int z;
        int empty;
        uint64_t storages[6][6];
    };

    struct grid_line_space_data
    {
        gfx::line_space_bounds bounds;
        int x;
        int y;
        int z;
    private:
        int _p;
    public:
        uint64_t line_spaces;
    };
}