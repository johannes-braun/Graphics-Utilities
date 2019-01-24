#version 460 core

layout(location = 0) in float life;

layout(location = 0) out vec4 color;
void main()
{
	const vec3 start = vec3(6, 5, 5);
	const vec3 end = vec3(0.2, 0, 0);

	color = vec4(mix(start, end, 1-life), 1);
}