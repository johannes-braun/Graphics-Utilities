#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in flat uint draw_id;

layout(location = 0) out vec4 color;

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
    uint material_index;
    mat4 model_matrix;
    vec4 bounds[2];
};
layout(set = 1, binding=0, std430) readonly buffer Models
{
    mesh_info meshes[];
};
struct material_info
{
    vec3 f0;
    // packUnorm4x8
    uint color_roughness;
};
layout(set = 1, binding = 1, std430) readonly buffer Materials
{
    material_info materials[];
};

layout(set = 2, binding = 0) uniform sampler2DShadow shadow_maps[1];

struct light_info
{
    mat4 view;
    mat4 projection;
    vec3 position;
    uint object_count;
    vec3 color;
    int shadow_map;
};
layout(set = 2, binding=1, std430) readonly buffer Lights
{
    light_info lights[];
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

    vec3 map_pos = get_pos(mat, pos + 0.06f * normal);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));
    const int size = 5;

    for(int i=0; i<size*size; ++i)
    {
        const int x = (i % size - (size >> 1)-1);
        const int y = (i / size - (size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec3(uv, map_pos.z + 0.00001f)).r;

        shadow += depth;
    }
    return max(dot(normal, light_dir),0) * shadow / (size*size);
}

void main()
{
    color = vec4(0, 0, 0, 1);
    vec3 view_dir = normalize(camera_position - position);
    vec3 normal = normalize(in_normal);

    material_info material = materials[meshes[draw_id].material_index];
    vec4 color_roughness = unpackUnorm4x8(material.color_roughness);

    float rgh = color_roughness.w;
    float mat_ior = 0;
    vec3 mat_color = color_roughness.rgb;
    float metal = 0;
    vec3 env = 0.01*vec3(0.1f, 0.3f, 0.4f);

    vec3 fresnel = material.f0 + (1-material.f0)*pow(1-max(dot(view_dir, normal), 0), 5);

    for(int i = 0; i < 1; ++i)
    {
        light_info cligh = lights[i];

        light current_light;
        current_light.position = vec4(cligh.position, 1);
        current_light.color = vec4(cligh.color, 1);
        current_light.direction = vec4(normalize(-current_light.position).xyz, 0);

        vec3 light_pos = current_light.position.xyz;
        vec3 to_light = light_pos - position;
        vec3 light_dir = normalize(to_light);
        vec3 light_color = current_light.color.w * current_light.color.xyz;
        float len2 = dot(to_light, to_light);
        float att = 1 / (0.01f*len2);
    
        float light_half_fov = cos(atan(1.0/cligh.projection[1][1]));
        float shd = smoothstep(light_half_fov, 1.f, dot(light_dir, -current_light.direction.xyz));

        if(cligh.shadow_map != -1)
        {
            mat4 shadow_matrix = cligh.projection * cligh.view;
            shd *= 1 - shadow(shadow_maps[cligh.shadow_map], shadow_matrix, position, normal, light_dir);
        }

        shd = max((shd - 0.1) / 0.9, 0);

        //if(shd <= 0.f)
        //    continue;

        vec3 half_vector = normalize(light_dir + view_dir);

        float alpha = rgh;
        float alpha2 = alpha * alpha;
        vec3 fresnel_v = material.f0 + (1-material.f0)*pow(1-max(dot(view_dir, normal), 0), 5);
        vec3 fresnel_l = material.f0 + (1-material.f0)*pow(1-max(dot(light_dir, normal), 0), 5);

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
    
        // BRDF
        vec3 brdf = (dggx * fresnel_v * ggx_geom);
        vec3 spec = max(dot(normal, light_dir), 0) * brdf * light_color;
        vec3 diff = max(dot(normal, light_dir), 0) * mat_color * att * light_color;
        color += shd* vec4(mix(diff, spec, fresnel_l), 1);
    }

    //color += vec4(mix(env * mat_color, vec3(0), metal), 0);
}