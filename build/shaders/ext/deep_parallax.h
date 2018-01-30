#pragma once

vec2 deep_parallax( 
    sampler2D displacement_texture, 
    const in vec3 view_tangent_space,
    const in vec2 base_uv,
    int min_layers, 
    int max_layers, 
    float depth_scale);

#include "impl/deep_parallax.inl"