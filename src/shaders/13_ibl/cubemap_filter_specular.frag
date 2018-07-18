#version 460 core
#define TAU 6.282185317
const float PI = 3.14159235659f;

layout(location = 0) in vec3 dir;
layout(location = 0) out vec4 color;

layout(binding = 0) uniform Roughness 
{
	float roughness;
};
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
vec3 importance_sample(vec2 random, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * random.x;
    float cos_theta = sqrt((1.0 - random.y) / (1.0 + (a*a - 1.0) * random.y));
    float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
	
	return normalize(vec3(cos(phi) * sin_theta , sin(phi) * sin_theta, cos_theta));
}

void main()
{
	vec3 direction = normalize(dir);
    vec3 u = normalize(cross(direction, (abs(direction.z) < 0.5f) ? vec3(1, 0, 0) : vec3(0, 1, 0)));
    mat3 to_world = mat3(u, cross(direction, u), direction);

	vec3 N = direction;    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = random_hammersley_2d(i, 1.f/SAMPLE_COUNT);
        vec3 H  = to_world * importance_sample(Xi, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(cubemap, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    color = vec4(prefilteredColor, 1.0);
}