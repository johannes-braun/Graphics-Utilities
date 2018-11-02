#version 460 core
//! #extension GL_KHR_vulkan_glsl : enable
const float PI = 3.14159265359;

layout(location=0) in vec2 uv;

layout(std140, set=0, binding=0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 position;
    int  do_cull;
} camera;

const float focal_length = 6.f;
const float aperture = 0.32f;
const float sensor_response = 1.f; // may be received from texture
const float exposure = 1.f;
const float gamma = 1.8f;

const int output_default = 0;
const int output_norm_samples = 1;

layout(set=0, binding=1) uniform sampler2D bokeh_shape;

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

const uint bsdf_opaque = 0;
const uint bsdf_transparent = 1;
const uint bsdf_emissive = 2;
struct instance
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

layout(std430, set=2, binding=0) restrict readonly buffer Globals
{
	ivec2 viewport;
	float random;
	int rendered_count;
	int render_output;
} globals;

layout(set = 2, binding = 1) uniform sampler1D cie_spectrum;
layout(set = 2, binding = 2) uniform samplerCube environment_map;
layout(set = 2, binding = 3) uniform sampler2D accumulation_image;
layout(set = 2, binding = 4) uniform sampler2D bounce_image;
layout(set = 2, binding = 5) uniform sampler2D positions_bounce_image;
layout(set = 2, binding = 6) uniform sampler2D directions_sample_image;

////////////////////////////////////////////////////////////////////////////////////
////
////		Attachment Outputs
////
////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 accumulation_output;
layout(location = 2) out vec4 bounce_accumulation_output;
layout(location = 3) out vec4 positions_bounce_output;
layout(location = 4) out vec4 directions_sample_output;

#include "bvh.glsl"
#include "random.glsl"
#include "sample.glsl"

const mat3 cie_xyz_to_rgb = mat3(2.3706743, -0.9000405, -0.4706338,
								-0.5138850,  1.4253036,  0.0885814,
								 0.0052982, -0.0146949,  1.0093968);
const mat3 cie_rgb_to_xyz = mat3(0.4887180,  0.3106803,  0.2006017,
								 0.1762044,  0.8129847,  0.0108109,
								 0.0000000,  0.0102048,  0.9897952);

struct ray_state_t
{
	vec3 origin;
	vec3 direction;
	int bounce_n;
	int sample_n;
	float frequency;
	float intensity;
	vec4 accum_color;
} ray_state;

vec3 randDisk(float u, float v, vec3 normal, float radius, out float x, out float y)
{
    // Sample a point on a unit disk
    float r = sqrt(u);
    float theta = 2 * PI * v;
    x = r * cos(theta);
    y = r * sin (theta);

	// multiply radius
    vec3 dir = vec3( radius * x, radius * y, 0 );

    // Create an orthonormal basis around the surface normal
   vec3 s = abs(normal.x) > abs(normal.y) ?
            vec3(-normal.z, 0, normal.x) / sqrt(normal.x * normal.x + normal.z * normal.z):
            vec3(0, normal.z, -normal.y) / sqrt(normal.y * normal.y + normal.z * normal.z);
    vec3 t = cross(normal, s);

    // Transform into local shading coordinate system
    return dir.x * s + dir.y * t;
}

float from_nanometers(float nm)
{
	return (nm - 380) / (860 - 380);
}

float gauss(float sigma, float e, float x)
{
	return 1/(sigma * sqrt(2 * PI)) * exp(-0.5f * pow((x - e) / sigma, 2));
}

