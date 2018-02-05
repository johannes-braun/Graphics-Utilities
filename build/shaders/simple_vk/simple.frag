#include "../ext/bsdf.h"

layout(location = 0) in struct
{
    vec4 position;
    vec2 texcoords;
    vec4 normal;
    vec4 cam_position;
} fragment_input;

layout(binding = 0) uniform sampler2D image;

layout(location = 0) out vec4 color;

void main() 
{
    vec3 cam_v = normalize(fragment_input.position.xyz - fragment_input.cam_position.xyz).xyz;
    float frs = bsdf_fresnel(cam_v, fragment_input.normal.xyz, 0.8f, 0.f);

    vec3 lpos = vec3(3, 3, 2);
    vec3 v = lpos - fragment_input.position.xyz;
    float len = dot(v, v);
    vec3 to_light = normalize(v);
    vec3 lcol = vec3(40.f);

    vec3 env = vec3(0.7f, 0.8f, 1.f);
    float costheta = max(dot(to_light, fragment_input.normal.xyz), 0);
    vec3 diff = texture(image, fragment_input.texcoords).rgb;// vec3(0.4f, 0.4f, 0.4f);
    vec3 tint = env * diff;


    color = vec4(mix(mix(tint, max(lcol/len *diff, tint), costheta), env, frs), 1);
}