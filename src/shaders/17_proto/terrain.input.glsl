//! #version 460 core
//! #include "../api.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform sampler2D heightmap;
layout(loc_gl(0) loc_vk(0, 1)) uniform TerrainInfo
{
	float           chunk_size;
	int             chunk_count;
};
layout(loc_gl(1) loc_vk(1, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;