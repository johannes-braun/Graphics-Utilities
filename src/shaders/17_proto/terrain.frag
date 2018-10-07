#version 460 core
#include "../api.glsl"
#include "sky.glsl"
#include "terrain.input.glsl"
#include "shadow.glsl"

layout(loc_gl(1) loc_vk(2, 0)) uniform sampler2D bumpmap;
layout(loc_gl(2) loc_vk(2, 1)) uniform sampler2D colormap;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_u;
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

void main()
{
	const vec3 base_normal = normalize(normal_u);
	const vec3 view   = normalize(position - camera.pos);
	const float cam_dist = distance(position, camera.pos);

//	color = vec4(base_normal, 1);
//	return;
////
//	vec3 normals[4];
//	normals[0] = from_bump(position, base_normal, uv, view, bumpmap, 0.01f);
//	for(int i=1; i < normals.length(); ++i)
//	{
//		normals[i] = mix(normals[i-1], from_bump(position, base_normal, uv * 2*(i+1), view, bumpmap, (i+1)*0.01f), 1.f / (i+1));
//	}
//	vec3 colors[4];
//	colors[0] = texture(colormap, uv).rgb;
//	for(int i=1; i < colors.length(); ++i)
//	{
//		colors[i] = mix(colors[i-1], texture(colormap, uv * 8 * (i+1)).rgb, 1.f / (i+1));
//	}
//	
	float scaling_0_25 = smoothstep(0, 25, cam_dist);
	float scaling_25_100 = smoothstep(25, 100, cam_dist);
	float scaling_100_500 = smoothstep(100, 500, cam_dist);
	
	float scaling_0_100 = smoothstep(0, 100, cam_dist);
	float scaling_0_500 = smoothstep(0, 500, cam_dist);
//	int index = clamp(int(normals.length() * scaling), 0, normals.length()-1);
//	const vec3 normal = normalize(normals[index]);
//	const vec3 albedo = colors[index];

	const vec3 normal = normalize(from_bump(position, base_normal, 2.f*uv, view, bumpmap, 0.1f) + from_bump(position, base_normal, 0.5f*uv, view, bumpmap, 0.1f) + from_bump(position, base_normal, 0.05f*uv, view, bumpmap, 0.1f) + from_bump(position, base_normal, 0.006f*uv, view, bumpmap, 0.1f));
	const vec3 albedo = texture(colormap, 0.1f*uv).rgb;

	const vec3 light  = normalize(position-shadow_camera.data[0].pos);
	const float ndotl = max(dot(light, normal), 0.f);
	const float ndotv = max(dot(-view, normal), 0.f);

	const float f0 = pow((1+1.5f) / (1-1.5f), 2);
	const float fres = f0 + (1-f0)* (1-pow(ndotv, 5));

	const float cam_distxz = distance(position.xzy, camera.pos.xzy); 
	float s = shadow(shadowmap, position, cam_distxz, normal, light);

	color = vec4(s * ndotl * albedo + albedo * vec3(0.3f, 0.5f, 1.f) / 3.14159265359f, 1);
	
	const float max_dist = chunk_size * chunk_count;
	color = mix(color, vec4(sky_noclouds(view, camera.pos), 1), smoothstep(max_dist / 2.5f, max_dist/2.f, cam_dist));
}
