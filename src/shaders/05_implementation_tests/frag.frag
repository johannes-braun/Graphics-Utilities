#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

layout(loc_gl(0) loc_vk(0, 0)) uniform MyUBO0 { float floats[5]; } u0;
layout(loc_gl(1) loc_vk(1, 0)) uniform MyUBO1 { float floats[5]; } u1;
layout(loc_gl(2) loc_vk(1, 1)) uniform MyUBO2 { float floats[5]; } u2;
layout(loc_gl(0) loc_vk(1, 2)) buffer MySSBO2 { float floats[5]; } ss0;

layout(loc_gl(0) loc_vk(0, 1)) uniform samplerCube cubemaps[2];
layout(loc_gl(0) loc_vk(0, 3), rgba8) uniform image2D some_image;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 0, 0, 1);
}