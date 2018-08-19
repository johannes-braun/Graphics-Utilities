#version 460 core
#include "../api.glsl"
#include "sky.glsl"

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

layout(loc_gl(1) loc_vk(1, 0)) uniform sampler2D bumpmap;
layout(loc_gl(2) loc_vk(1, 1)) uniform sampler2D colormap;
layout(loc_gl(1) loc_vk(1, 2)) uniform TerrainInfo
{
	float           chunk_size;
	int             chunk_count;
};

layout(loc_gl(3) loc_vk(2, 0)) uniform sampler2DShadow shadowmap;
layout(loc_gl(2) loc_vk(2, 1)) uniform ShadowCamera
{
	mat4 view;
	mat4 proj;
	vec3 pos;
} shadow_camera;

layout(location = 0) in vec3 position;
layout(location = 1) flat in vec3 normal_u;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 color;

mat3 cotangentFrame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1  = dFdx(p);
    vec3 dp2  = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    // solve the linear system
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T       = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B       = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = 1.0 / sqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, N);
}

vec3 from_bump(vec3 surf_pos, vec3 surf_norm, vec2 uv, vec3 view_dir, sampler2D bmap, float bscale)
{
    mat3 tbn = cotangentFrame(surf_norm, surf_pos, uv);

    float height = bscale * (texture(bmap, uv).r - 0.5f);

    // The following does weird stuff
    // uv += height * (view_dir * tbn).xy;

    vec2 duv = 1.f / vec2(textureSize(bmap, 0));
    vec4 h   = bscale *
             vec4(texture(bmap, uv + vec2(duv.x, 0)).r,
                  texture(bmap, uv + vec2(-duv.x, 0)).r,
                  texture(bmap, uv + vec2(0, duv.y)).r,
                  texture(bmap, uv + vec2(0, -duv.y)).r) /
             duv.xxyy;

    return tbn * normalize(cross(vec3(2, 0, h[0] - h[1]), vec3(0, 2, h[2] - h[3])));
}

vec3 get_pos(mat4 mat, vec3 pos)
{
    vec4 map_pos = mat * vec4(pos, 1);
    map_pos /= map_pos.w;
    map_pos.xy = 0.5f * map_pos.xy + 0.5f;
    return map_pos.xyz;
}

float shadow(in sampler2DShadow map, in mat4 mat, vec3 pos, vec3 normal, vec3 light_dir){

    vec2 tex_size = textureSize(map, 0);

    vec3 map_pos = get_pos(mat, pos + 0.1f * normal);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));
    const int size = 7;
    const vec2 frc = fract(map_pos.xy * tex_size + 0.5f).xy;
    float slope = 0.3f+clamp(tan(acos(dot(light_dir,normal))), -1, 1);

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const float eps = 0.0002 * slope;
        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec3(uv, map_pos.z + eps)).r;

        shadow += depth;
    }
    return /*max(dot(normal, light_dir),0) */ shadow / (size*size);
}

void main()
{
	const vec3 base_normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const float cam_dist = distance(position, camera.pos);

//	color = vec4(base_normal, 1);
//	return;

	vec3 normals[4];
	normals[0] = from_bump(position, base_normal, uv, view, bumpmap, 0.01f);
	for(int i=1; i < normals.length(); ++i)
	{
		normals[i] = mix(normals[i-1], from_bump(position, base_normal, uv * 2*(i+1), view, bumpmap, (i+1)*0.01f), 1.f / (i+1));
	}
	vec3 colors[4];
	colors[0] = texture(colormap, uv).rgb;
	for(int i=1; i < colors.length(); ++i)
	{
		colors[i] = mix(colors[i-1], texture(colormap, uv * 2 * (i+1)).rgb, 1.f / (i+1));
	}
	
	float scaling = 1-smoothstep(0, 1200, cam_dist);
	int index = clamp(int(normals.length() * scaling), 0, normals.length()-1);
	const vec3 normal = normalize(normals[index]);
	const vec3 albedo = colors[index];

	const vec3 light  = normalize(position-shadow_camera.pos);
	const float ndotl = max(dot(light, normal), 0.f);
	const float ndotv = max(dot(-view, normal), 0.f);

	const float f0 = pow((1+1.5f) / (1-1.5f), 2);
	const float fres = f0 + (1-f0)* (1-pow(ndotv, 5));

	float s = shadow(shadowmap, shadow_camera.proj * shadow_camera.view, position, normal, light);

	color = vec4(s * ndotl * albedo + sky_noclouds(view, normal), 1);

	//const float max_dist = chunk_size * chunk_count;
	//color = mix(color, vec4(sky(view, camera.pos), 1), smoothstep(max_dist / 4.f, max_dist/2.f, cam_dist));
}
