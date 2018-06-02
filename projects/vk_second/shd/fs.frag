#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in flat uint draw_id;

layout(location = 0) out vec4 color;

layout(set = 2, binding = 0) uniform sampler2DShadow shadow_maps[1];

layout(set = 0, binding = 0) uniform Scene
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    uint object_count;
};

struct light
{
    vec4 color;
    vec4 position;
    vec4 direction;
};


struct mesh_info
{
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int vertexOffset;
    uint firstInstance;
    mat4 model_matrix;
    vec4 bounds[2];
};
layout(set = 1, binding=0, std430) readonly buffer Models
{
    mesh_info infos[];
};

const float pi = 3.14159235659;

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
    const int size = 5;
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
    return max(dot(normal, light_dir),0) * shadow / (size*size);
}

void main()
{
    color = vec4(0, 0, 0, 0);

    light current_light;
    current_light.position = vec4(3, 17, 3, 1);
    current_light.color = vec4(1, 1, 1, 15);
    current_light.direction = vec4(normalize(-current_light.position).xyz, 0);
    float rgh = 0.1f;
    float mat_ior = 1.5f;
    vec3 mat_color = vec3(1, 1, 1);
    vec3 env = 0.01*vec3(0.1f, 0.3f, 0.4f);
    float metal = 0.f;

    vec3 light_pos = current_light.position.xyz;
    vec3 to_light = light_pos - position;
    vec3 light_dir = normalize(to_light);
    vec3 light_color = current_light.color.w * current_light.color.xyz;
    vec3 view_dir = normalize(camera_position - position);
    float len2 = dot(to_light, to_light);
    float att = 2*pi / (len2);
    
    float shd = smoothstep(0.8f, 0.88f, dot(light_dir, -current_light.direction.xyz));

    mat4 shd_mat = mat4(1.009853, 1.006377, 0.000012, -0.058621, 0.000000, -0.118397, 0.000199, -0.996558, -1.009853, 1.006377, 0.000012, -0.058621, -0.000001, 0.000001, 0.016590, 17.058722);
    shd *= 1-shadow(shadow_maps[0], shd_mat, position, normal, light_dir);

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
    float fct = mix(gmcbgpc * (1 + den * den), 1, metal);
    
    // BRDF
    float brdf = (dggx * fct * ggx_geom);
    vec3 spec = max(dot(normal, light_dir) + 0.6f, 0) * brdf * light_color * mix(vec3(1), mat_color, metal);
    vec3 diff = mix(max(dot(normal, light_dir), 0) * mat_color * att * light_color, vec3(0), metal);
    color += vec4(shd * (spec + diff) + mix(env * mat_color, vec3(0), metal), 1);
}