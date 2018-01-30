#pragma once
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
  float eye_dot_half2 = dot(vector, facet_normal);
  float chi = ggx_chi(eye_dot_half2 / dot(vector, normal));
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

vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness)
{
  float phi = random_sample.y * 2.f * PI;
  float rough2 = roughness*roughness;
  float theta = atan(sqrt(rough2 * random_sample.x / (1 - random_sample.x)));
  return vec2(phi, theta);
}

vec3 ggx_importance_hemisphere(const in vec2 importance_sample)
{
  float phi = importance_sample.x;
  float cosTheta = cos(importance_sample.y);
  float sinTheta = sin(importance_sample.y);
  return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

#undef PI