#pragma once

#include "intersect_impl.inl"

struct bvh_attribute
{
    uint position_stride;
    uint position_offset;
    uint index_stride;
    uint index_offset;
};

const uint bvh_node_type_inner = 0;
const uint bvh_node_type_leaf = 1;
struct bvh_node
{
    vec4 bounds_min;
    vec4 bounds_max;
    uint type;
    uint left;
    uint right;
    uint parent;
};

uint _bvh_mode_current = bvh_mode_nearest;
void bvh_state_set_mode(uint mode)
{
    _bvh_mode_current = mode;
}

bvh_result bvh_hit(const vec3 origin, const vec3 direction,
    const uintptr_t bvh, const uintptr_t elements, const uintptr_t vertices,
    const float max_distance)
{
    bvh_result result;
    result.near_distance = max_distance;
    result.hits = false;
    float current_distance = 0;
    
    bvh_attribute* attributes = (bvh_attribute*)bvh;
    bvh_node* root_node = (bvh_node*)((intptr_t)(attributes+1));

    bvh_node* current_node = root_node;
    bool hits_scene = intersect_bounds(origin, direction, 
        current_node->bounds_min.xyz, 
        current_node->bounds_max.xyz, 
        max_distance);

    uint bitstack = 0;
    uint switchstack = 0;

    while(hits_scene)
    {
        while(current_node->type == bvh_node_type_inner)
        {
            bvh_node* left = root_node + current_node->left;
            bvh_node* right = root_node + current_node->right;
            float min_left = 1.f/0.f;
            float min_right = 1.f/0.f;
            bool hits_left = intersect_bounds(origin, direction, left->bounds_min.xyz, left->bounds_max.xyz, 
                result.near_distance, min_left);
            bool hits_right = intersect_bounds(origin, direction, right->bounds_min.xyz, right->bounds_max.xyz, 
                result.near_distance, min_right);

            if(!hits_left && !hits_right)
                break;
            
            bool nrm = min_left < min_right;
            bvh_node* first = nrm ? left : right;
            bvh_node* second = nrm ? right : left;
            bool hits_first = nrm ? hits_left : hits_right;
            bool hits_second = nrm ? hits_right : hits_left;

            switchstack = (switchstack << 1) | int(nrm); // 0x1 -> right next, 0x0 -> left next
            bitstack = (bitstack << 1) | int(hits_left && hits_right);
            current_node = hits_first ? first : second;
        }

        if(current_node->type == bvh_node_type_leaf)
        {
            vec2 current_barycentric;
            uint start = current_node->left;
            uint end   = current_node->right;
            for(uint i = start; i != end+1; ++i)
            {
                uint* idx = ((uint*)(elements + 3*i*attributes->index_stride + attributes->index_offset));
                vec3 tv1 = *((vec3*)(vertices + attributes->position_stride * *idx + attributes->position_offset));
                vec3 tv2 = *((vec3*)(vertices + attributes->position_stride * *(idx+1) + attributes->position_offset));
                vec3 tv3 = *((vec3*)(vertices + attributes->position_stride * *(idx+2) + attributes->position_offset));

                if(intersect_triangle(
                    origin,
                    direction,
                    tv1, tv2, tv3,
                    current_distance,
                    current_barycentric) && current_distance < result.near_distance) 
                {
                    result.hits = true;
                    result.near_distance = current_distance;
                    result.near_barycentric = current_barycentric;
                    result.near_triangle = i;

                    if(_bvh_mode_current == bvh_mode_any)
                        return result;
                }
            }
        }

        while((bitstack & 1) == 0)
        {
            if(bitstack == 0)
                return result;

            current_node = root_node + current_node->parent;
            bitstack = bitstack >> 1;
            switchstack = switchstack >> 1;
        }

        current_node = root_node + 
            (((switchstack & 0x1)==0x1) ? (root_node + current_node->parent)->right : 
            (root_node + current_node->parent)->left);
        bitstack = bitstack ^ 1;
    }
    return result;
}