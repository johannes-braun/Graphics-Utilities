#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 unnormalized_normal;

layout(binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 position;
} camera;

layout(binding = 0) uniform samplerCube irradiance_cubemap;
layout(binding = 1) uniform samplerCube specular_cubemap;
layout(binding = 2) uniform sampler2D brdf_lut;

layout(location = 0) out vec4 color;

vec3 fresnel(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main()
{
	const float roughness = 0.08f;
	const float metallic = 0.0f;
	const vec3 albedo = vec3(1.f, 0.1f, 0.02f);

	vec3 normal = normalize(unnormalized_normal);
	vec3 view = normalize(position - camera.position);
	float ndotv = max(dot(normal, -view), 0.f);
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	vec3 fnl = fresnel(ndotv, F0, roughness);

	vec3 irradiance = textureLod(irradiance_cubemap, normal, 0).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 kS = fnl;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;	
	vec3 R = reflect(view, normal);
	const float MAX_REFLECTION_LOD = 10.0;
	vec3 prefilteredColor = textureLod(specular_cubemap, R, roughness * MAX_REFLECTION_LOD).rgb;   
	vec2 envBRDF  = texture(brdf_lut, vec2(ndotv, roughness)).rg;
	vec3 specular = prefilteredColor * (fnl * envBRDF.x + envBRDF.y);

	color = vec4(kD * diffuse + specular, 1.f);
}