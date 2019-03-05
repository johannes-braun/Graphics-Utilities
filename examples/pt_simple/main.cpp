#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#define MYGL_IMPLEMENTATION
#include "input/camera.hpp"
#include "input/input.hpp"
#include "input/input_glfw.hpp"
#include <gfx.ecs.components/basic.hpp>
#include <gfx.ecs.components/movement.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.file/file.hpp>
#include <gfx.math/datastructure/basic_bvh.hpp>
#include <mygl/mygl.hpp>
#include <random>

#include <Qt>
#include <QtWidgets/QtWidgets>

constexpr const char* screen = R"(
#version 460 core

layout(location=0) out vec2 uv;

void main()
{
    gl_Position = vec4(mix(-1, 3, gl_VertexID & 0x1), mix(-1, 3, (gl_VertexID >> 1) & 0x1), 0.f, 1.f);
    uv = ((gl_Position.xy+1)*0.5f);
	uv.y = 1-uv.y;
}
)";

static std::string pt = R"(
#version 460 core

layout(location=0) in vec2 uv;

float prand1(uint n)
{
    const float g = 1.6180339887498948482f;
    return fract(0.5 + n / g);
}

vec2 prand2(uint n)
{
    const float g = 1.32471795724474602596;
    const float a1 = 1.0 / g;
    const float a2 = 1.0 / (g*g);
    return fract(0.5 + vec2(a1, a2) * n);
}

layout(location = 0) uniform uint cur_sample;
uniform mat4 inverse_view_projection;
uniform vec3 cam_pos;
uniform float rndm;
layout(binding = 0) uniform samplerCube cubemap;

struct bvh_node
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
layout(std430, binding = 0) restrict readonly buffer BVH 
{ 
    bvh_node nodes[]; 
} bvh;
layout(std430, binding = 1) restrict readonly buffer VTX 
{ 
    vertex_t vertices[]; 
};
layout(std430, binding = 2) restrict readonly buffer IDX 
{ 
    uint indices[]; 
};

layout(rgba32f, binding = 0) uniform image2D fac_image;
layout(rgba32f, binding = 1) uniform image2D accum_image;
layout(rgba32f, binding = 2) uniform image2D i_orig_bounce;
layout(rgba32f, binding = 3) uniform image2D i_dir;
layout(rgba32f, binding = 4) uniform image2D i_arb;

layout(location = 0) out vec4 out_color;

const vec3 box_normals[] = vec3[](
    vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1),
    vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)
);

const uint bvh_node_type_inner = 0;
const uint bvh_node_type_leaf  = 1;
const uint bvh_mode_any      = 0;
const uint bvh_mode_nearest  = 1;

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

uint       _bvh_mode_current = bvh_mode_nearest;
void       bvh_state_set_mode(uint mode)
{
    _bvh_mode_current = mode;
}

bvh_result bvh_hit(const vec3 origin, const vec3 direction, const float max_distance);
bvh_result bvh_hit(vec3 origin, vec3 direction, const float max_distance, uint root_node);
bvh_result bvh_hit_instanced(const vec3 origin, const vec3 direction, const float max_distance);
void       bvh_state_set_mode(uint mode);



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


int faceID(float t, float t1, float t2, float t3, float t4, float t5, float t6)
{
    return t == t1 ? 0                                                     // neg_x
                   : (t == t2 ? 3                                          // pos_x
                              : (t == t3 ? 1                               // neg_y
                                         : (t == t4 ? 4                    // pos_y
                                                    : (t == t5 ? 2         // neg_z
                                                               : 5))));    // pos_z
}

struct hit_t
{
    int id;
    bool hits;
    vec3 pos;
    vec3 norm;
    float t;
};

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

    if(tmax >= 0 && tmin <= tmax && tmin <= max_distance)
    {
        face_tmin = faceID(tmin, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);
        face_tmax = faceID(tmax, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);
        min_distance     = min(tmin, tmax);
        out_max_distance = max(tmin, tmax);
        return true;
    }
    return false;
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

