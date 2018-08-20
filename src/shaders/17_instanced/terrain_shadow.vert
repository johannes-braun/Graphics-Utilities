#version 460 core
#include "../api.glsl"

layout(location = 0) in vec2 position;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;
layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D heightmap;

layout(loc_gl(1) loc_vk(1, 2)) uniform TerrainInfo
{
	float           chunk_size;
	int             chunk_count;
};

float get_height(vec2 position)
{
	position /=1500.f;
	return 80 * pow(texture(heightmap, position).r, 1);
}

void main()
{
	const vec2 chunk_extents = vec2(chunk_size * chunk_count);
	const vec2 cam_chunk = ivec2(camera.pos.xz / chunk_size) * chunk_size;

	vec2 cp = cam_chunk + position + chunk_size*0.5f;
	
	const vec2 uvx = vec2(cam_chunk.x + position.x, cam_chunk.y + position.y);

	gl_Position = camera.proj * camera.view * vec4(uvx.x, get_height(uvx), uvx.y, 1);
}