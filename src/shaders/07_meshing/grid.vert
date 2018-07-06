//! #version 450 core

layout(location = 0) in vec3 position;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(binding = 0) uniform CamData
{
	mat4 view;
	mat4 proj;
};

layout(location = 0) out vec2 uv;

void main()
{
	uv = 10*position.xy;
	gl_Position = proj * view * vec4(10*position, 1);
}