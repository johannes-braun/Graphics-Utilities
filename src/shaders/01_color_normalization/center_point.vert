#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
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
    vec3 real_center = clamp(hat_mat * vec4(average, 1), 0, 1).xyz;
    gl_Position = view_projection * vec4(scale * real_center, 1);
    gl_PointSize = 8.f;
}