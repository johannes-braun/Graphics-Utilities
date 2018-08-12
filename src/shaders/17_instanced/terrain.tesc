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
	float sqrdist = distance(camera.pos, gl_in[gl_InvocationID].gl_Position.xyz);
	int subdiv = int((100.f / sqrdist) * 128);

	gl_TessLevelOuter[0] = subdiv;
	gl_TessLevelOuter[1] = subdiv;
	gl_TessLevelOuter[2] = subdiv;
	gl_TessLevelOuter[3] = subdiv;
	gl_TessLevelInner[0] = subdiv;
	gl_TessLevelInner[1] = subdiv;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}