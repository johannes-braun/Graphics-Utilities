#version 460 core

layout(location = 0) in vec2 uv;

layout(binding = 0) uniform sampler2D input_textures[2];

layout(location = 0) out vec4 color;

void main()
{
	const vec4 c0 = texture(input_textures[0], uv);
	const vec4 c1 = texture(input_textures[1], uv);
	color = max(c0, c1);

	const float gamma = 1.6;
    vec3 mapped = color.rgb;// / (color.rgb + vec3(0.1));
    mapped = pow(mapped, vec3(1.0 / gamma));
	color.rgb = mapped;
}