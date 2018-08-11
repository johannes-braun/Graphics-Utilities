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
	mat4 model;
	vec4 albedo;
};

layout(loc_gl(0) loc_vk(0, 1), std430) restrict readonly buffer Instances
{
	instance instances[];
};

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec3 out_albedo;

void main()
{
	const mat4 model = instances[gl_InstanceID].model;
	out_position = vec3(model * vec4(position, 1));
	out_uv = uv;
	out_normal = vec3(inverse(transpose(model)) * vec4(normal, 0));
	out_albedo = instances[gl_InstanceID].albedo.rgb;
	gl_Position = vec4(camera.proj * camera.view * vec4(out_position, 1));
}