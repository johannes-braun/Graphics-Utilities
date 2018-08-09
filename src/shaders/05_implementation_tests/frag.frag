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
layout(location=2) in vec3 unn_view_direction;

layout(loc_gl(0) loc_vk(0, 0)) uniform MyUBO0 { 
	mat4 view;
	mat4 proj;
} camera;

layout(loc_gl(0) loc_vk(0, 1)) uniform samplerCube cubemap;
layout(loc_gl(1) loc_vk(1, 0)) uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
	vec3 normal = normalize(unn_normal);
	vec3 view_direction = normalize(unn_view_direction);

	vec3 reflected = reflect(view_direction, normal);

	float vdotn = dot(-view_direction, normal);
	float f0 = (1.56f - 1.f)/(1.56f + 1.f);
	float fresnel = f0 + (1-f0) * pow(vdotn, 5.f);

	color = mix(vec4(texture(tex, uv).rgb * normal, 1), vec4(texture(cubemap, reflected).rgb, 1), 1-fresnel);
}