#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(loc_gl(0) loc_vk(0, 0)) uniform MyUBO0 { 
	mat4 view;
	mat4 proj;
} camera;

layout(location=0) out vec3 out_normal;
layout(location=1) out vec2 out_uv;

void main()
{
	gl_Position = camera.proj * camera.view * vec4(position, 1);
	out_normal = normal;
	out_uv = uv;
}