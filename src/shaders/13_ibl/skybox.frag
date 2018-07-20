#version 460 core

layout(location = 0) in vec3 uv;

layout(binding = 0) uniform samplerCube cubemap;

layout(location = 0) out vec4 color;

layout(binding = 1) uniform Lighting
{
	float gamma;
	float exposure;
} lighting;

void main()
{
	color = texture(cubemap, uv);
	color = 1.f - exp(-color * lighting.exposure);
	color = pow(color, vec4(1.f / lighting.gamma));
}