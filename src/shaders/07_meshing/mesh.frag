//! #version 450 core

layout(location = 0) in vec3 normal;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(normal, 1);
}