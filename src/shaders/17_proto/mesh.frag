#version 460 core
#include "../api.glsl"
#include "sky.glsl"
#include "shadow.glsl"

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

layout(loc_gl(3) loc_vk(2, 0)) uniform sampler2D overlay_texture;

layout(location = 0) out vec4 color;
const float chunk_size = 2.f;
const float chunk_count = 400;

void main()
{
	const vec3 normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const vec3 light  = normalize(position-shadow_camera.data[0].pos);

	const float ndotl = max(dot(light, normal), 0.f);

	color = vec4(ndotl * albedo * texture(overlay_texture, uv).rgb, 1);

	const float cam_distxz = distance(position.xzy, camera.pos.xzy);
	float s = shadow(shadowmap, position, cam_distxz, normal, light);

	const float max_dist = chunk_size * chunk_count;
	color = mix(s * color + vec4(albedo * vec3(0.3f, 0.5f, 1.f) / 3.14159265359f, 0), vec4(sky_noclouds(view, camera.pos), 1), smoothstep(max_dist / 2.5f, max_dist/2.f, distance(position, camera.pos)));
}