//! #version 450 core

layout(location = 0) in vec3 normal;

layout(location = 0) out vec4 color;

void main()
{
	float d = dot(normal, normalize(vec3(1)));
	float m = (int(d*100)/20)/5.f;

	color = mix(0.8f*vec4(1), vec4(1), m);
}