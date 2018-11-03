#version 460 core
#include <gfx.shaders/api.glsl> //! #include "../../../src/gfx.shaders/include/gfx.shaders/api.glsl"

layout(location=0) flat out int axis;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(loc_gl(0) loc_vk(0, 0)) uniform render_info
{
    mat4 view_projection;
    mat4 hat_mat;
    vec3 average;
};
const float scale = 4.f;

void main()
{
    axis = gl_VertexID / 2;
    int fac = gl_VertexID % 2;
    vec4 position = vec4(0, 0, 0, 1);
    position[axis] = scale*fac;
    gl_Position = view_projection * position;
}