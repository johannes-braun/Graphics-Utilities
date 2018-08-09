#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;

layout(loc_gl(0) loc_vk(1, 0)) uniform sampler2D tex;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = color * texture(tex, uv);
}