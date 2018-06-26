#pragma once

vec2 deep_parallax( 
    sampler2D displacement_texture, 
    const in vec3 view_tangent_space,
    const in vec2 base_uv,
    int min_layers, 
    int max_layers, 
    float depth_scale)
{
    float num_layers = mix(float(max_layers), float(min_layers), abs(dot(vec3(0, 0, 1), view_tangent_space)));
    float layer_depth = 1.0 / num_layers; 
    float current_layer_depth = 0.0; 
    vec2 P = view_tangent_space.xy * depth_scale; 
    vec2 delta = P / num_layers; 
    vec2 ofs = base_uv;  

    float depth = texture(displacement_texture, ofs).r;
    float current_depth = 0.0;
    int i = 0;
    while(current_depth < depth && i++ < max_layers) {
        ofs -= delta;
        depth = texture(displacement_texture, ofs).r;
        current_depth += layer_depth;
    }
    vec2 prev_ofs = ofs + delta;
    float after_depth  = depth - current_depth;
    float before_depth = texture(displacement_texture, prev_ofs).r - current_depth + layer_depth;
    float weight = after_depth / (after_depth - before_depth);
    return mix(ofs,prev_ofs,weight); 
}