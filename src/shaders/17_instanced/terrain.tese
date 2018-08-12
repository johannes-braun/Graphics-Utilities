#version 460 core

#include "../api.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D heightmap;

layout(quads, equal_spacing) in;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;

const float chunk_size = 150.f;
const float chunk_count = 15;

float get_height(vec2 position)
{
	position += vec2(chunk_size * chunk_count) / 2.f;
	position /= chunk_size * chunk_count;
	return 220 * pow(texture(heightmap, position).r, 1);
}

vec4 get_position(vec2 off)
{
	const vec4 v_00_10 = mix(gl_in[0].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	const vec4 v_01_11 = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	vec4 position = mix(v_00_10, v_01_11, gl_TessCoord.y) + vec4(off.x, 0, off.y, 0);
	position.y = get_height(position.xz);
	return position;
}

void main()
{
	const vec4 p0 = get_position(vec2(0));
	const float eps = 5.f;
	const vec4 dp0 = get_position(vec2(-eps, -eps));
	const vec4 dp1 = get_position(vec2(-eps, eps));
	const vec4 dp2 = get_position(vec2(eps, eps));
	const vec4 dp3 = get_position(vec2(eps, -eps));

	const vec3 normal_0 = cross(dp2.xyz - dp0.xyz, dp3.xyz - dp1.xyz);
	normal = normal_0;
	position = p0.xyz;

	gl_Position = camera.proj * camera.view * p0;
}