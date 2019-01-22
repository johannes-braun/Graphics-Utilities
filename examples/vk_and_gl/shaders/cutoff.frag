#version 460 core

#ifdef VULKAN
layout( push_constant ) uniform ConstantBlock {
    float luma_cutoff;
} constants;
#else
uniform ConstantBlock {
    float luma_cutoff;
} constants;
#endif

layout(location = 0) in vec2 uv;

layout(binding = 0) uniform sampler2D input_texture;

layout(location = 0) out vec4 color;

float luma(vec3 color) {
	return dot(color, vec3(0.299, 0.587, 0.114));
}

void main()
{
    vec4 average = texture(input_texture, uv);
	if(luma(average.rgb) > 1.f)
		color = average;
	else
		color = vec4(0, 0, 0, 0);
}