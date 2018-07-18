#version 460 core

layout(location = 0) in vec3 uv;

layout(binding = 0) uniform samplerCube cubemap;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(cubemap, uv);
}