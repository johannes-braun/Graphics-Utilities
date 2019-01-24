#version 460 core
#include "particle.h"
#include "geometry.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(
#ifdef VULKAN
set = 1,
#endif
binding = 1) restrict buffer Particles
{
	particle_t particles[];
};

layout(set = 0, binding = 0) uniform Camera
{
	camera_t camera;
};

#ifdef VULKAN
#define gl_InstanceID gl_InstanceIndex
#endif

layout(location = 0) out float life;
void main()
{
	const vec3 out_position = particles[gl_InstanceID].position + 0.3*position;
	gl_Position = camera.proj * camera.view * vec4(out_position, 1);
	life = particles[gl_InstanceID].life;
}