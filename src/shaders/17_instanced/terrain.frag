#version 460 core
#include "../api.glsl"
#include "sky.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_u;

layout(location = 0) out vec4 color;

void main()
{
	const vec3 normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const vec3 light  = normalize(vec3(4, 4, 4));
	const float ndotl = max(dot(light, normal), 0.f);
	const float ndotv = max(dot(-view, normal), 0.f);

	const float px = mod(position.x+ 50.f, 100.0f);
	const float line_x_p = smoothstep(145.f, 150.f, px);
	const float line_x_n = smoothstep(5.f, 0.f, px);
	const float line_x = line_x_p + line_x_n;

	const float py = mod(position.z+ 50.f, 100.0f);
	const float line_y_p = smoothstep(145.f, 150.f, py);
	const float line_y_n = smoothstep(5.f, 0.f, py);
	const float line_y = line_y_p + line_y_n;

	const float f0 = pow((1+1.5f) / (1-1.5f), 2);
	const float fres = f0 + (1-f0)* (1-pow(ndotv, 5));

	color = vec4((1-line_x - line_y) * ndotl * vec3(0.5f, 0.2f, 0.1f) + sky_noclouds(view, normal), 1);
	color = mix(color, vec4(sky(view, camera.pos), 1), smoothstep(1500.f, 2500.f, distance(position, camera.pos)));
}
