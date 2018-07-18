#version 460 core

const float PI = 3.14159235659f;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec2 brdf;

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

vec3 importance_sample(vec2 random, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * random.x;
    float cos_theta = sqrt((1.0 - random.y) / (1.0 + (a*a - 1.0) * random.y));
    float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
	
	return normalize(vec3(cos(phi) * sin_theta , sin(phi) * sin_theta, cos_theta));
}

float geom_schlick(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
float geom_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float ggx2 = geom_schlick(max(dot(N, V), 0.0), roughness);
    float ggx1 = geom_schlick(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}  

vec2 IntegrateBRDF(float ndotv, float roughness)
{
    vec3 V = vec3(sqrt(1.0 - ndotv*ndotv), 0.0, ndotv);

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = random_hammersley_2d(i, 1.f/SAMPLE_COUNT);
        vec3 H  = importance_sample(Xi, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float ndotl = max(L.z, 0.0);
        float ndoth = max(H.z, 0.0);
        float vdoth = max(dot(V, H), 0.0);

        if(ndotl > 0.0)
        {
            float G = geom_smith(N, V, L, roughness);
            float G_Vis = (G * vdoth) / (ndoth * ndotv);
            float Fc = pow(1.0 - vdoth, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main()
{
	brdf.xy = IntegrateBRDF(uv.x, uv.y);
}