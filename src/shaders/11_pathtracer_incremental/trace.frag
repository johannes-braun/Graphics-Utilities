#version 460 core

#include "../api.glsl"

layout(loc_gl(0) loc_vk(0, 7), rgba32f) uniform image2D accumulation_cache;
layout(loc_gl(1) loc_vk(0, 8), rgba32f) uniform image2D bounce_cache;
layout(loc_gl(2) loc_vk(0, 9), rgba32f) uniform image2D direction_cache;
layout(loc_gl(3) loc_vk(0, 10), rgba32f) uniform image2D origin_cache;
layout(loc_gl(4) loc_vk(0, 11), rg32ui) uniform uimage2D counter_cache;
layout(loc_gl(0) loc_vk(0, 6)) uniform samplerCube cubemap;
layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 position;
}
camera;
layout(loc_gl(1) loc_vk(0, 1)) uniform HelperInfo
{
    mat4  inverse_view_proj;
    float time;
    uint  reset;
	float rngval;
	uint bounce_limit;
};

struct bvh_attribute
{
    uint position_stride;
    uint position_offset;
    uint index_stride;
    uint index_offset;
};
struct bvh_node
{
    vec4 bounds_min;
    vec4 bounds_max;
    uint type;
    uint left;
    uint right;
    uint parent;
};
struct bvh_result
{
    // The index of the nearest and farthest triangles.
    // (multiply by 3 to get the element index)
    uint near_triangle;

    float near_distance;

    // Only two barycentric coordinates are needed.
    // Calculate the last one with z = 1 - bary.x - bary.y .
    vec2 near_barycentric;
	uint instance;
    bool hits;
};
layout(loc_gl(0) loc_vk(0, 2)) restrict readonly buffer ModelBVH
{
    bvh_node nodes[];
}
model_bvh;

struct vertex
{
        vec3 position;
        uint metadata_position;
        vec3 normal;
        uint metadata_normal;
        vec2 uv;
        uint metadata_uv;
};
layout(loc_gl(1) loc_vk(0, 3)) restrict readonly buffer ModelVertices
{
    vertex model_vertices[];
};
layout(loc_gl(2) loc_vk(0, 4)) restrict readonly buffer ModelIndices
{
    uint model_indices[];
};

#ifndef PI
#define PI 3.14159265359
#endif

// The GGX chi function
float ggx_chi(float v)
{
    return float(v > 0);
}

float ggx_distribution(const in float cos_theta, const in float roughness)
{
  float roughness2 = max(roughness * roughness, 0.0001f);
  float normal_dot_half2 = cos_theta * cos_theta;
  float denominator = normal_dot_half2 * roughness2 + (1-normal_dot_half2);
  return ggx_chi(cos_theta) * roughness2 / (PI * denominator * denominator);
}

float ggx_partial_geometry(const in vec3 vector, const in vec3 normal, const in vec3 facet_normal, float roughness)
{
  float eye_dot_half2 = (dot(vector, facet_normal));
  float chi = ggx_chi(eye_dot_half2 / (dot(vector, normal)));
  eye_dot_half2 = eye_dot_half2 * eye_dot_half2;
  float tan2 = (1 - eye_dot_half2) / eye_dot_half2;
  return (chi*2) / (1 + sqrt(1 + roughness * roughness * tan2));
}

float ggx_geometry(const in vec3 view, const in vec3 outgoing, const in vec3 normal, const in vec3 facet_normal, float roughness)
{
  float geom_in = ggx_partial_geometry(-view, normal, facet_normal, roughness);
  float geom_out = ggx_partial_geometry(outgoing, normal, facet_normal, roughness);
  return geom_in * geom_out;
}

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
layout(loc_gl(3) loc_vk(0, 5)) restrict readonly buffer ModelInstances
{
    instance model_instances[];
};

layout(location = 0) in vec2 uv_unused;
layout(location = 0) out vec4 color_output;

const uint bvh_mode_any      = 0;
const uint bvh_mode_nearest  = 1;
bvh_result bvh_hit(const vec3 origin, const vec3 direction, const float max_distance);
bvh_result bvh_hit(vec3 origin, vec3 direction, const float max_distance, uint root_node);
bvh_result bvh_hit_instanced(const vec3 origin, const vec3 direction, const float max_distance);
void       bvh_state_set_mode(uint mode);
bool       intersect_bounds(const vec3 origin, const vec3 direction, const vec3 bounds_min, const vec3 bounds_max, const float max_distance,
                            inout float min_distance, inout float out_max_distance, inout int face_tmin, inout int face_tmax);

