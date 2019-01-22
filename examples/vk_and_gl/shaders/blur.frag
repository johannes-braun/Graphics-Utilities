#version 460 core
#ifdef VULKAN
layout( push_constant ) uniform ConstantBlock {
    ivec2 resolution;
} constants;
layout(constant_id = 0) const uint kernel_half_size = 15;
layout(constant_id = 1) const uint blur_direction   = 0;
#else
layout(location=0) uniform ivec2 resolution_c;
const uint kernel_half_size = 15;
layout(location=2) uniform uint blur_direction = 0;
#endif


layout(location = 0) in vec2 uv;
layout(binding = 0) uniform sampler2D input_texture;

layout(location = 0) out vec4 color;

float weights[16] = float[] (0.06707,0.066147,0.063453,0.059206,0.053733,0.047433,0.040727,0.034013,0.02763,0.021831,0.016778,0.012542,0.009119,0.006449,0.004436,0.002968);

struct TM_c
{
	ivec2 resolution;
};
void main()
{
#ifndef VULKAN
	TM_c constants;
	constants.resolution = resolution_c;
#endif

    const ivec2 res = constants.resolution;
    vec2 step_width = vec2(0, 0);
    step_width[blur_direction] = 1.0f / res[blur_direction];

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