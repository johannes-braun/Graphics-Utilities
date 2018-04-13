#pragma once

struct line
{
    int triangle;
};

line get_line_grid(uintptr_t grid_line_space, vec3 origin, vec3 direction, float max_distance);
line get_line(uintptr_t line_space, int index, vec3 origin, vec3 direction, float max_distance);

#include "impl/line_space_impl.inl"