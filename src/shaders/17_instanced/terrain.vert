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

const float chunk_size = 16.f;
const float chunk_count = 200;
layout(location = 0) flat out int valid;
layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D heightmap;

float get_height(vec2 position)
{
	position += vec2(chunk_size * chunk_count) / 2.f;
	position /= chunk_size * chunk_count;
	return 220 * pow(texture(heightmap, position).r, 1);
}

void main()
{
	const vec2 chunk_extents = vec2(chunk_size * chunk_count);
	const vec2 cam_chunk = ivec2(camera.pos.xz / chunk_size) * chunk_size;

	vec2 cp = cam_chunk + chunk_position + chunk_size*0.5f;
	vec4 sscp = camera.proj * camera.view * vec4(cp.x, get_height(cp), cp.y, 1);

	valid = 1;//int(!(any(greaterThan(sscp.xy / sscp.ww, vec2(1.5f))) || any(lessThan(sscp.xy / sscp.ww, -vec2(1.5f)))));

	gl_Position = vec4(cam_chunk.x + chunk_position.x + position.x, 0, cam_chunk.y + chunk_position.y + position.y, 1);
}