#pragma once

#include "intersect_impl.inl"

struct grid_ls
{
    vec4 bounds_min;
    vec4 bounds_max;
    ivec4 xyz_p;
    uintptr_t line_spaces;
};

struct buffer_data
{
    vec4 bounds_min;
    vec4 bounds_max;
    ivec4 xyz_p;
    uintptr_t storages[6][6];
};

const ivec2 patch_indices[6] = {
    ivec2( 0, 1 ),
    ivec2( 0, 2 ),
    ivec2( 1, 2 ),
    
    ivec2( 0, 1 ),
    ivec2( 0, 2 ),
    ivec2( 1, 2 ),
};

int smallest_axis(const in vec3 vector)
{
     if(vector.x < vector.y)
     {
        if(vector.x < vector.z)
            return 0;
        else
            return 2;
    }
    else if(vector.y < vector.z)
        return 1;
    else
        return 2;
}

line get_line_grid(uintptr_t grid_line_space, vec3 origin, vec3 direction, float max_distance)
{
    grid_ls* grid = (grid_ls*)grid_line_space;

    int start_face, end_face;
    float temp_tmin, temp_tmax;
    line result;
    result.triangle = -1;
    if(intersect_bounds(origin - 1e3f * direction, direction, grid->bounds_min.xyz, grid->bounds_max.xyz, max_distance, temp_tmin, temp_tmax, start_face, end_face))
    {
        vec3 voxel_size = (grid->bounds_max.xyz - grid->bounds_min.xyz).xyz / grid->xyz_p.xyz;

        origin = origin + int(temp_tmin == temp_tmax) * temp_tmin * direction;

        vec3 step = sign(direction);
        vec3 delta = step / direction * voxel_size;
        vec3 position = (origin - grid->bounds_min.xyz) / voxel_size;
        ivec3 index = clamp(ivec3(position), ivec3(0), grid->xyz_p.xyz-1);
        ivec3 totalStepSign = ivec3(greaterThan(step, ivec3(0)));

        vec3 tnext = (totalStepSign + (1-2*totalStepSign) * (position - index)) * delta;
        float temp_t_min = 0.f;

        for(int i=0; i< 2 * max(grid->xyz_p.x, max(grid->xyz_p.y, grid->xyz_p.z));++i)
        {
            int ls_index = index.x + index.y * grid->xyz_p.x + index.z * grid->xyz_p.x * grid->xyz_p.y;
            int axis = smallest_axis(tnext);
            
            line l = get_line(grid->line_spaces, ls_index, origin, direction, max_distance);
            if(l.triangle != -1)
                return l;

            temp_t_min = tnext[axis]; 
            index[axis] += int( step[axis] ); 
            if ((index[axis] < 0) || (index[axis] >=  grid->xyz_p[axis]))
            {
                break;
            }
            tnext[axis] += delta[axis];
        }
    }

    return result;
}

line get_line(uintptr_t line_space, int index, vec3 origin, vec3 direction, float max_distance)
{
    buffer_data* line_space_data = ((buffer_data*)line_space) + index;

    float min_t = 1.f / 0.f;
    float max_t = - (1.f / 0.f);
    bool bound_intersection = intersect_bounds(origin, direction, line_space_data->bounds_min.xyz, line_space_data->bounds_max.xyz, max_distance);

    line result;
    result.triangle = -1;
    if (bound_intersection)
    {
        const vec3 bounds_size = line_space_data->bounds_max.xyz - line_space_data->bounds_min.xyz;

        int start_face = -1;
        int end_face = -1;
        intersect_bounds(origin - 1e3f * direction, direction, line_space_data->bounds_min.xyz, line_space_data->bounds_max.xyz, max_distance, min_t, max_t, start_face, end_face);
        
        start_face = clamp(start_face, 0, 5);
        end_face = clamp(end_face, 0, 5);

        if (start_face == end_face)
            return result;

        const vec3 on_bounds_start = origin + (min_t - 1e3f) * direction;
        const vec3 on_bounds_end = origin + (max_t - 1e3f) * direction;

        const vec3 start_diff = on_bounds_start - line_space_data->bounds_min.xyz;
        const vec3 end_diff = on_bounds_end - line_space_data->bounds_min.xyz;

        vec2 start_offset;
        start_offset.x = start_diff[patch_indices[start_face].x];
        start_offset.y = start_diff[patch_indices[start_face].y];

        vec2 end_offset;
        end_offset.x = end_diff[patch_indices[end_face].x];
        end_offset.y = end_diff[patch_indices[end_face].y];

        ivec2 start_size;
        start_size.x = line_space_data->xyz_p[patch_indices[start_face].x];
        start_size.y = line_space_data->xyz_p[patch_indices[start_face].y];

        ivec2 end_size;
        end_size.x = line_space_data->xyz_p[patch_indices[end_face].x];
        end_size.y = line_space_data->xyz_p[patch_indices[end_face].y];

        vec2 start_bounds_size;
        start_bounds_size.x = bounds_size[patch_indices[start_face].x];
        start_bounds_size.y = bounds_size[patch_indices[start_face].y];

        vec2 end_bounds_size;
        end_bounds_size.x = bounds_size[patch_indices[end_face].x];
        end_bounds_size.y = bounds_size[patch_indices[end_face].y];

        ivec2 start_patch = clamp(ivec2((start_offset / start_bounds_size) * start_size), ivec2(0), start_size - ivec2(1));
        ivec2 end_patch = clamp(ivec2((end_offset / end_bounds_size) * end_size), ivec2(0), end_size - ivec2(1));

        line* line_set = (line*)line_space_data->storages[start_face][end_face];

        const int start_count = start_size.x * start_size.y;
        const int end_count = end_size.x * end_size.y;
        const uint index = uint(end_count * (start_patch.y * start_size.x + start_patch.x) + end_patch.y * end_size.x  + end_patch.x);

        return line_set[index];
    }
    return result;
}