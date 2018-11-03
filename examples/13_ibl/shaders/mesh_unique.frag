#version 460 core
#include <gfx.shaders/api.glsl> //! #include "../../../src/gfx.shaders/include/gfx.shaders/api.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 unnormalized_normal;

layout(loc_gl(0) loc_vk(0, 0)) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 position;
} camera;

layout(loc_gl(1) loc_vk(0, 1)) uniform Lighting
{
	float gamma;
	float exposure;
} lighting;

struct packed_material
{
	uint packed_logf0_roughness;
	uint packed_albedo_transparency;
};

layout(loc_gl(0) loc_vk(0, 2), std430) restrict readonly buffer Materials
{
	packed_material materials[];
};

layout(loc_gl(0) loc_vk(0, 3)) uniform samplerCube irradiance_cubemap;
layout(loc_gl(1) loc_vk(0, 4)) uniform samplerCube specular_cubemap;
layout(loc_gl(2) loc_vk(0, 5)) uniform sampler2D brdf_lut;

layout(location = 0) out vec4 color;

vec3 fresnel(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

vec3 get_f0(vec3 n, vec3 k)
{
    const vec3 k2  = k * k;
	const vec3 omn = 1-n;
	const vec3 opn = 1+n;
	return (omn * omn + k2) / (opn * opn + k2);
}

void main()
{
	const vec4 unpacked_albedo_transparency = unpackUnorm4x8(materials[0].packed_albedo_transparency);
	const vec4 unpacked_logf0_roughness		= unpackUnorm4x8(materials[0].packed_logf0_roughness);

	const vec3 albedo = unpacked_albedo_transparency.rgb;
	const float transparency = unpacked_albedo_transparency.a;
	const vec3 f0 = pow(vec3(2.71828182846), unpacked_logf0_roughness.rgb)-1;
	const float roughness = unpacked_logf0_roughness.a;

	vec3 normal = normalize(unnormalized_normal);
	vec3 view = normalize(position - camera.position);
	float ndotv = max(dot(normal, -view), 0.f);

	vec3 fnl = fresnel(ndotv, f0, roughness);

	vec3 irradiance = textureLod(irradiance_cubemap, normal, 0).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 kS = fnl;
	vec3 kD = 1.0 - kS;
	vec3 R = reflect(view, normal);
	const float MAX_REFLECTION_LOD = 10.0;
	vec3 prefilteredColor = textureLod(specular_cubemap, R, roughness * MAX_REFLECTION_LOD).rgb;   
	vec2 envBRDF  = texture(brdf_lut, vec2(ndotv, roughness)).rg;
	vec3 specular = prefilteredColor * (fnl * envBRDF.x + envBRDF.y);

	vec3 ior = vec3(1.6, 1.5, 1.4);

	vec3 refract_r = refract(view, normal, 1.f / ior.x);
	vec3 refract_g = refract(view, normal, 1.f / ior.y);
	vec3 refract_b = refract(view, normal, 1.f / ior.z);

	vec3 refr_color;
	refr_color.r = textureLod(specular_cubemap, refract_r, roughness * MAX_REFLECTION_LOD).r;
	refr_color.g = textureLod(specular_cubemap, refract_g, roughness * MAX_REFLECTION_LOD).g;
	refr_color.b = textureLod(specular_cubemap, refract_b, roughness * MAX_REFLECTION_LOD).b;

	color = vec4(kD * mix(diffuse, refr_color, transparency) + specular, 1.f);
	color = 1.f - exp(-color * lighting.exposure);
	color = pow(color, vec4(1.f / lighting.gamma));
}