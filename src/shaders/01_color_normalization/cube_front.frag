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

layout(location = 0) out vec4 color;

void main()
{
    color = texture(tex, uv);
    color.a = (1-color.r) * 0.9f;
    color.rgb = vec3(0.85f);
}