bvh_result bvh_hit(uint root, uint base_vertex, uint base_index, vec3 origin, vec3 direction, const float max_distance, mat4 transform)
{
    bvh_result result;
    result.near_distance   = max_distance;
    result.hits            = false;
    float current_distance = 0;
	
	mat4 inv_tf = inverse(transform);
	origin		= (inv_tf * vec4(origin, 1)).xyz;
	direction	= ((inv_tf * vec4(direction, 0)).xyz);
	uint root_node = root;

    bvh_node current_node = bvh.nodes[root_node];
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
            bool hits_left = intersect_bounds(origin, direction, bvh.nodes[root_node + left].bounds_min.xyz, bvh.nodes[root_node + left].bounds_max.xyz,
                                              result.near_distance, min_left);
            bool hits_right = intersect_bounds(origin, direction, bvh.nodes[root_node + right].bounds_min.xyz,
                                               bvh.nodes[root_node + right].bounds_max.xyz, result.near_distance, min_right);

            if (!hits_left && !hits_right) break;

            bool nrm         = min_left < min_right;
            uint first       = nrm ? left : right;
            uint second      = nrm ? right : left;
            bool hits_first  = nrm ? hits_left : hits_right;
            bool hits_second = nrm ? hits_right : hits_left;

            switchstack  = (switchstack << 1) | int(nrm);    // 0x1 -> right next, 0x0 -> left next
            bitstack     = (bitstack << 1) | int(hits_left && hits_right);
            current_node = bvh.nodes[root_node + (current_id = hits_first ? first : second)];
        }

        if (current_node.type == bvh_node_type_leaf)
        {
            vec2 current_barycentric;
            uint start = current_node.left;
            uint end   = current_node.right;
            for (uint i = start; i != end + 1; ++i)
            {
                vec3 tv1 = vertices[base_vertex + indices[base_index + 3 * i + 0]].position;
                vec3 tv2 = vertices[base_vertex + indices[base_index + 3 * i + 1]].position;
                vec3 tv3 = vertices[base_vertex + indices[base_index + 3 * i + 2]].position;

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

            current_id  = bvh.nodes[root_node + current_id].parent;
            bitstack    = bitstack >> 1;
            switchstack = switchstack >> 1;
        }

        current_id = ((switchstack & 0x1) == 0x1) ? bvh.nodes[root_node + bvh.nodes[root_node + current_id].parent].right
                                                  : bvh.nodes[root_node + bvh.nodes[root_node + current_id].parent].left;

        current_node = bvh.nodes[root_node + current_id];
        bitstack     = bitstack ^ 1;
    }
    return result;
}

hit_t
    ray_aabb_hit(int id, vec3 r0, vec3 rd, vec3 bmin, vec3 bmax)
{
    int fmin, fmax;
    float tmin = 0, tmax = 0;
    hit_t hit;
    hit.id = id;
    if(!intersect_bounds(r0, rd, bmin, bmax, 1.f / 0.f, tmin, tmax, fmin, fmax))
    {
        hit.hits = false;
        return hit;
    }
    if(tmin < 0)
    {
        hit.t = tmax;
        hit.norm = box_normals[fmax];
    }
    else
    {
        hit.t = tmin;
        hit.norm = box_normals[fmin];
    }
    hit.pos = r0 + hit.t * rd + 1e-3 * hit.norm;
    hit.hits = true;
    return hit;
}

hit_t ray_oobb_hit(int id, vec3 r0, vec3 rd, vec3 bmin, vec3 bmax, mat4 tf)
{
    const mat4 itf = inverse(tf);
    
    hit_t hit = ray_aabb_hit(id, (itf * vec4(r0, 1)).xyz, (itf * vec4(rd, 0)).xyz, bmin, bmax);
    if(!hit.hits)
        return hit;
    hit.norm = (tf * vec4(hit.norm, 0)).xyz;
    hit.pos = (tf * vec4(hit.pos, 1)).xyz;
    return hit;
}

bool intSphere( in vec4 sp, in vec3 ro, in vec3 rd, in float tm, out float t )
{
    bool  r = false;
    vec3  d = ro - sp.xyz;
    float b = dot(rd,d);
    float c = dot(d,d) - sp.w*sp.w;
    t = b*b-c;
    if( t > 0.0 )
    {
        t = -b-sqrt(t);
        r = (t > 0.0) && (t < tm);
    }

    return r;
}

