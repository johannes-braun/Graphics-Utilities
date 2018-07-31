#version 460 core

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D img;

void main()
{
	color = vec4(in_color.rgb, 1) * texture(img, in_uv);

	if(color.a < 0.3) discard;
}