#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

layout(location=0) in vec2 uv;

layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D tex;

layout(loc_gl(0) loc_vk(0, 0)) uniform render_info
{
    mat4 view_projection;
    mat4 hat_mat;
    vec3 average;
};

layout(location=0) out vec4 color;

void main()
{
    color = clamp(hat_mat * texture(tex, vec2(uv.x, 1-uv.y)), vec4(0), vec4(1));
}