#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

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