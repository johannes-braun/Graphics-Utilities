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