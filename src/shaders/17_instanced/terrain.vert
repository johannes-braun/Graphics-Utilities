#version 460 core
#include "../api.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 chunk_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

const float chunk_size = 100.f;
const float chunk_count = 50;

void main()
{
	const vec2 chunk_extents = vec2(chunk_size * chunk_count);
	const vec2 cam_chunk = ivec2(camera.pos.xz / chunk_size) * chunk_size;

	gl_Position = vec4(cam_chunk.x + chunk_position.x + position.x, 0, cam_chunk.y + chunk_position.y + position.y, 1);
}