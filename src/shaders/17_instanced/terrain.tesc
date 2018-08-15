#version 460 core
#include "../api.glsl"

layout(vertices = 4) out;
layout(location = 0) flat in int valid[];

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

out gl_PerVertex
{
  vec4 gl_Position;
} gl_out[];

float dot2(vec3 a)
{
	return dot(a, a);
}

const vec2 uvs[] = {
	vec2(0, 0),
	vec2(1, 0),
	vec2(1, 1),
	vec2(0, 1)
};

layout(location = 0) out vec2 uv[];


layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D heightmap;

const float chunk_size = 16.f;
const float chunk_count = 200;

float get_height(vec2 position)
{
	position += vec2(chunk_size * chunk_count) / 2.f;
	position /= chunk_size * chunk_count;
	return 220 * pow(texture(heightmap, position).r, 1);
}

void main()
{
	const int base_resolution = 32;

	if(valid[0] == 1)
	{
		float ydist = abs(camera.pos.y - get_height(camera.pos.xz));
		float scaling = 1-smoothstep(0,300, ydist);
		vec2 dxy = camera.pos.xz - gl_in[gl_InvocationID].gl_Position.xz;
		float falloff = 1000.f / (1+dot(dxy, dxy));

		float sqrdist = distance(camera.pos, gl_in[gl_InvocationID].gl_Position.xyz);
		int subdiv = clamp(int(pow(scaling, 3) * base_resolution * falloff), 0, base_resolution);//int((130.f / sqrdist) * base_resolution);

		gl_TessLevelOuter[gl_InvocationID] = subdiv+2;

		gl_TessLevelInner[0] = subdiv;
		gl_TessLevelInner[1] = subdiv;
	}
	else
	{
		gl_TessLevelOuter[gl_InvocationID] = 2;
		gl_TessLevelInner[0] = 1;
		gl_TessLevelInner[1] = 1;
	}

	uv[gl_InvocationID] = uvs[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}