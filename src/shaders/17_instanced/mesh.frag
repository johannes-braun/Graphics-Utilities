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
	shadow_cam_data data[256];
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

int get_layer(float dist)
{
	ivec3 ts = textureSize(shadowmap, 0);
	float layers = ts.z;
	const int l = int(log2(int(dist / 5)));
	if(l >= layers) return -1;
	return l;
}

float shadow(in sampler2DArrayShadow map, in mat4 mat, vec3 pos, float dist, vec3 normal, vec3 light_dir){

	ivec3 ts = textureSize(map, 0);
    vec2 tex_size = ts.xy;
	float layers = ts.z;

    vec3 map_pos = get_pos(mat, pos + 0.1f * normal);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));
    const int size = 3;
    const vec2 frc = fract(map_pos.xy * tex_size + 0.5f).xy;
    float slope = 0.3f+clamp(tan(acos(dot(light_dir,normal))), -1, 1);

	const int layer = get_layer(dist);

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const float eps = 0.0002 * slope;
        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec4(uv, layer, map_pos.z + eps)).r;

        shadow += depth;
    }
    return /*max(dot(normal, light_dir),0) */ shadow / (size*size);
}

void main()
{
	const vec3 normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const vec3 light  = normalize(position-shadow_camera.data[0].pos);

	const float ndotl = max(dot(light, normal), 0.f);

	color = vec4(ndotl * albedo/* + sky_noclouds(view, normal)*/, 1);
	
	float s = 1;
	const float cam_distxz = distance(position.xz, camera.pos.xz);
	const int layer = get_layer(cam_distxz);
	if(layer != -1)
		s = shadow(shadowmap, shadow_camera.data[layer].proj * shadow_camera.data[layer].view, position, cam_distxz, normal, light);

	const float max_dist = chunk_size * chunk_count;
	color = mix(s * color, vec4(sky(view, camera.pos), 1), smoothstep(max_dist / 4.f, max_dist/2.f, distance(position, camera.pos)));
}