vec2 random_hammersley_2d(float current, float inverse_sample_count);
vec3 sample_cosine_hemisphere(vec2 uv);
vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness);
vec3 ggx_importance_hemisphere(const in vec2 importance_sample);
vec3 bsdf_local_to_world(const in vec3 vector, const in vec3 normal);

const float inverse_max_uint = 1.f / float(0xFFFFFFFFu);
const float random_primes[6] = {69019.f, 96013.f, 32159.f, 22783.f, 87011.f, 45263.f};

uint  random_wang_hash(uint a);
uint  random_xorshift();
float next_random();
float random_value(int seed);
void  init_random(ivec2 pixel, float seed);


void shade_retrace(inout vec3 origin, inout vec3 direction, inout vec4 bounce_color, inout float diffusion, in vec3 position, in vec3 normal, in vec2 uv,
                   in vec2 random_value, const in instance inst)
{
	vec3 diffuse = unpackUnorm4x8(inst.color).rgb;
	float roughness = inst.roughness;
    float alpha2    = roughness * roughness;

	float F0 = 1.f-sqrt(inst.reflectivity);
	vec3 fresnel = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - max(dot(-direction, normal), 0), 5.0);

	if((fresnel.x + fresnel.y + fresnel.z)/3.f > next_random())
	{
		vec3 msnormal = bsdf_local_to_world(sample_cosine_hemisphere(random_value), normal);
		direction     = msnormal;
		origin        = position + direction * 1e-3f;
		bounce_color *= abs(dot(direction, normal)) * vec4(diffuse, 1); vec4(0.8f, 0.4f, 0.2f, 1);
		diffusion = 1;
	} else {
		vec3 msnormal = bsdf_local_to_world(ggx_importance_hemisphere(ggx_importance_sample(random_value, alpha2)), normal);
		direction     = reflect(direction.xyz, msnormal);
		origin        = position;
		bounce_color *= vec4(diffuse, 1);
		diffusion = 0;
	}
}


