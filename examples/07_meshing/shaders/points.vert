#version 460 core
#include <gfx.shaders/api.glsl> //! #include "../../../src/gfx.shaders/include/gfx.shaders/api.glsl"

layout(location = 0) in vec3 position;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
};

layout(loc_gl(0) loc_vk(0, 2), std140) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

void main()
{
	gl_Position = camera.proj * camera.view * vec4(position, 1) + vec4(0, 0, 0.001f, 0);
    gl_PointSize = 5.0f;
}