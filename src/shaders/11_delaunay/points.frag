#version 460 core

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D tex;

void main()
{
	color = vec4(in_color, 1) * texture(tex, gl_FragCoord.xy * 0.01f);
}