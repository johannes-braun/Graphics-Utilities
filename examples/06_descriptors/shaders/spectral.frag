#version 460 core
//! #extension GL_KHR_vulkan_glsl : enable
const float PI = 3.14159265359;
#include "geometry.glsl"

layout(location=0) in vec2 uv;

layout(std140, set=0, binding=0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 position;
    int  do_cull;
} camera;

const float focal_length = 6.f;
const float aperture = 0.02f;
const float sensor_response = 7.f; // may be received from texture
const float exposure = 1.0f;
const float gamma = 1.8f;

const int output_default = 0;
const int output_norm_samples = 1;

layout(set=0, binding=1) uniform sampler2D bokeh_shape;

layout(set = 1, binding = 0) restrict readonly buffer ModelBVH
{
    bvh_node nodes[];
} model_bvh;
layout(set = 1, binding = 1) restrict readonly buffer ModelVertices
{
    vertex_t model_vertices[];
};
layout(set = 1, binding = 2) restrict readonly buffer ModelIndices
{
    uint model_indices[];
};
layout(set = 1, binding = 3) restrict readonly buffer ModelInstances
{
    instance_t model_instances[];
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
layout(set = 2, binding = 7) uniform sampler2D randoms_image;

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
layout(location = 5) out vec4 randoms_output;
layout(location = 6) out vec4 fp_output;

#include "bvh.glsl"
#include "random.glsl"
#include "sample.glsl"

const mat3 cie_xyz_to_rgb = mat3(2.3706743, -0.9000405, -0.4706338,
								-0.5138850,  1.4253036,  0.0885814,
								 0.0052982, -0.0146949,  1.0093968);
const mat3 cie_rgb_to_xyz = mat3(0.4887180,  0.3106803,  0.2006017,
								 0.1762044,  0.8129847,  0.0108109,
								 0.0000000,  0.0102048,  0.9897952);

ray_state_t ray_state;
vertex_t	hit_state;
instance_t	instance_state;

#include "bsdf.glsl"

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

void load_state()
{
	ivec2 pixel    = ivec2(gl_FragCoord.xy);
	vec4 packed_positions_bounces = texelFetch(positions_bounce_image, pixel, 0);
	vec4 packed_directions_samples = texelFetch(directions_sample_image, pixel, 0);
	vec4 packed_bounce_accum = texelFetch(bounce_image, pixel, 0);
	vec4 accum_prob = texelFetch(accumulation_image, pixel, 0);
	vec4 randoms = texelFetch(randoms_image, pixel, 0);
	
	const int max_samples = 1600;
	init_random(ivec2(0, 0) + pixel, max_samples * globals.random);

	if(globals.rendered_count == 1)
	{
		randoms.w = ray_state.random.w = uintBitsToFloat(_rng_state);
		ray_state.bounce_n = 0;
		ray_state.sample_n = 0;
		ray_state.accum_color = vec3(0);
	}
	else
	{
		ray_state.bounce_n = floatBitsToInt(packed_positions_bounces.w);
		ray_state.sample_n = floatBitsToInt(packed_directions_samples.w);
		ray_state.accum_color = accum_prob.rgb;
	}

	if(ray_state.bounce_n == 0) 
	{
		ray_state.random.xy = random_hammersley_2d(int(next_random() * max_samples + ray_state.sample_n * 641) % max_samples, 1.f/max_samples);
		ray_state.random.w = uintBitsToFloat(_rng_state);
		ray_state.random.z =  max_samples;
	
		const mat4 inv_vp = inverse(camera.projection * mat4(mat3(camera.view)));
		ivec2 img_size = ivec2(globals.viewport);
		vec2 uvx = vec2(uv + (2*(ray_state.random.xy - 0.5f) / vec2(img_size)));
		ray_state.direction = vec3(inv_vp * vec4(uvx * 2 - 1, 0.f, 1.f));
		ray_state.origin = camera.position;

		// Apply DOF
		vec2 random_value_dof = ray_state.random.xy;

		const vec3 focal_point = ray_state.origin + focal_length * ray_state.direction;
		vec2 offset;
		const vec3 disk_sampled_point = randDisk(random_value_dof.x, random_value_dof.y, vec3(inv_vp * vec4(0, 0, 0.f, 1.f)), 1.f, offset.x, offset.y);

		ray_state.origin = ray_state.origin + aperture * disk_sampled_point;
		ray_state.direction = focal_point - ray_state.origin;

		const float bokeh_intensity = texture(bokeh_shape, vec2(0.5f) + offset).r;

		ray_state.direction = normalize(ray_state.direction);
		ray_state.frequency = next_random();
		ray_state.intensity_xyz = vec3(bokeh_intensity * sensor_response);
		ray_state.probability = 1.f;
	}
	else
	{
		ray_state.direction = packed_directions_samples.xyz;
		ray_state.origin = packed_positions_bounces.xyz;
		ray_state.frequency = packed_bounce_accum.r;
		ray_state.intensity_xyz = packed_bounce_accum.gba;
		ray_state.probability = accum_prob.a;
		ray_state.random = randoms.xyzw;
	}
}

void store_state()
{
	accumulation_output = vec4(ray_state.accum_color, ray_state.probability);
	bounce_accumulation_output = vec4(ray_state.frequency, ray_state.intensity_xyz);
	positions_bounce_output = vec4(ray_state.origin, intBitsToFloat(ray_state.bounce_n));
	directions_sample_output = vec4(ray_state.direction, intBitsToFloat(ray_state.sample_n));
	randoms_output = ray_state.random;
}

vec3 freq_to_xyz(float freq)
{
	return texture(cie_spectrum, freq).xyz;
}

void main()
{
	ivec2 pixel    = ivec2(gl_FragCoord.xy);
    ivec2 img_size = ivec2(globals.viewport);
    if (any(greaterThanEqual(pixel, img_size))) return;

	load_state();
	
    bvh_result hit = bvh_hit_instanced(ray_state.origin.xyz, ray_state.direction.xyz, 1.f / 0.f);
    if (hit.hits)
    {
		instance_state = model_instances[hit.instance];
		uint v0  = uint(instance_state.base_vertex) + model_indices[instance_state.base_index + hit.near_triangle * 3 + 1];
		uint v1  = uint(instance_state.base_vertex) + model_indices[instance_state.base_index + hit.near_triangle * 3 + 2];
		uint v2  = uint(instance_state.base_vertex) + model_indices[instance_state.base_index + hit.near_triangle * 3 + 0];
		hit_state.position = (instance_state.transform * vec4(hit.near_barycentric.x * model_vertices[v0].position + hit.near_barycentric.y * model_vertices[v1].position
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].position, 1)).xyz;
		hit_state.normal = normalize((instance_state.transform * vec4(hit.near_barycentric.x * model_vertices[v0].normal + hit.near_barycentric.y * model_vertices[v1].normal
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].normal, 0)).xyz);
		hit_state.uv = hit.near_barycentric.x * model_vertices[v0].uv + hit.near_barycentric.y * model_vertices[v1].uv
					+ (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].uv;
		hit_state.metadata_position = dot(hit_state.normal, ray_state.direction.xyz) < 0 ? 1 : 0;
		hit_state.normal = faceforward(hit_state.normal, ray_state.direction.xyz, hit_state.normal);
		
		bsdf_result bsdf = compute_bsdf();

		ray_state.direction = normalize(bsdf.sampled_direction);
		ray_state.origin = hit_state.position + 1e-4f * ray_state.direction;

		ray_state.probability *= bsdf.pdf;
		ray_state.intensity_xyz *= bsdf.bsdf;
		ray_state.bounce_n += 1;

		if(ray_state.bounce_n > 16 || ray_state.probability < 0.001f)
		{       
			vec3 env = pow(clamp(texture(environment_map, ray_state.direction).rgb, 0, 1000.f), vec3(1.f));
			vec3 add_result = vec3((transpose(cie_rgb_to_xyz) * freq_to_xyz(ray_state.frequency)) * ray_state.intensity_xyz / ray_state.probability);
			ray_state.accum_color += add_result;
			ray_state.sample_n += 1;
			ray_state.bounce_n = 0;
		}
	}  
	else 
	{   
		vec3 env = pow(clamp(texture(environment_map, ray_state.direction).rgb, 0, 1000.f), vec3(1.f));
		
		ray_state.intensity_xyz *= (env);
		vec3 add_result = vec3((transpose(cie_rgb_to_xyz) * freq_to_xyz(ray_state.frequency)) * ray_state.intensity_xyz / ray_state.probability);
		ray_state.accum_color += add_result;
		ray_state.sample_n += 1;
		ray_state.bounce_n = 0;	
	}
	 
	switch(globals.render_output)
	{ 
	case output_default:
		color = vec4((ray_state.accum_color / max(ray_state.sample_n + 1, 1)).rgb, 1);
		color = 1.f - exp(-color * exposure);
		color = pow(color, vec4(1.f / gamma));
		fp_output = color;
		break;
	case output_norm_samples:
		color = vec4(vec3(ray_state.sample_n / float(globals.rendered_count)), 1.f);
		break;
	};

	store_state();
}
