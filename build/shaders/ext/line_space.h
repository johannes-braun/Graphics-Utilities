#pragma once

struct line
{
    int triangle;
};


vec3 get_line(uintptr_t line_space, vec3 origin, vec3 direction, float max_distance);

#include "impl/line_space_impl.inl"