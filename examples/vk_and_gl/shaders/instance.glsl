//? #version 460 core
#include "def.hpp"

struct basic_instance 
{
	uint index_count;
	uint instance_count;
	uint base_index;
	int base_vertex;
	uint base_instance;
	uint base_bvh_node;
	uint vertex_count;
	uint bvh_node_count;
	mat4 transform;
	mesh_info info;
};

vec4 unpack_rgba8(uint c)
{
	return vec4(
		((c >> 24) & 0xff) / 255.f,
		((c >> 16) & 0xff) / 255.f,
		((c >> 8) & 0xff) / 255.f,
		((c >> 0) & 0xff) / 255.f
	);
}

vec4 get_color(const in basic_instance instance)
{
	return unpack_rgba8(instance.info.color);
};