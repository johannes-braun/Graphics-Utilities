#version 460 core

layout(location = 0) in vec3 position;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
};

layout(binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

layout(location = 0) out vec3 color;

void main()
{
	gl_Position = camera.proj * camera.view * vec4(position, 1);
	gl_PointSize = 5.f;
	color = vec3(0.05f * gl_VertexID);
}