void main()
{
    ivec2 pixel    = ivec2(gl_FragCoord.xy);
    ivec2 img_size = ivec2(imageSize(accumulation_cache));
    if (any(greaterThanEqual(pixel, img_size))) return;

    init_random(ivec2(0, 0) + pixel, int(3000 * rngval));
    vec2 random_value =
        random_hammersley_2d(int(next_random() * img_size.x * img_size.y) % (1212121),
                             1.f / (1212121));
	vec2 uv = get_uv(vec2(((pixel + random_value-0.5f) / vec2(img_size)) * 2 - 1));
    vec3 precalc_direction = vec3(inverse_view_proj * vec4(uv, 0.f, 1.f));

    uvec2 c_counters      = reset != 0 ? imageLoad(counter_cache, pixel).xy : uvec2(0);
    vec4  c_accumulation  = reset != 0 ? imageLoad(accumulation_cache, pixel) : vec4(0, 0, 0, 1);
    vec4  c_bounce        = reset != 0 && c_counters.x != 0 ? imageLoad(bounce_cache, pixel) : vec4(1);
    vec4  c_direction     = reset != 0 && c_counters.x != 0 ? imageLoad(direction_cache, pixel) : vec4(precalc_direction, 0);
    vec4  c_origin        = reset != 0 && c_counters.x != 0 ? imageLoad(origin_cache, pixel) : vec4(camera.position, 0);
    vec4  c_display_color = vec4(0.f);

    // READ ----------------------------------------

    for (int i = 0; i < 3; ++i)
    {
        if (c_counters.x == 0)
        {
            random_value      = random_hammersley_2d(int(next_random() * img_size.x * img_size.y)
                                                    % (1212121),
                                                1.f / (1212121));
			uv = get_uv(vec2(((pixel + random_value-0.5f) / vec2(img_size)) * 2 - 1));
			precalc_direction = vec3(inverse_view_proj * vec4(uv, 0.f, 1.f));
            c_bounce          = vec4(1);
            c_direction       = vec4(precalc_direction, 0);
            c_origin          = vec4(camera.position, 0);
        }

        bvh_result hit = bvh_hit_instanced(c_origin.xyz, c_direction.xyz, 1.f / 0.f);
        if (hit.hits)
        {
			instance inst = model_instances[hit.instance];

            uint v0  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 1];
            uint v1  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 2];
            uint v2  = inst.base_vertex + model_indices[inst.base_index + hit.near_triangle * 3 + 0];
            vec3 pos = (inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].position + hit.near_barycentric.y * model_vertices[v1].position
                       + (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].position, 1)).xyz;
            vec3 norm = normalize((inst.transform * vec4(hit.near_barycentric.x * model_vertices[v0].normal + hit.near_barycentric.y * model_vertices[v1].normal
                        + (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].normal, 0)).xyz);
            vec2 uv = hit.near_barycentric.x * model_vertices[v0].uv + hit.near_barycentric.y * model_vertices[v1].uv
                      + (1 - hit.near_barycentric.x - hit.near_barycentric.y) * model_vertices[v2].uv;
			
			norm = faceforward(norm, c_direction.xyz, norm);

            vec3 orig = c_origin.xyz;
            vec3 dir  = c_direction.xyz;
			vec3 view = -dir;
			float diffusion = 0;
			shade_retrace(orig, dir, c_bounce, diffusion, pos, norm, uv, random_value, inst);
            c_origin    = vec4(orig + dir * 1e-3f, 1);
            c_direction = vec4(dir, 0);

			bvh_state_set_mode(bvh_mode_any);
			const vec3 light = vec3(-7, 8, -4);
			const float dist = distance(light, pos);
			const vec3 to_light = normalize(light - pos);
			bvh_result shadow_test = bvh_hit_instanced(pos + 1e-4 * to_light, to_light, dist);
			if (!shadow_test.hits)
			{
				vec3 diffuse = unpackUnorm4x8(inst.color).rgb;
				float roughness = inst.roughness;

				float F0 = 1.f-sqrt(inst.reflectivity);
				vec3 fresnel = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - max(dot(view, norm), 0), 5.0);

				float D = ggx_distribution(max(dot(norm, normalize(view + to_light)), 0), roughness);
				float G = ggx_geometry(view, to_light, norm, normalize(view + to_light), roughness);

				c_accumulation += diffusion * vec4(vec3(200) * max(dot(norm, to_light), 0) * unpackUnorm4x8(inst.color).rgb / (dist*dist), 0);
				c_accumulation += clamp(vec4(vec3(1) * max(dot(norm, to_light), 0) * (1-diffusion) * D * G * fresnel, 0), 0, 8);
			}
			bvh_state_set_mode(bvh_mode_nearest);

            float den = ++c_counters.x;
            if (den > bounce_limit)
            {
                ++c_counters.y;
                c_accumulation += clamp(c_bounce, 0, 8);
                c_counters.x = 0;
            }
            c_display_color = (c_accumulation + c_bounce / den) / (c_counters.y + 1);
        }
        else
        {
			//#define sample_env() texture(cubemap, c_direction.xyz)
			#define sample_env() vec4(0.2f)

			vec4 env = sample_env();
			if(any(isnan(env)))
			{
				env = vec4(0, 1, 1, 1);
			}

            ++c_counters.y;
            c_accumulation += vec4(c_bounce.rgb * clamp(env.rgb, 0, 8), 0);
            c_counters.x    = 0;
            c_display_color = (c_accumulation + c_bounce / (c_counters.x + 1)) / (c_counters.y + 1);
        }
    }
    // WRITE ----------------------------------------

    color_output = c_display_color;
	color_output = 1.f - exp(-color_output * 1.0f);
	color_output = pow(color_output, vec4(1.f / 1.8f));

    imageStore(counter_cache, pixel, uvec4(c_counters, 0, 0));
    imageStore(accumulation_cache, pixel, c_accumulation);
    imageStore(bounce_cache, pixel, c_bounce);
    imageStore(direction_cache, pixel, c_direction);
    imageStore(origin_cache, pixel, c_origin);
}


// IMPL -----------------------------------------------------------------------------------------------------------------------


vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness)
{
    float phi    = random_sample.y * 2.f * 3.14156235659f;
    float rough2 = roughness * roughness;
    float theta  = atan(sqrt(rough2 * random_sample.x / (1 - random_sample.x)));
    return vec2(phi, theta);
}

