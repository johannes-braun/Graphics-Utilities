#version 460 core
#include "../api.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(location = 0) in vec2 position;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(loc_gl(1) loc_vk(1, 2)) uniform TerrainInfo
{
	float           chunk_size;
	int             chunk_count;
};
layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D heightmap;

float get_height(vec2 position)
{
	//position += vec2(chunk_size * chunk_count);
	position /= 1500.f;
	return 80 * pow(texture(heightmap, position).r, 1);
}

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void main()
{
	const vec2 chunk_extents = vec2(chunk_size * chunk_count);
	const vec2 cam_chunk = ivec2(camera.pos.xz / chunk_size) * chunk_size;

	vec2 cp = cam_chunk + position + chunk_size*0.5f;
	
	const vec2 uvx = vec2(cam_chunk.x + position.x, cam_chunk.y + position.y);

	#define get_position(Off) (vec4(uvx.x + (Off.x), get_height(uvx + Off), uvx.y + (Off.y), 0))

	const float eps = 10.f;
	const vec4 dp0 = get_position(vec2(-eps, -eps));
	const vec4 dp1 = get_position(vec2(-eps, eps));
	const vec4 dp2 = get_position(vec2(eps, eps));
	const vec4 dp3 = get_position(vec2(eps, -eps));

	const vec3 normal_0 = cross(dp2.xyz - dp0.xyz, dp3.xyz - dp1.xyz);

	out_position = vec3(uvx.x, get_height(uvx), uvx.y);
	out_uv = uvx;
	out_normal = normal_0;
	gl_Position = camera.proj * camera.view * vec4(out_position, 1);
}