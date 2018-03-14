#extension GL_NV_command_list : require

#include "ext/bvh.h"

// Inputs
layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 normal_world;

// Outputs
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 out_normal_depth;

// Uniforms
layout(binding = 0, std140) uniform Data
{
    sampler2D image_texture;
    vec3 light_position;
    vec3 light_color;
    vec3 background;
    uintptr_t indices;
    uintptr_t vertices;
    uintptr_t bvh_buffer;
};

void main()
{
    const vec3 to_light = light_position - position.xyz;
    const float dist_squared = dot(to_light, to_light);
    vec3 light_tint = light_color * max(dot(normalize(to_light), normalize(normal_world.xyz)), 0.f) / (1+dist_squared);

    bvh_result res = bvh_hit(
        // Ray in local space 
        // (already transformed relative to the Mesh transformation)
        position.xyz+1e-3f*normal_world.xyz, normalize(to_light),
        // Data (as bindless buffer pointers). The data has to be compatible 
        // with the given bvh buffer layout and the assigned attribute data.
        bvh_buffer, indices, vertices, 
        // Search distance on ray
        sqrt(dist_squared));

    if (res.hits)
        light_tint = vec3(0);

    color = vec4(light_tint + background, 1) * texture(image_texture, uv);
    out_normal_depth = vec4(normal.xyz, (1-gl_FragCoord.z)/gl_FragCoord.w);
}
layout(commandBindableNV) uniform;