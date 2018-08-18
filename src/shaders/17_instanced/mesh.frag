#version 460 core
#include "../api.glsl"
#include "sky.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal_u;
layout(location = 3) in vec3 albedo;

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(location = 0) out vec4 color;
const float chunk_size = 16.f;
const float chunk_count = 200;

void main()
{
	const vec3 normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const vec3 light  = normalize(vec3(4, 4, 4));

	const float ndotl = max(dot(light, normal), 0.f);

	color = vec4(ndotl * albedo/* + sky_noclouds(view, normal)*/, 1);
	
	//const float max_dist = chunk_size * chunk_count;
	//color = mix(color, vec4(sky_noclouds(view, camera.pos), 1), smoothstep(max_dist / 4.f, max_dist/2.f, distance(position, camera.pos)));
}