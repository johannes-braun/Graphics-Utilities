#pragma once

bool intersect_bounds(const vec3 origin, const vec3 direction, const vec3 bounds_min, const vec3 bounds_max, const float max_distance);

bool intersect_triangle(const vec3 origin, const vec3 direction, const vec3 v1, const vec3 v2, const vec3 v3, inout float t, inout vec2 barycentric);

#include "impl/intersect_impl.inl"