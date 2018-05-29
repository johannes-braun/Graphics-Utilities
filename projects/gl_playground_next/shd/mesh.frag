#include <ext/ggx.h>

layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal_world;
layout(location=3) flat in uint draw_id;

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

struct instance_info
{
    uint count;
    uint instance_count;
    uint base_index;
    uint base_vertex;
    uint base_instance;
    uint vertex_count;
    uint id;

    vec3 bounds_min;
    vec3 bounds_max;

    mat4 model_matrix;
    vec3 color;
    float roughness;
};

layout(binding = 10, std430) readonly buffer ModelData
{
    instance_info instances[];
};

layout(binding=5) uniform Time { float time; };
layout(binding=15) uniform sampler3D noise_texture;

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
    float slope = 0.3f+clamp(tan(acos(dot(light_dir,normal))), -1, 1);

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const float eps = 0.0002 * slope;
        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec3(uv, map_pos.z + eps)).r;

        shadow += depth;
    }
    return shadow / (size*size);
}

void main()
{
    vec3 normal = normalize(normal_world);
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

    rgh = instances[draw_id].roughness;
    float mat_ior = 1.5f;
    vec3 mat_color = instances[draw_id].color;

    for(int i=0; i<lights.length(); ++i)
    {
        light current_light = lights[i];

        vec3 light_pos = current_light.position.xyz;
        vec3 to_light = light_pos - position;
        vec3 light_dir = normalize(to_light);

        float ang = dot(light_dir, normal);
        float slope = sqrt(1 - ang*ang) / ang;
        float shd = 1;
        shd *= smoothstep(0.8f, 0.88f, dot(light_dir, -current_light.direction.xyz));
        if(shd == 0)
            continue;
        if(current_light.map.x + current_light.map.y != 0) 
            shd *= shadow(sampler2DShadow(current_light.map), current_light.matrix, position, normal, light_dir);
        

        vec3 light_color = current_light.color.w * current_light.color.xyz;
        vec3 view_dir = normalize(camera_position - position);
        float len2 = dot(to_light, to_light);
        float att = 2*pi / (len2);
    
        vec3 half_vector = normalize(light_dir + view_dir);

        float alpha = rgh;
        float alpha2 = alpha * alpha;

        // GGX Distribution
        float mdotn = max(dot(normal, half_vector), 0);
        float base = mdotn * mdotn * (alpha2 - 1) + 1;
        float dggx = alpha2 / (pi * base * base);

        // GGX Geometry term
        float ndotv = max(dot(normal, view_dir), 0);
        float ggx_geom_v = 1 / (ndotv + sqrt(mix(ndotv*ndotv, 1, alpha2)));
        float ndotl = max(dot(normal, light_dir), 0);
        float ggx_geom_l = 1 / (ndotl + sqrt(mix(ndotl*ndotl, 1, alpha2)));
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
        const float den = ((g+c)*c - 1) / max((g-c)*c + 1, 0.001f);
        float fct = gmcbgpc * (1 + den * den);
    
        // BRDF
        float brdf = (dggx * fct * ggx_geom);
        vec3 spec = max(dot(normal, light_dir) + 0.6f, 0) * brdf * light_color;
        vec3 diff = max(dot(normal, light_dir), 0) * mat_color * att * light_color * smoothstep(0.8f, 0.88f, dot(light_dir, -current_light.direction.xyz));
        color += vec4(shd * (spec + diff) + env * mat_color, 1);

        const int P = 0;
        const int N = 2;
        if(current_light.map.x + current_light.map.y != 0) 
        {
            float sigh = 0.f;
            for(int p = 0; p < P; ++p)
            {
                float dist = distance(position, camera_position) / N * (1-0.2f*random(gl_FragCoord.xyz, int(i + p*238 + 1214*time) % 48));
                for(int i=0; i<N; ++i)
                {
                    const vec3 wpos = camera_position - dist * i * view_dir + 0.1f * normal;
                    const vec3 pos = get_pos(current_light.matrix, wpos);
                    float fac = texture(noise_texture, wpos / 10.f).r;

                    sigh += abs(fac * 0.6f*(1-texture(sampler2DShadow(current_light.map), pos).r) * smoothstep(0.8f, 0.88f, dot(light_dir, -current_light.direction.xyz)));
                }
            }
            color = mix(color, current_light.color, sigh / (P*N));
        }
    }

    //color /= lights.length();
}