#version 460 core
#include "../api.glsl"

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

const float z = 0.0f;
layout(location = 0) out vec3 uv;

void main()
{
	gl_Position = vec4(mix(-1, 3, gl_VertexID & 0x1), mix(-1, 3, (gl_VertexID >> 1) & 0x1), z, 1.f);
	uv = (mat4(mat3(inverse(camera.view))) * inverse(camera.proj) * gl_Position).xyz;
}