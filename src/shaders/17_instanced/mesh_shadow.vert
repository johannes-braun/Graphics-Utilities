#version 460 core
#include "../api.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

struct instance
{
	uint index_count;
	uint instance_count;
	uint base_index;
	uint base_vertex;
	uint base_instance;
	uint did;

	mat4 model;
	vec4 albedo;
	vec4 bmin;
	vec4 bmax;
};

layout(loc_gl(0) loc_vk(1, 0), std430) restrict readonly buffer Instances
{
	instance instances[];
};

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	const mat4 model = instances[gl_DrawID].model;
	gl_Position = vec4(camera.proj * camera.view * model * vec4(position, 1));
}