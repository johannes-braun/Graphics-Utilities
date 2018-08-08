#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

layout(location=0) in vec3 unn_normal;
layout(location=1) in vec2 uv;

layout(loc_gl(0) loc_vk(0, 0)) uniform MyUBO0 { 
	mat4 view;
	mat4 proj;
} camera;

layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D tex;
layout(loc_gl(1) loc_vk(0, 2)) uniform samplerCube cubemap;

layout(location = 0) out vec4 color;

void main()
{
	vec3 normal = normalize(unn_normal);
	color = vec4(texture(tex, uv).rgb * normal, 1);
	//color = vec4(1, 0, 0, 1) + pos;
}