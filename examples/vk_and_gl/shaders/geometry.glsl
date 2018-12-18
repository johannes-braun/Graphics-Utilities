//? #version 460 core

struct bvh_node_t
{
    vec4 bounds_min;
    vec4 bounds_max;
    uint type;
    uint left;
    uint right;
    uint parent;
};

struct vertex_t
{
	vec3 position;
	uint metadata_position;
	vec3 normal;
	uint metadata_normal;
	vec2 uv;
	uint metadata_uv;
};