//! #version 460 core

vec3 sample_cosine_hemisphere(vec2 uv)
{
    // (Uniformly) sample a point on the unit disk
    float r     = sqrt(uv.x);
    float theta = 2 * 3.14159265359f * uv.y;
    float x     = r * cos(theta);
    float y     = r * sin(theta);

    // Project point up to the unit sphere
    float z = float(sqrt(max(0.f, 1 - x * x - y * y)));
    return vec3(x, y, z);
}
vec3 local_to_world(const in vec3 vector, const in vec3 normal)
{
    // Find an axis that is not parallel to normal
	vec3 perp = vec3(1, 0, 0);
    vec3 u = normalize(cross(normal, perp));
	for(int i=1; any(isnan(u)) && i < 3; ++i)
	{
		perp[i-1] = 0;
		perp[i] = 1;
		u = normalize(cross(normal, perp));
	}
    return normalize(mat3(u, cross(normal, u), normal) * vector);
}
vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness)
{
    float phi    = random_sample.y * 2.f * 3.14156235659f;
    float rough2 = roughness * roughness;
    float theta  = atan(sqrt(rough2 * random_sample.x / (1 - random_sample.x)));
    return vec2(phi, theta);
}

vec3 ggx_importance_hemisphere(const in vec2 importance_sample)
{
    float phi      = importance_sample.x;
    float cosTheta = cos(importance_sample.y);
    float sinTheta = sin(importance_sample.y);
    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}