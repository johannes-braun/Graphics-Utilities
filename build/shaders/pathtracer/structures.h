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
    vec3 glass_tint;
    float roughness_sqrt;
    vec3 reflection_tint;
    float glass;
    vec3 base_color;
    float ior;
    vec3 glass_scatter_color;
    float glass_density;

    float glass_density_falloff;
    float extinction_coefficient;
    float _p[2];
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