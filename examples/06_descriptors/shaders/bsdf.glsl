//? #version 460 core
//? #include "geometry.glsl"
//? #include "random.glsl"
//? #include "sample.glsl"

// Global variables:
//? ray_state_t ray_state;
//? vertex_t hit_state;
//? instance_t	instance_state;

const uint bsdf_opaque = 0;
const uint bsdf_transparent = 1;
const uint bsdf_emissive = 2;

struct bsdf_result
{
	float pdf;
	vec3 bsdf;
	vec3  sampled_direction;
};

vec3 sample_bsdf(vec2 random_value);
vec3 evaluate_bsdf(vec3 out_dir, vec2 random_value);
float distribution_bsdf(vec3 out_dir, vec2 random_value);

bsdf_result compute_bsdf()
{
	bsdf_result result;
	//vec2 random_value = vec2(next_random(), next_random());//random_hammersley_2d(int(next_random() * ray_state.random.z + ray_state.sample_n * 727) % int(ray_state.random.z), 1.f/ray_state.random.z);

	const int max_samples = 800;
	vec2 random_value = random_hammersley_2d(int(next_random() * max_samples + ray_state.sample_n * 227) % max_samples, 1.f/max_samples);

	result.sampled_direction = normalize(sample_bsdf(random_value));
	result.bsdf = evaluate_bsdf(result.sampled_direction, random_value);
	result.pdf = distribution_bsdf(result.sampled_direction, random_value);

//	if(any(isnan(result.bsdf)))
//		result.bsdf = vec3(0);

	return result;
}


// The GGX chi function
float DistributionGGX(vec3 N, vec3 H, float a)
{
	if(a < 0.0001f)
		return 1.f;

	const float PI =  3.14159265359f;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float ior(float freq)
{
	float x = freq;
	float i = pow((1/(x+0.2) + pow((x+0.2), 2)) / 2 + 0.5, 0.4) + 0.8f;
	return 1.7 + 0.07 * freq;
}

bool _is_reflected = false;
bool _is_transmitted = false;
vec3 _msurf_normal;
float _fresnel;
vec3 sample_bsdf(vec2 random_value) 
{
	bool incoming = hit_state.metadata_position == 1;
	float alpha2 = max(instance_state.roughness * instance_state.roughness, 0.00001f);
	float ior_f = ior(ray_state.frequency);
	float ior_in = incoming ? 1.f : ior_f;
	float ior_out = incoming ? ior_f : 1.f;
	float F0 = pow((ior_in-ior_out)/(ior_in+ior_out), 2);
	if(instance_state.bsdf == bsdf_opaque)
		F0 = mix(F0, 1.0, sqrt(instance_state.reflectivity));
	_msurf_normal = normalize(local_to_world(ggx_importance_hemisphere(ggx_importance_sample(random_value, alpha2)), hit_state.normal));
	_fresnel = F0 + (1 - F0) * pow(1.0 - max(dot(-ray_state.direction, _msurf_normal), 0), 5.0);

	if(_fresnel > next_random())
	{
		_is_reflected = true;
		_is_transmitted = false;
		return reflect(ray_state.direction, _msurf_normal);
	}
	else
	{
		_is_reflected = false;
		_is_transmitted = instance_state.bsdf == bsdf_transparent;
		if(instance_state.bsdf == bsdf_opaque)
			return normalize(local_to_world(sample_cosine_hemisphere(random_value), hit_state.normal));
		else
		{
			vec3 refracted = refract(ray_state.direction, _msurf_normal, ior_in / ior_out);
			if(dot(refracted, refracted) == 0)
			{
				_is_reflected = true;
				_is_transmitted = false;
				return reflect(ray_state.direction, _msurf_normal);
			}
			else
			{
				return refracted;
			}
		}
	}
}

vec3 evaluate_bsdf(vec3 out_dir, vec2 random_value) 
{
	const float hdotv = max(dot(_msurf_normal, -ray_state.direction),0.0001);
	const float hdotl = max(dot(faceforward(hit_state.normal, -out_dir, hit_state.normal), out_dir),0.0001);
	float inner = _is_transmitted ? 1+0*DistributionGGX(_msurf_normal, -ray_state.direction, instance_state.roughness) :
		(1 / (3.14159265359f));
	float outer = 1+0*DistributionGGX(_msurf_normal, -ray_state.direction, instance_state.roughness);
	return (_is_reflected ? _fresnel * outer * mix(vec3(1.f), unpackUnorm4x8(instance_state.color).rgb, sqrt(instance_state.reflectivity)) : (1-_fresnel) * inner*unpackUnorm4x8(instance_state.color).rgb);// * hdotl;
}
float distribution_bsdf(vec3 out_dir, vec2 random_value) 
{
	const float hdotl = max(dot(faceforward(hit_state.normal, -out_dir, hit_state.normal), out_dir),0.0001);
	float inner = _is_transmitted ? 1+0*DistributionGGX(_msurf_normal, -ray_state.direction, instance_state.roughness) :
		(1 / 3.14159265359f);
	float outer = 1+0*DistributionGGX(_msurf_normal, -ray_state.direction, instance_state.roughness);
	return (_is_reflected ? _fresnel * outer : (1-_fresnel) * inner);// * hdotl;
}