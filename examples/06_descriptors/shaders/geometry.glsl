//? #version 460 core

struct bvh_node
{
    vec4 bounds_min;
    vec4 bounds_max;
    uint type;
    uint left;
    uint right;
    uint parent;
};

struct ray_state_t
{
	vec3 origin;
	vec3 direction;
	int bounce_n;
	int sample_n;
	float frequency;
	vec3 intensity_xyz;
	vec3 accum_color;
	float probability;
	vec4 random;
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

struct instance_t
{
    uint index_count;
    uint instance_count;
    uint base_index;
    int  base_vertex;
    uint base_instance;
    uint base_bvh_node;
    uint vertex_count;
    uint bvh_node_count;
	mat4 transform;
	uint color;
	float roughness;
	float reflectivity;
	uint bsdf;
};