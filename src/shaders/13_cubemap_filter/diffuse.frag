#version 460 core
#define TAU 6.282185317

layout(location = 0) in vec3 dir;
layout(location = 0) out vec4 color;

layout(binding = 0) uniform samplerCube cubemap;

vec2 random_hammersley_2d(float current, float inverse_sample_count)
{
    vec2 result;
    result.x = current * inverse_sample_count;

    // Radical inverse
	uint bits = uint(current);
    bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	result.y = float(bits) * 2.3283064365386963e-10f;
    return result;
}

vec3 sample_uniform_hemisphere(vec2 uv)
{
	float phi = uv.y * TAU;
	float cosTheta = abs(2 * uv.x - 1);
	float sinTheta = sqrt(max(0, 1 - cosTheta * cosTheta));
	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}


vec3 local_to_world(const in vec3 vector, const in vec3 normal) {
    // Find an axis that is not parallel to normal
    vec3 u = normalize(cross(normal, (abs(normal.x) <= 0.01f) ? vec3(1, 0, 0) : vec3(0, 1, 0)));
    return normalize(mat3(u, cross(normal, u), normal) * vector);
}

void main()
{
	color = vec4(0);
	vec3 direction = normalize(dir);

	const int count = 256;
	for(int i=0; i < count; ++i)
	{
		const vec2 rnd	= random_hammersley_2d(float(i), 1.f/count);
		const vec3 hemi = sample_uniform_hemisphere(rnd);
		const vec3 ltw	= local_to_world(hemi, direction);
		
		color += 3.14159265359 * textureLod(cubemap, ltw, 0);
	}
	color /= count;
}