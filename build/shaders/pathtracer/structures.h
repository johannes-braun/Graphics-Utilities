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
    float extinction_coefficient;

    float _p[3];
};

struct vertex {
    vec4 position;
    vec4 uv;
    vec4 normal;
};

struct triangle {
    uint a;
    uint b;
    uint c;
};