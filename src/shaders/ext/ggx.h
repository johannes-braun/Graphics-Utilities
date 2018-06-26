#pragma once

// BSDF functions

// Calculates a normaal distribution for an input cos(theta) value (theta = angle between normal and microsurface normal).
float ggx_distribution(const in float cos_theta, const in float roughness);

// The full ggx geometric attenuation term.
float ggx_geometry(const in vec3 view, const in vec3 outgoing, const in vec3 normal, const in vec3 facet_normal, float roughness);


// Ray generation functions

// Computes an importance sample {phi, theta} from any random sample {(0..1], (0..1]}.
vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness);

// Transforms the importance sample into a z-up normal space vector.
vec3 ggx_importance_hemisphere(const in vec2 importance_sample);

#include "impl/ggx_impl.inl"