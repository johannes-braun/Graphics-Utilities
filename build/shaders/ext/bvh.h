// BVH buffer format:
//  1) 4B uint position attribute stride in vertex buffer (e.g. size of the base struct containing the position)
//  2) 4B uint position attribute offset in vertex buffer (e.g. 0 if its the first struct member)
//  3) 4B uint index attribute stride in element array buffer
//  4) 4B uint index attribute offset in element array buffer
//  5) The full array of filled nodes
//  
// Root node is defined as follows:
// bvh_node* root_node = (bvh_node*)(bvh+4*4)
//
// Usage of this shader:
//
// When compiling, you can decide whether the traversal should include the farthest intersection.
// It's enabled by default, but you can disable it by defining
//          DISABLE_FARTHEST
// before including this file.
//
// Set the desired bvh traversal mode with
//          bvh_state_set_mode(uint mode)
// 
// The traversal will be processed by the
//          bvh_result bvh_hit(const vec3 origin, const vec3 direction, 
//              const uintptr_t bvh, const uintptr_t elements, 
//              const uintptr_t vertices, const float max_distance);
// function. See below for more information about the parameters.
// 
// 
#pragma once

// Will be returned after traversal.
struct bvh_result
{
    // The index of the nearest and farthest triangles. 
    // (multiply by 3 to get the element index)
    uint near_triangle;
    uint far_triangle;

    float near_distance;
    float far_distance;

    // Only two barycentric coordinates are needed.
    // Calculate the last one with z = 1 - bary.x - bary.y .
    vec2 near_barycentric;
    vec2 far_barycentric;

    bool hits;
};

// Any hit means that the first hit that is being found when traversing will be returned.
// Nearest hit means that all traversed candidates will be tested for their distance on the ray.
//      The nearest (and in cases the farthest) intersections will be returned.
const uint bvh_mode_any = 0;
const uint bvh_mode_nearest = 1;
void bvh_state_set_mode(uint mode);

// Traverse BVH and search for any or the nearest intersection, depending on the current mode set.
// see bvh_state_set_mode(uint).
bvh_result bvh_hit(
    // Ray in local space 
    // (already transformed relative to the Mesh transformation)
    const vec3 origin, const vec3 direction,
    // Data (as bindless buffer pointers). The data has to be compatible 
    // with the given bvh buffer layout and the assigned attribute data.
    const uintptr_t bvh, const uintptr_t elements, const uintptr_t vertices,
    // Search distance on ray
    const float max_distance);

#include "impl/bvh_impl.inl"