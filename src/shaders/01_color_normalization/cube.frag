#version 460 core

layout(location=0) in vec2 uv;

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4((1.f - 0.85f*texture(tex, uv)).rgb, 1.f);
}