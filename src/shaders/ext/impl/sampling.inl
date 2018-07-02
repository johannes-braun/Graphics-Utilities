#pragma once
#define TAU 6.282185317

vec3 sample_cosine_hemisphere(vec2 uv)
{
	// (Uniformly) sample a point on the unit disk
    float r = sqrt(uv.x);
    float theta = TAU * uv.y;
    float x = r * cos(theta);
    float y = r * sin(theta);

    // Project point up to the unit sphere
    float z = float(sqrt(max(0.f, 1 - x * x - y * y)));
    return vec3(x, y, z);
}

vec3 sample_uniform_sphere(vec2 uv)
{
	float cosTheta = float(2 * uv.x - 1);
	float phi = uv.y * TAU;
	float sinTheta = float(sqrt(max(0.f, 1 - cosTheta * cosTheta)));
	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

#undef TAU