#pragma once

vec3 bsdf_correct_facet_normal(const in vec3 view, const in vec3 facet_normal, const in vec3 normal)
{
    const vec3 mid = normalize(cross(cross(view, normal), view) + normal);
    return dot(facet_normal, view) > dot(mid, view) ? reflect(-facet_normal, mid) : facet_normal;
}

vec3 bsdf_local_to_world(const in vec3 vector, const in vec3 normal) {
    // Find an axis that is not parallel to normal
    vec3 u = normalize(cross(normal, (abs(normal.x) <= 0.6f) ? vec3(1, 0, 0) : vec3(0, 1, 0)));
    return normalize(mat3(u, cross(normal, u), normal) * vector);
}

bsdf bsdf_reflection(const in vec3 view, const in vec3 normal, const in vec3 facet_normal, const in vec3 outgoing, float roughness)
{
    const float m_dot_in = dot(-view, facet_normal);
    const float geometry = ggx_geometry(view, outgoing, normal, facet_normal, roughness);
    bsdf result;
    result.irradiance = vec3(0);
    result.radiance = 1 / m_dot_in;
    return result;
}

bsdf bsdf_transmission(const in vec3 view, const in vec3 normal, const in vec3 facet_normal, const in vec3 outgoing, float roughness, float ior)
{
    const bool swap = dot(normal, view) > 0; // normal and view in the same direction -> outgoing
    const float ior_in = swap ? ior : 1.f;
    const float ior_out = swap ? 1.f : ior;
    const float eta = ior_in / ior_out;
    const float m_dot_in = dot(-view, facet_normal);
    const float m_dot_out = dot(outgoing, facet_normal);
    const float n_dot_in = dot(-view, normal);
    const float n_dot_out = dot(outgoing, normal);
	const float geometry = ggx_geometry(view, outgoing, faceforward(normal, view, normal), facet_normal, roughness);

    float val1 = abs(ior_out * ior_out * m_dot_in * m_dot_out / n_dot_in * n_dot_out);
    float den = pow(ior_in * m_dot_in, 2) + pow(ior_out * m_dot_out, 2);

    bsdf result;
    result.irradiance = vec3(0);
    result.radiance = val1 * geometry;
    result.radiance = geometry;//val1 * geometry / den;
    return result;
}

float bsdf_normal_response(float ior, float metalness)
{
  float response = abs((1-ior) / (1+ior));
  response = response * response;
  return mix(response, 1.f, metalness);
}

float bsdf_fresnel(const in vec3 view, const in vec3 normal, float ior, float metalness)
{
    float norm_response = bsdf_normal_response(ior, metalness); 
    return clamp(norm_response + (1-norm_response) * pow(1 - max(dot(-view, normal), 0), 5), 0, 1);
}

float bsdf_fresnel_coefficient(const in vec3 view, const in vec3 normal, float ior, float coefficient)
{
    float nm1 = ior - 1;
    nm1 *= nm1;

    float np1 = ior + 1;
    np1 *= np1;

    float c2 = coefficient * coefficient;

    float mcosth = 1 - abs(dot(-view, normal));
    float mcosth2 = mcosth * mcosth;
    float fn1mcosth = 4 * ior * mcosth2 * mcosth2 * mcosth;

    float nom = nm1 + fn1mcosth + c2;
    float den = np1 + c2;
    return nom / den;
}

int bsdf_largest_component(const in vec3 vector)
{
    return int(vector.y > vector.x || vector.z > vector.x) + int(vector.z > vector.y && vector.z > vector.x);
}

bsdf_env_sample bsdf_env_get_sample(samplerCube map, 
    const in vec3 view,
    const in vec3 normal,
    float ior,
    float roughness_squared,
    float coat_roughness_squared)
{ 
    bsdf_env_sample cm_sample; 
    cm_sample.reflection_vector = reflect(view, normal);
    cm_sample.transmission_vector = refract(view, normal, 1.f/ior);
    if(isnan(cm_sample.transmission_vector.x))
        cm_sample.transmission_vector = cm_sample.reflection_vector;

    ivec2 img_size2 = textureSize(map, 0); 
    float mipmapLevel = textureQueryLod(map, cm_sample.reflection_vector).x;
    float rough_scaling = log2(max(roughness_squared * img_size2.x, 1));   
    float rough_scaling_coat = log2(max(coat_roughness_squared * img_size2.x, 1));   
    cm_sample.reflection_color = vec3(0); 
    cm_sample.transmission_color = vec3(0);
    vec2 cms = 2.f*vec2(textureSize(map, int(max(mipmapLevel, rough_scaling))));
    vec2 cmsc = 2.f*vec2(textureSize(map, int(max(mipmapLevel, rough_scaling_coat))));
    vec2 inv_cms = 1.f / cms; 
    vec2 inv_cmsc = 1.f / cmsc; 
    for(int i=0; i<bsdf_env_sampling_offset_count; ++i)
    {  
        vec2 offset = bsdf_env_sampling_offsets[i] * inv_cms;
        vec2 offsetc = bsdf_env_sampling_offsets[i] * inv_cmsc;

        vec3 off_vec = cm_sample.reflection_vector;
        int l_rf_cm = bsdf_largest_component(cm_sample.reflection_vector);
        off_vec[(l_rf_cm+1) % 3] += offset.x;
        off_vec[(l_rf_cm+2) % 3] += offset.y;
        cm_sample.reflection_color += 0.2f * textureLod(map, normalize(off_vec), max(mipmapLevel, rough_scaling)).rgb;

        off_vec = cm_sample.reflection_vector;
        l_rf_cm = bsdf_largest_component(cm_sample.reflection_vector);
        off_vec[(l_rf_cm+1) % 3] += offsetc.x;
        off_vec[(l_rf_cm+2) % 3] += offsetc.y;
        cm_sample.reflection_color_coat += 0.2f * textureLod(map, normalize(off_vec), max(mipmapLevel, rough_scaling_coat)).rgb;

        off_vec = cm_sample.transmission_vector;
        int l_rr_cm = bsdf_largest_component(cm_sample.transmission_vector);

        off_vec[(l_rr_cm+1) % 3] += offset.x;
        off_vec[(l_rr_cm+2) % 3] += offset.y;
        cm_sample.transmission_color += 0.2f * textureLod(map, normalize(off_vec), max(mipmapLevel, rough_scaling)).rgb;
    } 

    cm_sample.diffuse_color = textureLod(map, normal, log2(max(img_size2.x, 1))).rgb;
    return cm_sample;
}