void load_state(vec2 random_value)
{
	ivec2 pixel    = ivec2(gl_FragCoord.xy);
	vec4 packed_positions_bounces = texelFetch(positions_bounce_image, pixel, 0);
	vec4 packed_directions_samples = texelFetch(directions_sample_image, pixel, 0);
	vec4 packed_bounce_accum = texelFetch(bounce_image, pixel, 0);

	if(globals.rendered_count == 1)
	{
		ray_state.bounce_n = 0;
		ray_state.sample_n = 0;
		ray_state.accum_color = vec4(0);
	}
	else
	{
		ray_state.bounce_n = floatBitsToInt(packed_positions_bounces.w);
		ray_state.sample_n = floatBitsToInt(packed_directions_samples.w);
		ray_state.accum_color = texelFetch(accumulation_image, pixel, 0);
	}

	if(ray_state.bounce_n == 0)
	{
		const mat4 inv_vp = inverse(camera.projection * mat4(mat3(camera.view)));
		ivec2 img_size = ivec2(globals.viewport);
		vec2 uvx = vec2(uv + ((random_value - 0.5f) / vec2(img_size)));
		ray_state.direction = vec3(inv_vp * vec4(uvx * 2 - 1, 0.f, 1.f));
		ray_state.origin = camera.position;

		// Apply DOF
		vec2 random_value_dof =
			random_hammersley_2d(int(next_random() * img_size.x * img_size.y) % (1212121),
								 1.f / (1212121));

		const vec3 focal_point = ray_state.origin + focal_length * ray_state.direction;
		vec2 offset;
		const vec3 disk_sampled_point = randDisk(random_value_dof.x, random_value_dof.y, vec3(inv_vp * vec4(0, 0, 0.f, 1.f)), 1.f, offset.x, offset.y);

		ray_state.origin = ray_state.origin + aperture * disk_sampled_point;
		ray_state.direction = focal_point - ray_state.origin;

		const float bokeh_intensity = texture(bokeh_shape, vec2(0.5f) + offset).r;

		ray_state.direction = normalize(ray_state.direction);
		ray_state.frequency = from_nanometers(400 + 360 * next_random());
		ray_state.intensity = bokeh_intensity * sensor_response;
	}
	else
	{
		ray_state.direction = packed_directions_samples.xyz;
		ray_state.origin = packed_positions_bounces.xyz;
		ray_state.frequency = packed_bounce_accum.r;
		ray_state.intensity = packed_bounce_accum.g;
	}
}

void store_state()
{
	accumulation_output = ray_state.accum_color;
	bounce_accumulation_output = vec4(ray_state.frequency, ray_state.intensity, 0, 0);
	positions_bounce_output = vec4(ray_state.origin, intBitsToFloat(ray_state.bounce_n));
	directions_sample_output = vec4(ray_state.direction, intBitsToFloat(ray_state.sample_n));
}

vec3 freq_to_xyz(float freq)
{
	return texture(cie_spectrum, freq).xyz;
}

float ior(float freq)
{
	float x = freq;
	float i = pow((1/(x+0.2) + pow((x+0.2), 2)) / 2 + 0.5, 0.4) + 0.5f;
	return i;
}

void emit_ray(vec3 origin, vec3 direction, float intensity)
{
	ray_state.direction = direction;
	ray_state.origin = origin + 1e-4f * ray_state.direction;
	ray_state.bounce_n += 1;
	ray_state.intensity *= intensity;
}

void end_ray()
{
	ray_state.bounce_n = 1000;
}

