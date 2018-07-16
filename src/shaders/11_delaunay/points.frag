#version 460 core

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(in_color, 1);
}