hit_t ray_sphere_hit(int id, vec3 r0, vec3 rd, vec3 s0, float sr) {
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    hit_t hit;
    hit.id = id;
   
    if(!intSphere(vec4(s0, sr), r0, rd, 1.f / 0.f, hit.t))
    {
        hit.hits = false;
        return hit;
    }

    hit.pos = r0 + hit.t * rd;
    hit.norm = normalize(hit.pos - s0);
    hit.pos += 1e-3 * hit.norm;
    hit.hits = true;
    return hit;
}

hit_t nearest_hit(const in hit_t a, const in hit_t b)
{
    return (a.hits && b.hits && a.t < b.t) || !b.hits ? a : b;
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
// The GGX chi function
float DistributionGGX(vec3 N, vec3 H, float a)
{
	if(a < 0.0001f)
		return 1.f;

	const float PI =  3.14159265359f;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

vec3 tonemapFilmic(const vec3 color) {
	vec3 x = max(vec3(0.0), color + 0.004);
	return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
}
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
mat4 translate(const vec3 delta)
{
    mat4 u = mat4(1.0);    
    u[3].xyz = delta;
    return u;
}

vec3 randDisk(float u, float v, vec3 normal, float radius, out float x, out float y)
{
    const float PI = 3.14159265359;
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

)"
                        R"(

void main()
{
    const uint cs = cur_sample + uint(prand1(uint(gl_FragCoord.x)) * 138203 + prand1(uint(gl_FragCoord.y)) * 28103);

    vec4 orb = imageLoad(i_orig_bounce, ivec2(gl_FragCoord.xy));
    vec4 dir = imageLoad(i_dir, ivec2(gl_FragCoord.xy));
    vec4 fac = imageLoad(fac_image, ivec2(gl_FragCoord.xy));
    vec4 accum = imageLoad(accum_image, ivec2(gl_FragCoord.xy));
    vec4 arb = imageLoad(i_arb, ivec2(gl_FragCoord.xy));

    if(orb.w > 8)
        orb.w = 0;

    if(orb.w == 0)
    {
        vec2 img_size = imageSize(accum_image).xy;
        vec2 random_value = prand2(cs);
        vec2 random_value2 = prand2(cs + uint(mix(34144u, 1431223u, rndm)));
	    vec2 uv = vec2(((gl_FragCoord.xy + random_value*2-1) / vec2(img_size)) * 2 - 1);
        dir.xyz = vec3(inverse_view_projection * vec4(uv, 0.f, 1.f));
        orb.xyz = cam_pos;
    
        const float focal_length = 5.f;
		const vec3 focal_point = orb.xyz + focal_length * normalize(dir.xyz);
        vec2 offset;
        vec3 disk = randDisk(random_value2.x, random_value2.y, vec3(inverse_view_projection * vec4(0, 0, 0.f, 1.f)), 1.f, offset.x, offset.y);

        orb.xyz = orb.xyz + 0.01f * disk;
		dir.xyz = focal_point - orb.xyz;

        fac = vec4(1, 1, 1, 1);
        dir.w = 1.f;
        arb.x += 1.f;
    }

    const vec3 ndir = normalize(dir.xyz);
    
    vec3 colors[] = {
        vec3(1),
        vec3(1, 0, 0),
        vec3(0, 0, 1),
        vec3(1),
        vec3(1),
        vec3(0.95, 0.9, 0.85),
        vec3(1),
        vec3(1, 0.3, 0.1)
    };
    bool metal[] = {
        true,
        false,
        false,
        false,
        true,
        true,
        false,
        false
    };
    bool transmit[] = {
        false,
        false,
        false,
        false,
        false,
        false,
        true,
        true
    };
    float rough[] = {
        0.5f,
        0.5f,
        0.5f, 
        0.5f,
        0.1f,
        0.3f,
        0.1f,
        0.01f
    };

    hit_t bun_hit;
    bvh_result bun_hit_bvh = bvh_hit(0, 0, 0, orb.xyz, ndir, 1.f / 0.f, mat4(1.f));
    if(bun_hit_bvh.hits)
    {
        uint v0  = uint(0) + indices[0 + bun_hit_bvh.near_triangle * 3 + 1];
		uint v1  = uint(0) + indices[0 + bun_hit_bvh.near_triangle * 3 + 2];
		uint v2  = uint(0) + indices[0 + bun_hit_bvh.near_triangle * 3 + 0];
		bun_hit.norm = normalize((mat4(1.f) * vec4(bun_hit_bvh.near_barycentric.x * vertices[v0].normal + bun_hit_bvh.near_barycentric.y * vertices[v1].normal
					+ (1 - bun_hit_bvh.near_barycentric.x - bun_hit_bvh.near_barycentric.y) * vertices[v2].normal, 0)).xyz);
		bun_hit.pos = (mat4(1.f) * vec4(bun_hit_bvh.near_barycentric.x * vertices[v0].position + bun_hit_bvh.near_barycentric.y * vertices[v1].position
					+ (1 - bun_hit_bvh.near_barycentric.x - bun_hit_bvh.near_barycentric.y) * vertices[v2].position, 1)).xyz + 1e-3 * bun_hit.norm;
        bun_hit.hits = true;
        bun_hit.t = bun_hit_bvh.near_distance;
        bun_hit.id = 7;
    }
    else
    {
        bun_hit.hits = false;
    }


    mat4 rot = rotationMatrix(normalize(vec3(1, 0.8f, 0.2)), 40.f * 3.14159265359 / 360);
    mat4 rot2 = rotationMatrix(normalize(vec3(0, 0.7f, 1.2)), -60.f * 3.14159265359 / 360);
    hit_t all_hits[] = {
        ray_aabb_hit(0, orb.xyz, ndir, vec3(-1, -1.1, -1), vec3(1, -1, 1)),
        ray_aabb_hit(1, orb.xyz, ndir, vec3(-1.1, -1, -1), vec3(-1, 1, 1)),
        ray_aabb_hit(2, orb.xyz, ndir, vec3(1, -1, -1), vec3(1.1, 1, 1)),
        ray_aabb_hit(3, orb.xyz, ndir, vec3(-1, -1, 1), vec3(1, 1, 1.1)),
        
        ray_sphere_hit(4, orb.xyz, ndir, vec3(0.5f, -0.6f, 0.3f), 0.4f),
        ray_sphere_hit(5, orb.xyz, ndir, vec3(0.7f, -0.8f, -0.3f), 0.2f),
        ray_sphere_hit(6, orb.xyz, ndir, vec3(-0.5f, -0.6f, -0.6f), 0.4f),
        bun_hit, //ray_sphere_hit(7, orb.xyz, ndir, vec3(-0.5f, -0.2f, -0.6f), 0.2f),
    };

    hit_t nearest;
    nearest.hits = false;
    for(int i=0; i<all_hits.length(); ++i)
    {
        nearest = nearest_hit(nearest, all_hits[i]);
    }

    if(nearest.hits)
    {
        vec3 norm = nearest.norm;
        vec3 fnorm = faceforward(norm, dir.xyz, norm);
        orb.xyz = nearest.pos;
            
        const float alpha = rough[nearest.id] * rough[nearest.id];
        float D = DistributionGGX(fnorm, dir.xyz, alpha);
        if(isnan(D) || isinf(D) || D < 1e-5) D = 1.f;
        const vec2 smp = prand2(cs + uint(mix(381u, 2930u, rndm)));
        const vec3 msn = normalize(bsdf_local_to_world(ggx_importance_hemisphere(ggx_importance_sample(smp, alpha)), fnorm));

        const float ior_in = 1.f;
        const float ior_out = 1.5f;
        float F0 = pow((ior_in-ior_out)/(ior_in+ior_out), 2);
        float fresnel = F0 + (1 - F0) * pow(1.0 - max(dot(normalize(-dir.xyz), msn), 0), 5.0);
        vec3 odir = normalize(dir.xyz);
        dir.xyz = reflect(normalize(odir.xyz), msn);

        if(!metal[nearest.id] && prand1(cs + uint(mix(2931u, 18021u, rndm))) > fresnel)
        {
            if(transmit[nearest.id])
            {
                vec3 refracted = refract(odir, msn, fnorm == norm ? ior_in/ior_out : ior_out/ior_in);
                if(dot(refracted, refracted) != 0)
                {
                    dir.xyz = refracted;
                    orb.xyz -= 2e-3 * msn;
                }
            }
            else
            {
                dir.xyz = bsdf_local_to_world(sample_cosine_hemisphere(prand2(cs + uint(mix(41u, 381u, rndm)))), norm);
                fac *= vec4(colors[nearest.id], 1);
                D = 3.14159265359;
            }
        }

        fac *= D;// * abs(dot(norm, dir.xyz));
        orb.w = orb.w + 1;
        dir.w *= D;// * abs(dot(norm, dir.xyz));
    }
    else
    {
        const vec3 background = (texture(cubemap, dir.xyz).xyz); //vec4(0.8f, 0.9f, 0.95f, 1)
        out_color = fac * vec4(background, 1) / dir.w;
        accum += vec4(tonemapFilmic(out_color.xyz), out_color.w);
        orb.w = 0.f;
    }

    imageStore(accum_image, ivec2(gl_FragCoord.xy), accum);
    out_color = accum / (arb.x);

    out_color.xyz = out_color.xyz;

    imageStore(i_orig_bounce, ivec2(gl_FragCoord.xy), orb);
    imageStore(i_dir, ivec2(gl_FragCoord.xy), dir);
    imageStore(fac_image, ivec2(gl_FragCoord.xy), fac);
    imageStore(i_arb, ivec2(gl_FragCoord.xy), arb);
}
)";
int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    const auto w = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(w);
    mygl::load();


    glDebugMessageCallback([](gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char* message,
                              const void* userParam) { std::cout << message << '\n'; },
                           nullptr);

    std::mt19937                          g;
    std::uniform_real_distribution<float> d(0, 1);

    gfx::ecs::ecs ecs;
    auto user_entity = ecs.create_entity_shared(gfx::transform_component {glm::vec3 {0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});
    gfx::ecs::system_list   main_systems;
    gfx::glfw_input_system  input(w);
    gfx::user_camera_system camera_system(input);
    main_systems.add(input);
    main_systems.add(camera_system);

    auto vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &screen, nullptr);
    glCompileShader(vs);

    const char* css = pt.data();
    auto        fs  = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &css, nullptr);
    glCompileShader(fs);

    auto pr = glCreateProgram();
    glAttachShader(pr, vs);
    glAttachShader(pr, fs);
    glLinkProgram(pr);

    mygl::texture fac;
    glCreateTextures(GL_TEXTURE_2D, 1, &fac);
    glTextureStorage2D(fac, 1, GL_RGBA32F, 1280, 720);
    mygl::texture tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTextureStorage2D(tex, 1, GL_RGBA32F, 1280, 720);
    mygl::texture origbounce;
    glCreateTextures(GL_TEXTURE_2D, 1, &origbounce);
    glTextureStorage2D(origbounce, 1, GL_RGBA32F, 1280, 720);
    mygl::texture dir;
    glCreateTextures(GL_TEXTURE_2D, 1, &dir);
    glTextureStorage2D(dir, 1, GL_RGBA32F, 1280, 720);
    mygl::texture arb;
    glCreateTextures(GL_TEXTURE_2D, 1, &arb);
    glTextureStorage2D(arb, 1, GL_RGBA32F, 1280, 720);

    int  l;
    char buf[2048];
    glGetShaderInfoLog(vs, 2048, &l, buf);
    std::cout << buf << '\n';
    glGetShaderInfoLog(fs, 2048, &l, buf);
    std::cout << buf << '\n';
    glGetProgramInfoLog(pr, 2048, &l, buf);
    std::cout << buf << '\n';

    gfx::image_file cpx("hdri/hdr/posx.hdr", gfx::bits::b32, 4);
    gfx::image_file cnx("hdri/hdr/negx.hdr", gfx::bits::b32, 4);
    gfx::image_file cpy("hdri/hdr/posy.hdr", gfx::bits::b32, 4);
    gfx::image_file cny("hdri/hdr/negy.hdr", gfx::bits::b32, 4);
    gfx::image_file cpz("hdri/hdr/posz.hdr", gfx::bits::b32, 4);
    gfx::image_file cnz("hdri/hdr/negz.hdr", gfx::bits::b32, 4);

    mygl::texture cubemap;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);
    glTextureStorage2D(cubemap, 1, GL_RGBA32F, cpx.width, cpx.height);
    glTextureSubImage3D(cubemap, 0, 0, 0, 0, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpx.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 1, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cnx.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 2, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpy.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 3, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cny.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 4, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpz.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 5, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cnz.bytes());

    mygl::sampler cubemap_sampler;
    glCreateSamplers(1, &cubemap_sampler);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gfx::bvh3d      bvh(3);
    gfx::scene_file bun("bunny.dae", 0.3f);

	gfx::bvh3d::build_cache cache;
    for (int i = 0; i < 10; ++i)
    {
        auto t = std::chrono::steady_clock::now();
        bvh.build_indexed(cache, bun.mesh.indices.begin(), bun.mesh.indices.end(), [&](std::uint32_t i) { return bun.mesh.vertices[i]; });
        std::cout << (std::chrono::steady_clock::now() - t).count() << '\n';
	}
    mygl::buffer bun_bufs[3];
    glCreateBuffers(3, bun_bufs);
    glNamedBufferStorage(bun_bufs[0], bvh.nodes().size() * sizeof(gfx::bvh3d::node), bvh.nodes().data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(bun_bufs[1], bun.mesh.vertices.size() * sizeof(gfx::vertex3d), bun.mesh.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(bun_bufs[2], bun.mesh.indices.size() * sizeof(gfx::index32), bun.mesh.indices.data(), GL_DYNAMIC_STORAGE_BIT);

    mygl::vertex_array arr;
    glCreateVertexArrays(1, &arr);

    int count = 0;
    glClearColor(1, 1, 1, 1);
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(w))
    {
        const double delta = glfwGetTime() - last;
        last               = glfwGetTime();
        ecs.update(delta, main_systems);

        glm::vec4 clear(0, 0, 0, 0);
        glm::vec4 clear1(1);
        if (count == 0 || glfwGetKey(w, GLFW_KEY_R))
        {
            count = 0;
            glClearTexImage(fac, 0, GL_RGBA, GL_FLOAT, value_ptr(clear1));
            glClearTexImage(tex, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(dir, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(arb, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(origbounce, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
        }

        count = (count + 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(pr);
        glUniform1ui(0, count);
        glBindTextureUnit(0, cubemap);
        glBindSampler(0, cubemap_sampler);

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bun_bufs[0], 0, bvh.nodes().size() * sizeof(gfx::bvh3d::node));
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, bun_bufs[1], 0, bun.mesh.vertices.size() * sizeof(gfx::vertex3d));
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, bun_bufs[2], 0, bun.mesh.indices.size() * sizeof(gfx::index32));

        const auto iv = inverse(user_entity->get<gfx::transform_component>()->matrix());
        const auto ip = user_entity->get<gfx::projection_component>()->matrix();
        glUniformMatrix4fv(glGetUniformLocation(pr, "inverse_view_projection"), 1, false, value_ptr(inverse(ip * iv)));
        glUniform3fv(glGetUniformLocation(pr, "cam_pos"), 1, value_ptr(user_entity->get<gfx::transform_component>()->position));
        glUniform1f(glGetUniformLocation(pr, "rndm"), d(g));
        glBindVertexArray(arr);
        glBindImageTexture(0, fac, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, tex, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(2, origbounce, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(3, dir, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(4, arb, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void main_settings(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow settings_window;

    QWidget* settings_widget = new QWidget(&settings_window);
    settings_window.setCentralWidget(settings_widget);

    QFormLayout* items_layout = new QFormLayout(settings_widget);

    settings_window.show();

    QApplication::exec();
}
