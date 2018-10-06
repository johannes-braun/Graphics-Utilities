#version 460 core
#include "../api.glsl"
#include "terrain.input.glsl"

layout(location = 0) in vec2 position;

out gl_PerVertex
{
	vec4 gl_Position;
};

float get_height(vec2 position)
{
	position /= 1500.f;
	return 80 * pow(texture(heightmap, position).r, 1);
}

void main()
{
	const vec2 chunk_extents = vec2(chunk_size * chunk_count);
	const vec2 cam_chunk = ivec2(camera.pos.xz / chunk_size) * chunk_size;

	vec2 cp = cam_chunk + position + chunk_size * 0.5f;
	
	const vec2 uvx = vec2(cam_chunk.x + position.x, cam_chunk.y + position.y);

	gl_Position = camera.proj * camera.view * vec4(uvx.x, get_height(uvx), uvx.y, 1);
}