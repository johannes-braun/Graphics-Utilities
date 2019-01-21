#version 460 core

layout(constant_id = 0) const uint kernel_half_size = 5;
layout(constant_id = 1) const uint blur_direction   = 0;

layout( push_constant ) uniform ConstantBlock {
    ivec2 resolution;
} constants;

layout(location = 0) in vec2 uv;

layout(binding = 0) uniform sampler2D input_texture;

layout(location = 0) out vec4 color;

float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    const ivec2 res = constants.resolution;
    vec2 step_width = vec2(0, 0);
    step_width[blur_direction] = 1.5f / res[blur_direction];

    vec4 average = texture(input_texture, uv) * weights[0];
    for(int i = 1; i < kernel_half_size; ++i)
    {
        vec2 new_uv = uv + step_width * i;
        if(new_uv[blur_direction] > 1)
            break;

        average += texture(input_texture, new_uv) * weights[i];
    }
    for(int i = 1; i < kernel_half_size; ++i)
    {
        vec2 new_uv = uv - step_width * i;
        if(new_uv[blur_direction] < 0)
            break;
        
        average += texture(input_texture, new_uv) * weights[i];
    }

    color = average;
}