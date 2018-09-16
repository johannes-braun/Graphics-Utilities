#version 460 core
#include "../api.glsl"
#include "sky.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal_u;
layout(location = 3) in vec3 albedo;

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(loc_gl(3) loc_vk(2, 0)) uniform sampler2DArrayShadow shadowmap;
struct shadow_cam_data
{
	mat4 view;
	mat4 proj;
	vec3 pos;
};
layout(loc_gl(2) loc_vk(2, 1)) uniform ShadowCamera
{
	shadow_cam_data data[8];
} shadow_camera;

layout(location = 0) out vec4 color;
const float chunk_size = 2.f;
const float chunk_count = 400;

vec3 get_pos(mat4 mat, vec3 pos)
{
    vec4 map_pos = mat * vec4(pos, 1);
    map_pos /= map_pos.w;
    map_pos.xy = 0.5f * map_pos.xy + 0.5f;
    return map_pos.xyz;
}

int get_layer(float dist, out float frac)
{
	ivec3 ts = textureSize(shadowmap, 0);
	float layers = ts.z;
	const int l = int(log2(int(dist / 20.f + 0.5f)));
	frac = 0.f;
	if(l >= layers) return -1;
	frac = fract(log2(dist / 20.f + 0.5f));
	return l;
}

float shadow(in sampler2DArrayShadow map, vec3 pos, float dist, vec3 normal, vec3 light_dir){
	ivec3 ts = textureSize(map, 0);
    vec2 tex_size = ts.xy;
	float layers = ts.z;
	
	float frac = 0.f;
	const int layer = get_layer(dist, frac);
	if(layer == -1)
		return 1.f;

	const mat4 mat = shadow_camera.data[layer].proj * shadow_camera.data[layer].view;
    vec3 map_pos = get_pos(mat, pos + 0.3f * normal);

	const float layer_next = clamp(layer+1, 0, layers-1);
	const mat4 mat_next = shadow_camera.data[int(layer_next)].proj * shadow_camera.data[int(layer_next)].view;
    vec3 map_pos_next = get_pos(mat_next, pos + 0.3f * normal);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));
    const int size = 3;

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const vec2 uv2 = clamp(map_pos_next.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec4(get_uv(uv), layer, map_pos.z)).r;
        const float depth2 = 1-texture(map, vec4(get_uv(uv2), layer_next, map_pos_next.z)).r;

        shadow += mix(depth, depth2, frac);
    }
    return shadow / (size*size);
}

void main()
{
	const vec3 normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const vec3 light  = normalize(position-shadow_camera.data[0].pos);

	const float ndotl = max(dot(light, normal), 0.f);

	color = vec4(ndotl * albedo, 1);

	const float cam_distxz = distance(position.xzy, camera.pos.xzy);
	float s = shadow(shadowmap, position, cam_distxz, normal, light);

	const float max_dist = chunk_size * chunk_count;
	color = mix(s * color + vec4(albedo * vec3(0.3f, 0.5f, 1.f) / 3.14159265359f, 0), vec4(sky_noclouds(view, camera.pos), 1), smoothstep(max_dist / 2.5f, max_dist/2.f, distance(position, camera.pos)));
}