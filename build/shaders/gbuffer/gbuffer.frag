#include "../ext/deep_parallax.h"
#include "../ext/cotangent_frame.h"
#include "../ext/bvh.h"
#include "../ext/bsdf.h"

#include "scene.h"

#define NO_EFFECTS
#ifndef NO_EFFECTS
#define USE_PARALLAX_MAPPING
#define USE_NORMAL_MAPPING 
#endif //NO_EFFECTS


layout(location = 0) in vec4 position;
layout(location = 1) in vec4 uv;
layout(location = 2) in vec4 normal;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal_depth;

layout(binding = 0) uniform SceneBuffer { scene current_scene; }; 

struct mesh
{
    uint num_vertices;
    uint num_elements;

    uintptr_t elements;
    uintptr_t vertices;

    uintptr_t material;
    uintptr_t data;

    int _padding[2];

    mat4 transform;
    mat4 inverse_transform;
};

layout(binding = 8) restrict buffer Meshes
{
    mesh meshes[];
};
  
uniform samplerCube cube_map;
uniform sampler2D my_texture;
uniform sampler2D texture_depth;  
uniform sampler2D texture_normal; 

const int depth_max_layers = 8; 
const int depth_min_layers = 4; 
const float depth_scale = 0.01f; 
const float normal_scale = -1.f;

void main()
{
	vec3 camera_position = current_scene.inverse_view[3].xyz; 
	vec3 camera_view = normalize(position.xyz - camera_position.xyz);  
    mat3 cot_frame = cotangent_frame(normal.xyz, position.xyz, uv.xy);
    cot_frame[0] *= normal_scale;
    cot_frame[1] *= normal_scale;
 
    vec2 base_uv = uv.xy; 
    vec3 base_normal = normal.xyz;
    
#ifdef USE_PARALLAX_MAPPING
    vec3 parallax_view = transpose(cot_frame) * -camera_view;
    base_uv = deep_parallax(texture_depth, parallax_view, base_uv, depth_min_layers, depth_max_layers, depth_scale);
#endif //USE_PARALLAX_MAPPING

#ifdef USE_NORMAL_MAPPING    
    base_normal = (cot_frame * (texture(texture_normal, base_uv.xy).rgb * 2 - 0.5)); 
    base_normal = normalize(bsdf_correct_facet_normal(camera_view, base_normal, normal.xyz));
#endif //USE_NORMAL_MAPPING
   
    const vec3 albedo = vec3(0.83f, 0.69f, 0.22f);//texture(my_texture, base_uv.xy).rgb;
    const float roughness = 0.1f;  
    const float roughness_coat = 0.04f;  
    const float coat_strength = 0.04f;
    const float glass = 0.f;
    const float metal = 0.0f; 
    const float ior = 1.57f; 
    
    bsdf_env_sample environment = bsdf_env_get_sample(cube_map, camera_view, base_normal, ior, 
        roughness * roughness, roughness_coat * roughness_coat);

    vec3 fresnel;
    fresnel.x = bsdf_fresnel_coefficient(camera_view, base_normal, 0.35018f, 2.6876f);
    fresnel.y = bsdf_fresnel_coefficient(camera_view, base_normal, 0.48899f, 2.3389f);
    fresnel.z = bsdf_fresnel_coefficient(camera_view, base_normal, 1.4503f, 1.9483f);

    //out_color = vec4(dot(-camera_view, base_normal));
    //return;


    out_color = vec4(environment.reflection_color * fresnel, 1);
    return;

    //float fresnel = bsdf_fresnel(camera_view, base_normal, ior, metal);
    vec3 reflection_tint = mix(vec3(1), albedo, fresnel);
    vec3 diffuse_color = environment.diffuse_color * albedo;
    vec3 transmission_color = environment.transmission_color * albedo;
    vec3 reflection_color = environment.reflection_color * fresnel;
    vec3 reflection_color_coat = environment.reflection_color_coat;

    vec3 base = mix(diffuse_color, transmission_color, glass);
    vec3 overlay = mix(base, reflection_color, length(fresnel));
    overlay = mix(overlay, reflection_color_coat, coat_strength);

    out_color = vec4(overlay, 1);
    out_normal_depth = vec4(base_normal.xyz, (1-gl_FragCoord.z)/gl_FragCoord.w);
}   
