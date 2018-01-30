#pragma once

#include "ggx.h"

struct bsdf
{
    vec3 irradiance;    // (E) Radiated light (reflected or transmitted)
    float radiance;      // (B) Influence weight of incoming light
};

vec3 bsdf_correct_facet_normal(const in vec3 view, const in vec3 facet_normal, const in vec3 normal);
vec3 bsdf_local_to_world(const in vec3 vector, const in vec3 normal);
bsdf bsdf_reflection(const in vec3 view, const in vec3 normal, const in vec3 facet_normal, const in vec3 outgoing, float roughness);
float bsdf_normal_response(float ior, float metalness);
float bsdf_fresnel(const in vec3 view, const in vec3 normal, float ior, float metalness);



struct bsdf_env_sample
{
    // Reflection
    vec3 reflection_vector;
    vec3 reflection_color;

    // Coating reflection color
    vec3 reflection_color_coat;

    // Refraction/Transmission
    vec3 transmission_vector;
    vec3 transmission_color;

    // Diffuse (normal directed) environment tint
    vec3 diffuse_color;
};

const int bsdf_env_sampling_offset_count = 5;
const vec2 bsdf_env_sampling_offsets[bsdf_env_sampling_offset_count] = vec2[](
    vec2(-1, -1), 
    vec2(-1, 1),
    vec2(0, 0),
    vec2(1, -1), 
    vec2(1, 1)
);

// Sample the cube map for environment shading.
bsdf_env_sample bsdf_env_get_sample(
    // The cubemap
    samplerCube map, 
    // The direction of the incoming ray at the shaded point in world space.
    const in vec3 view,
    // World space normal
    const in vec3 normal,
    // Index of refraction
    float ior,
    // Squared roughness of the base material
    float roughness_squared,
    // Squared roughness of a coating laying over the material.
    float coat_roughness_squared);

#include "impl/bsdf_impl.inl"