void main()
{
	ivec2 pixel    = ivec2(gl_FragCoord.xy);
    ivec2 img_size = ivec2(globals.viewport);
    if (any(greaterThanEqual(pixel, img_size))) return;

    init_random(ivec2(0, 0) + pixel, int(3000 * globals.random));
    vec2 random_value =
        random_hammersley_2d(int(next_random() * img_size.x * img_size.y) % (1212121),
                             1.f / (1212121));

	load_state(random_value);
	
    bvh_result hit = bvh_hit_instanced(ray_state.origin.xyz, ray_state.direction.xyz, 1.f / 0.f);
    if (hit.hits)
    {
		instance inst = model_instances[hit.instance];
		uint v0  = uint(inst.base_vertex) + model_indices[inst.base_index + hit.near_triangle * 3 + 1];
		uint v1  = uint(inst.base_vertex) + model_indices[inst.base_index + hit.near_triangle * 3 + 2];
		uint v2  = uint(inst.base_vertex) + model_indices[inst.base_index + hit.near_triangle * 3 + 0];
		vertex hit_vert;
		hit_vert.position = (inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].position + hit.near_barycentric.y * model_vertices[v1].position
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].position, 1)).xyz;
		hit_vert.normal = normalize((inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].normal + hit.near_barycentric.y * model_vertices[v1].normal
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].normal, 0)).xyz);
		hit_vert.uv = hit.near_barycentric.x * model_vertices[v0].uv + hit.near_barycentric.y * model_vertices[v1].uv
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].uv;
		bool incoming = dot(hit_vert.normal, ray_state.direction.xyz) < 0;
		hit_vert.normal = faceforward(hit_vert.normal, ray_state.direction.xyz, hit_vert.normal);
		
		vec3 new_direction;
		
		if(inst.bsdf == bsdf_emissive)
		{
			vec3 xyz = transpose(cie_rgb_to_xyz) * unpackUnorm4x8(inst.color).rgb;
			float ava = dot(transpose(cie_xyz_to_rgb) * freq_to_xyz(ray_state.frequency), transpose(cie_xyz_to_rgb) * xyz);
			ray_state.accum_color += vec4(inst.reflectivity * ava * freq_to_xyz(ray_state.frequency), 0);
			ray_state.sample_n += 1;
			ray_state.bounce_n = 0;
		}
		else
		{
			float roughness = inst.roughness;
			float alpha2    = roughness * roughness;
			float ior_f = ior(ray_state.frequency);

			float ior_in = incoming ? 1.f : ior_f;
			float ior_out = incoming ? ior_f : 1.f;
			float F0 = pow((ior_in-ior_out)/(ior_in+ior_out), 2);
			vec3 fresnel = F0 + (max(vec3(1.0 - inst.reflectivity) * F0, F0) - F0) * pow(1.0 - max(dot(-ray_state.direction, hit_vert.normal), 0), 5.0);
			
			vec3 msnormal = normalize(local_to_world(ggx_importance_hemisphere(ggx_importance_sample(random_value, alpha2)), hit_vert.normal));
			if((fresnel.x + fresnel.y + fresnel.z)/3.f <= next_random())
			{
				if(inst.bsdf == bsdf_transparent)
					new_direction     = refract(ray_state.direction, msnormal, ior_in / ior_out);
				else
				{
					new_direction = normalize(local_to_world(sample_cosine_hemisphere(random_value), hit_vert.normal));
					vec3 rgb = unpackUnorm4x8(inst.color).rgb;
					vec3 xyz = normalize(transpose(cie_rgb_to_xyz) * rgb);

					float d = dot(xyz, freq_to_xyz(ray_state.frequency));
					ray_state.intensity *= d;
				}
			}
			else
			{
				new_direction     = reflect(ray_state.direction, msnormal);
			}

			new_direction = normalize(new_direction);
			ray_state.bounce_n += 1;
		}

		ray_state.direction = new_direction;
		ray_state.origin = hit_vert.position + 1e-4f * ray_state.direction;

		if(ray_state.bounce_n > 12 || ray_state.intensity < 0.01f)
		{
			vec3 env = clamp(texture(environment_map, ray_state.direction).rgb, 0, 1000.f);
			ray_state.accum_color += ray_state.intensity * vec4(freq_to_xyz(ray_state.frequency) * (transpose(cie_rgb_to_xyz) * env), 1);
			ray_state.sample_n += 1;
			ray_state.bounce_n = 0;
		}
	}
	else
	{
		vec3 env = clamp(texture(environment_map, ray_state.direction).rgb, 0, 1000.f);
		ray_state.accum_color += ray_state.intensity * vec4(freq_to_xyz(ray_state.frequency) * (transpose(cie_rgb_to_xyz) * env), 1);
	
		ray_state.sample_n += 1;
		ray_state.bounce_n = 0;	
	}
	
	switch(globals.render_output)
	{
	case output_default:
		color = vec4(transpose(cie_xyz_to_rgb) * (ray_state.accum_color / max(ray_state.sample_n + 1, 1)).rgb, 1);
		color = 1.f - exp(-color * exposure);
		color = pow(color, vec4(1.f / gamma));
		break;
	case output_norm_samples:
		color = vec4(vec3(ray_state.sample_n / float(globals.rendered_count)), 1.f);
		break;
	};

	store_state();
}
