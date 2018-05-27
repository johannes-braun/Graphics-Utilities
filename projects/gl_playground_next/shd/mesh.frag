#include <ext/ggx.h>

layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

layout(binding = 1) uniform ModelData
{
    mat4 model;
    vec3 material_color;
    float material_roughness;
};
layout(binding=0) uniform samplerCube cubemap;

struct light
{
    mat4 matrix;
    vec4 position;
    vec4 direction;
    vec4 color;
    uvec2 map;
};

layout(std430, binding=0) restrict readonly buffer Lights
{
    light lights[];
};

layout(location=0) out vec4 color;

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

const vec2 kernel2[9] = 
{
    vec2(0, 0),
    vec2(1, 0),
    vec2(0, 1),
    vec2(-1, 0),
    vec2(0, -1),
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(-1, -1)
};

const vec2 kernel[13] =
{
   vec2(-0.9328896, -0.03145855), // left check offset
   vec2(0.8162807, -0.05964844), // right check offset
   vec2(-0.184551, 0.9722522), // top check offset
   vec2(0.04031969, -0.8589798), // bottom check offset
   vec2(-0.54316, 0.21186), vec2(-0.039245, -0.34345),	vec2(0.076953, 0.40667),
   vec2(-0.66378, -0.54068),	vec2(-0.54130, 0.66730),	vec2(0.69301, 0.46990),
   vec2(0.37228, 0.038106),	vec2(0.28597, 0.80228), vec2(0.44801, -0.43844)
};

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

vec3 get_pos(mat4 mat, vec3 pos)
{
    vec4 map_pos = mat * vec4(pos, 1);
    map_pos /= map_pos.w;
    map_pos.xy = 0.5f * map_pos.xy + 0.5f;
    return map_pos.xyz;
}

float shadow(in sampler2DShadow map, in mat4 mat, vec3 pos, vec3 normal, vec3 light_dir){

    vec2 tex_size = textureSize(map, 0);

    vec3 map_pos = get_pos(mat, pos + 0.1f * normal);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));
    const int size = 7;
    const vec2 frc = fract(map_pos.xy * tex_size + 0.5f).xy;
    float slope = 0.4f+clamp(tan(acos(dot(light_dir,normal))), -1, 1);

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const float eps = 0.0001 * slope;
        const vec2 uv = map_pos.xy + offset;
        const float depth = 1-texture(map, vec3(uv, map_pos.z + eps)).r;

        shadow += depth;
    }
    return shadow / (size*size);
}

void main()
{
    vec3 light_poses[3] = {
        vec3(8, 8, 6),
        vec3(-4, -3, -7),
        vec3(1, 8, -2)
    };
    vec3 light_colors[3] = {
        vec3(1.f, 0.6f, 0.4f),
        vec3(0.3f, 0.7f, 1.f),
        vec3(0.6f, 1.f, 0.65f)
    };
    color = vec4(0);

    vec3 env = 0.01f*vec3(0.3f, 0.3f, 0.4f);
    float pi = 3.14159265359f;

    float mval = smoothstep(-0.04f, 0.01f, position.x);
    vec3 col = mix(vec3(1.f), vec3(0, 0.2f, 1.f), mval);
    mval = smoothstep(-1.82f, -1.91f, position.x);
    col = mix(col, vec3(1.f, 0.2f, 0.f), mval);
    
    mval = smoothstep(-0.04f, 0.01f, position.x);
    float rgh = mix(1.f, 0.15f, mval);
    mval = smoothstep(-1.82f, -1.91f, position.x);
    rgh = mix(rgh, 0.01f, mval);

    rgh = material_roughness;
    float mat_ior = 1.5f;
    vec3 mat_color = material_color;

    for(int i=0; i<lights.length(); ++i)
    {
        light current_light = lights[i];

        if(dot(current_light.color, current_light.color) == 0)
            continue;

        vec3 light_pos = current_light.position.xyz;
        vec3 to_light = light_pos - position;
        vec3 light_dir = normalize(to_light);

        float ang = dot(light_dir, normal);
        float slope = sqrt(1 - ang*ang) / ang;
        float shd = 1;
        if(current_light.map.x + current_light.map.y != 0) 
            shd = shadow(sampler2DShadow(current_light.map), current_light.matrix, position, normal, light_dir) 
                * smoothstep(0.8f, 0.88f, dot(light_dir, -current_light.direction.xyz));

        vec3 light_color = current_light.color.w * current_light.color.xyz;
        vec3 view_dir = normalize(camera_position - position);
        float len2 = dot(to_light, to_light);
        float att = 2*pi / (len2);
    
        vec3 half_vector = normalize(light_dir + view_dir);

        float alpha = rgh;
        float alpha2 = alpha * alpha;

        // GGX Distribution
        float mdotn = max(dot(normal, half_vector), 0);
        float dggx = alpha2 / (pi *pow(mdotn * mdotn * (alpha2 - 1) + 1, 2));

        // GGX Geometry term
        float ndotv = max(dot(normal, view_dir), 0);
        float ggx_geom_v = 1 / (ndotv + sqrt(alpha2 + (1-alpha2)*ndotv*ndotv));
        float ndotl = max(dot(normal, light_dir), 0);
        float ggx_geom_l = 1 / (ndotl + sqrt(alpha2 + (1-alpha2)*ndotl*ndotl));
        float ggx_geom = ggx_geom_v * ggx_geom_l;

        // Fresnel (Cook-Torrance)
        float fnorm = abs((1-mat_ior) / (1+mat_ior));
        fnorm = fnorm * fnorm;
        float sqfnorm = sqrt(fnorm);
        float nr = (1+sqfnorm) / (1-sqfnorm);
        float c = max(dot(view_dir, half_vector), 0);
        float g = sqrt(nr * nr + c * c - 1);
        float gmcbgpc = (g-c) / (g+c);
        gmcbgpc *= gmcbgpc;
        gmcbgpc *= 0.5f;
        float fct = gmcbgpc * (1 + pow(((g+c)*c - 1) / ((g-c)*c + 1), 2));
    
        // BRDF
        float brdf = (dggx * fct * ggx_geom);

        vec3 spec = max(dot(normal, light_dir) + 0.6f, 0) * brdf * light_color;
        vec3 diff = max(dot(normal, light_dir), 0) * mat_color * att * light_color;
        color += vec4(shd * (spec + diff) + env * mat_color, 1);
    }

    color /= 2;
}