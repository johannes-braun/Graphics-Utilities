#pragma once

struct mesh
{
    intptr_t vertices;
    intptr_t elements;
    intptr_t bvh;
    intptr_t material;
    mat4 inv_model;
    mat4 model;
};

struct material
{
    vec3 ground_scatter_color;
    float ground_transmissiveness;
    vec3 ground_density_color;
    float ground_density;
    vec3 ground_reflection_color;
    float ground_density_falloff;
    float ground_roughness;

    float extinction;
    float ior;

    float surface_roughness;
    vec3 surface_reflection_color;
};

struct vertex {
    vec3 position;
    uint meta;
    vec3 normal;
    uint meta1;
    vec2 uv;
    uintptr_t meta2;
};

struct triangle {
    uint a;
    uint b;
    uint c;
};