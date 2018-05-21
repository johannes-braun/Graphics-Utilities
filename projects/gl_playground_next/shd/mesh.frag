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
};
layout(binding=0) uniform samplerCube cubemap;
layout(location=0) out vec4 color;

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

    vec3 env = vec3(0.5f, 0.55f, 0.6f);
    float pi = 3.14159265359f;

    float mval = smoothstep(-0.01f, 0.01f, position.x);
    vec3 col = mix(vec3(1.f), vec3(0, 0.2f, 1.f), mval);
    mval = smoothstep(-1.89f, -1.91f, position.x);
    col = mix(col, vec3(1.f, 0.2f, 0.f), mval);
    
    mval = smoothstep(-0.01f, 0.01f, position.x);
    float rgh = mix(1.f, 0.35f, mval);
    mval = smoothstep(-1.89f, -1.91f, position.x);
    rgh = mix(rgh, 0.04f, mval);

    float mat_ior = 1.5f;
    vec3 mat_color = col;

    for(int i=0; i<3; ++i)
    {
        vec3 light_pos = light_poses[i];
        vec3 light_color = 15.f*light_colors[i];
        vec3 view_dir = normalize(camera_position - position);
        vec3 to_light = light_pos - position;
        vec3 light_dir = normalize(to_light);
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
        color += vec4(spec + diff + env * mat_color, 1);
    }

    color /= 3;
}