//! #version 450 core

layout(location = 0) in vec3 position;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
};

layout(binding = 0, std140) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

void main()
{
	gl_Position = camera.proj * camera.view * vec4(position, 1);
    gl_PointSize = 5.0f;
}