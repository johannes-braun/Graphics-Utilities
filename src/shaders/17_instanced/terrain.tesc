#version 460 core
#include "../api.glsl"

layout(vertices = 4) out;

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

void main()
{
	const int base_resolution = 128;

	float sqrdist = distance(camera.pos, gl_in[gl_InvocationID].gl_Position.xyz);
	int subdiv = int((130.f / sqrdist) * base_resolution);

	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 128;
		gl_TessLevelOuter[1] = 128;
		gl_TessLevelOuter[2] = 128;
		gl_TessLevelOuter[3] = 128;
	}

	gl_TessLevelInner[0] = subdiv;
	gl_TessLevelInner[1] = subdiv;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}