vec3 ggx_importance_hemisphere(const in vec2 importance_sample)
{
    float phi      = importance_sample.x;
    float cosTheta = cos(importance_sample.y);
    float sinTheta = sin(importance_sample.y);
    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

uint _rng_state     = 0;
int  _current_prime = 0;

float random_value(int seed)
{
    _rng_state += random_wang_hash(uint(seed));
    return clamp(float(random_xorshift()) * inverse_max_uint, 0.f, 1.f);
}
void init_random(ivec2 pixel, float seed)
{
    random_value(int(pixel.x * random_primes[0] + pixel.y * random_primes[1] + random_primes[2] * seed));
}
float next_random()
{
    return random_value(_current_prime = (_current_prime + 1) % 6);
}

// Method to generate a pseudo-random seed.
uint random_wang_hash(uint a)
{
    a = (a ^ 61) ^ (a >> 16);

    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

uint random_xorshift()
{
    // Xorshift algorithm from George Marsaglia's paper
    _rng_state ^= (_rng_state << 13);
    _rng_state ^= (_rng_state >> 17);
    _rng_state ^= (_rng_state << 5);
    return _rng_state;
}

vec2 random_hammersley_2d(float current, float inverse_sample_count)
{
    vec2 result;
    result.x = current * inverse_sample_count;

    // Radical inverse
    uint bits = uint(current);
    bits      = (bits << 16u) | (bits >> 16u);
    bits      = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits      = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits      = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits      = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    result.y  = float(bits) * 2.3283064365386963e-10f;
    return result;
}
vec3 sample_cosine_hemisphere(vec2 uv)
{
    // (Uniformly) sample a point on the unit disk
    float r     = sqrt(uv.x);
    float theta = 2 * 3.14159265359f * uv.y;
    float x     = r * cos(theta);
    float y     = r * sin(theta);

    // Project point up to the unit sphere
    float z = float(sqrt(max(0.f, 1 - x * x - y * y)));
    return vec3(x, y, z);
}
vec3 bsdf_local_to_world(const in vec3 vector, const in vec3 normal)
{
    // Find an axis that is not parallel to normal
	vec3 perp = vec3(1, 0, 0);
    vec3 u = normalize(cross(normal, perp));
	for(int i=1; any(isnan(u)) && i < 3; ++i)
	{
		perp[i-1] = 0;
		perp[i] = 1;
		u = normalize(cross(normal, perp));
	}
    return normalize(mat3(u, cross(normal, u), normal) * vector);
}

int faceID(float t, float t1, float t2, float t3, float t4, float t5, float t6)
{
    return t == t1 ? 0                                                     // neg_x
                   : (t == t2 ? 3                                          // pos_x
                              : (t == t3 ? 1                               // neg_y
                                         : (t == t4 ? 4                    // pos_y
                                                    : (t == t5 ? 2         // neg_z
                                                               : 5))));    // pos_z
}

bool intersect_bounds(const vec3 origin, const vec3 direction,

                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance, inout float min_distance,
                      inout float out_max_distance, inout int face_tmin, inout int face_tmax)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    face_tmin = faceID(tmin, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);
    face_tmax = faceID(tmax, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);

    min_distance     = min(tmin, tmax);
    out_max_distance = max(tmin, tmax);
    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

const uint bvh_node_type_inner = 0;
const uint bvh_node_type_leaf  = 1;

// Any hit means that the first hit that is being found when traversing will be returned.
// Nearest hit means that all traversed candidates will be tested for their distance on the ray.
//      The nearest (and in cases the farthest) intersections will be returned.
uint       _bvh_mode_current = bvh_mode_nearest;
void       bvh_state_set_mode(uint mode)
{
    _bvh_mode_current = mode;
}

bool intersect_bounds(const vec3 origin, const vec3 direction,

                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

bool intersect_bounds(const vec3 origin, const vec3 direction,

                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance, inout float min_distance)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    min_distance = min(tmin, tmax);
    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

bool intersect_triangle(const vec3 origin, const vec3 direction, const vec3 v1, const vec3 v2, const vec3 v3, inout float t,
                        inout vec2 barycentric)
{
    float float_epsilon  = 1e-23f;
    float border_epsilon = 1e-6f;

    // Find vectors for two edges sharing V1
    vec3 e1 = v2 - v1;
    vec3 e2 = v3 - v1;

    // if determinant is near zero, ray lies in plane of triangle
    vec3  P   = cross(vec3(direction), e2);
    float det = dot(e1, P);
    if (det > -float_epsilon && det < float_epsilon) return false;

    // Calculate u parameter and test bound
    float inv_det = 1.f / det;
    vec3  T       = origin.xyz - v1;
    barycentric.x = dot(T, P) * inv_det;

    // The intersection lies outside of the triangle
    if (barycentric.x < -border_epsilon || barycentric.x > 1.f + border_epsilon) return false;

    // Calculate V parameter and test bound
    vec3 Q        = cross(T, e1);
    barycentric.y = dot(vec3(direction), Q) * inv_det;
    // The intersection lies outside of the triangle
    if (barycentric.y < -border_epsilon || barycentric.x + barycentric.y > 1.f + border_epsilon) return false;

    return (t = dot(e2, Q) * inv_det) > float_epsilon;
}

bvh_result bvh_hit_instanced(const vec3 origin, const vec3 direction, const float max_distance)
{
	bvh_result hit;
	hit.hits = false;
	float md = max_distance;
	for(int i=0; i<model_instances.length(); ++i)
	{
		bvh_result nh = bvh_hit(origin, direction, md, i);
		if(nh.hits)
		{
			if (_bvh_mode_current == bvh_mode_any)
				return nh;
			hit = nh;
			md = hit.near_distance;
			hit.instance = i;
		}
	}
	return hit;
}

bvh_result bvh_hit(const vec3 origin, const vec3 direction, const float max_distance)
{
	return bvh_hit(origin, direction, max_distance, 0);	
}
bvh_result bvh_hit(vec3 origin, vec3 direction, const float max_distance, uint instance_id)
{
    bvh_result result;
    result.near_distance   = max_distance;
    result.hits            = false;
    float current_distance = 0;
	
	instance inst = model_instances[instance_id];
	mat4 inv_tf = inverse(inst.transform);
	origin		= (inv_tf * vec4(origin, 1)).xyz;
	direction	= ((inv_tf * vec4(direction, 0)).xyz);
	uint root_node = inst.base_bvh_node;

    bvh_node current_node = model_bvh.nodes[root_node];
    uint     current_id   = root_node;
    bool     hits_scene   = intersect_bounds(origin, direction, current_node.bounds_min.xyz, current_node.bounds_max.xyz, max_distance);

    uint bitstack    = 0;
    uint switchstack = 0;

    while (hits_scene)
    {
        while (current_node.type == bvh_node_type_inner)
        {
            uint  left      = current_node.left;
            uint  right     = current_node.right;
            float min_left  = 1.f / 0.f;
            float min_right = 1.f / 0.f;
            bool hits_left = intersect_bounds(origin, direction, model_bvh.nodes[root_node + left].bounds_min.xyz, model_bvh.nodes[root_node + left].bounds_max.xyz,
                                              result.near_distance, min_left);
            bool hits_right = intersect_bounds(origin, direction, model_bvh.nodes[root_node + right].bounds_min.xyz,
                                               model_bvh.nodes[root_node + right].bounds_max.xyz, result.near_distance, min_right);

            if (!hits_left && !hits_right) break;

            bool nrm         = min_left < min_right;
            uint first       = nrm ? left : right;
            uint second      = nrm ? right : left;
            bool hits_first  = nrm ? hits_left : hits_right;
            bool hits_second = nrm ? hits_right : hits_left;

            switchstack  = (switchstack << 1) | int(nrm);    // 0x1 -> right next, 0x0 -> left next
            bitstack     = (bitstack << 1) | int(hits_left && hits_right);
            current_node = model_bvh.nodes[root_node + (current_id = hits_first ? first : second)];
        }

        if (current_node.type == bvh_node_type_leaf)
        {
            vec2 current_barycentric;
            uint start = current_node.left;
            uint end   = current_node.right;
            for (uint i = start; i != end + 1; ++i)
            {
                vec3 tv1 = model_vertices[inst.base_vertex + model_indices[inst.base_index + 3 * i + 0]].position;
                vec3 tv2 = model_vertices[inst.base_vertex + model_indices[inst.base_index + 3 * i + 1]].position;
                vec3 tv3 = model_vertices[inst.base_vertex + model_indices[inst.base_index + 3 * i + 2]].position;

                if (intersect_triangle(origin, direction, tv1, tv2, tv3, current_distance, current_barycentric)
                    && current_distance < result.near_distance)
                {
                    result.hits             = true;
                    result.near_distance    = current_distance;
                    result.near_barycentric = current_barycentric;
                    result.near_triangle    = i;

                    if (_bvh_mode_current == bvh_mode_any) return result;
                }
            }
        }

        while ((bitstack & 1) == 0)
        {
            if (bitstack == 0) return result;

            current_id  = model_bvh.nodes[root_node + current_id].parent;
            bitstack    = bitstack >> 1;
            switchstack = switchstack >> 1;
        }

        current_id = ((switchstack & 0x1) == 0x1) ? model_bvh.nodes[root_node + model_bvh.nodes[root_node + current_id].parent].right
                                                  : model_bvh.nodes[root_node + model_bvh.nodes[root_node + current_id].parent].left;

        current_node = model_bvh.nodes[root_node + current_id];
        bitstack     = bitstack ^ 1;
    }
    return result;
}
