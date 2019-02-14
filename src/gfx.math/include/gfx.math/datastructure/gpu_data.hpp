#pragma once

#include "line_space.hpp"
#include "basic_bvh.hpp"

namespace gfx::gpu
{
    struct line_space_data
    {
        alignas(16) bounds3f bounds;
        alignas(16) int x;
        alignas(4)  int y;
        alignas(4)  int z;
        alignas(4)  int empty;
        alignas(16) uint64_t storages[6][6];
    };

    struct grid_line_space_data
    {
        alignas(16) bounds3f bounds;
        alignas(16) int x;
        alignas(4)  int y;
        alignas(4)  int z;
        alignas(16) uint64_t line_spaces;
    };
}