#version 460 core
//! #extension GL_KHR_vulkan_glsl : enable

layout(location=0) in vec2 uv;

layout(std140, set=0, binding=0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 position;
    int  do_cull;
} camera;

struct bvh_node
{
    vec4 bounds_min;
    vec4 bounds_max;
    uint type;
    uint left;
    uint right;
    uint parent;
};
struct vertex
{
	vec3 position;
	uint metadata_position;
	vec3 normal;
	uint metadata_normal;
	vec2 uv;
	uint metadata_uv;
};
struct instance
{
	uint base_index;
	uint base_vertex;
	uint base_bvh_node;
	uint color;
	mat4 transform;
	float roughness;
	float reflectivity;
};

layout(set = 1, binding = 0) restrict readonly buffer ModelBVH
{
    bvh_node nodes[];
} model_bvh;
layout(set = 1, binding = 1) restrict readonly buffer ModelVertices
{
    vertex model_vertices[];
};
layout(set = 1, binding = 2) restrict readonly buffer ModelIndices
{
    uint model_indices[];
};
layout(set = 1, binding = 3) restrict readonly buffer ModelInstances
{
    instance model_instances[];
};

struct glb
{
	ivec2 viewport;
	float random;
	int rendered_count;
};
layout(std430, set=2, binding=0) restrict readonly buffer Globals
{
	glb globals;
};

layout(set = 2, binding = 1) uniform sampler1D cie_spectrum;
layout(set = 2, binding = 2) uniform sampler2D accumulation_image;

////////////////////////////////////////////////////////////////////////////////////
////
////		Attachment Outputs
////
////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 accumulation_output;

#include "bvh.glsl"
#include "random.glsl"

const mat3 cie_xyz_to_rgb = mat3(2.3706743, -0.9000405, -0.4706338,
								-0.5138850,  1.4253036,  0.0885814,
								 0.0052982, -0.0146949,  1.0093968);

void main()
{
	ivec2 pixel    = ivec2(gl_FragCoord.xy);
    ivec2 img_size = ivec2(globals.viewport);
    if (any(greaterThanEqual(pixel, img_size))) return;

    init_random(ivec2(0, 0) + pixel, int(3000 * globals.random));
    vec2 random_value =
        random_hammersley_2d(int(next_random() * img_size.x * img_size.y) % (1212121),
                             1.f / (1212121));

	float freq = next_random();
	vec3 rgb_color = 4 * transpose(cie_xyz_to_rgb) * texture(cie_spectrum, freq).xyz;
	vec4 old_rgb = texelFetch(accumulation_image, pixel, 0);

	vec2 uvx = vec2(uv + ((random_value - 0.5f) / vec2(img_size)));

	const mat4 inv_vp = inverse(camera.projection * mat4(mat3(camera.view)));
	const vec3 direction = normalize(vec3(inv_vp * vec4(uvx * 2 - 1, 0.f, 1.f)));
	const vec3 origin = camera.position;
	
    bvh_result hit = bvh_hit_instanced(origin.xyz, direction.xyz, 1.f / 0.f);
    if (hit.hits)
    {
		instance inst = model_instances[hit.instance];
		uint v0  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 1];
		uint v1  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 2];
		uint v2  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 0];
		vertex hit_vert;
		hit_vert.position = (inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].position + hit.near_barycentric.y * model_vertices[v1].position
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].position, 1)).xyz;
		hit_vert.normal = normalize((inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].normal + hit.near_barycentric.y * model_vertices[v1].normal
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].normal, 0)).xyz);
		hit_vert.uv = hit.near_barycentric.x * model_vertices[v0].uv + hit.near_barycentric.y * model_vertices[v1].uv
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].uv;
		hit_vert.normal = faceforward(hit_vert.normal, direction.xyz, hit_vert.normal);

		old_rgb += vec4(rgb_color * vec3(1, 0, 0), 1);
	}
	else
	{
		old_rgb += vec4(rgb_color, 1);
	}
	
	color = old_rgb / globals.rendered_count;
	accumulation_output = old_rgb;
}
