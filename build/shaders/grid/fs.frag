#include "sea.h"

layout(location=0) in struct {
    vec3 position;
    vec2 uv;
    vec3 normal;
} fragment_in;

layout(location=0) out vec4 color;
layout(location = 1) out vec4 out_normal_depth;

uniform mat4 inv_view;
uniform float time;
uniform samplerCube cubemap;
uniform sampler2D random;

const vec3 color_sand = vec3(1.f, 0.95f, 0.35f);
const vec3 color_grass = vec3(0.7f, 1.f, 0.2f);

void main()
{
    vec3 camera_position = inv_view[3].xyz;
	vec3 camera_look = normalize(fragment_in.position.xyz - camera_position);

    SEA_TIME = time;
    vec3 ds = fragment_in.position.xyz - camera_position;
    vec3 n = getNormal(fragment_in.position.xyz, dot(ds,ds) * EPSILON_NRM);
    vec3 light = normalize(vec3(0.0,1.0,0.8));
    vec3 ref = reflect(camera_look, n);


    float fresnel = 1.0 - max(dot(fragment_in.normal.xyz, -camera_look), 0.0);
    fresnel = pow(fresnel, 5.0);
    vec3 cube_ref = texture(cubemap, ref).xyz;
    vec3 refracted = vec3(1,1,1);
    vec3 c = cube_ref;//mix(refracted, cube_ref, fresnel);


    float off = 0.f; float tscale = 1.f;
    float sst1 = smoothstep(1.f, 0.4f, mod(fragment_in.position.x + off - sin(fragment_in.position.z+cos(tscale * time + fragment_in.position.x + off)), 8.f));
    
    off = 4.f; tscale = 1.6f;
    float sst2 = smoothstep(1.f, 0.4f, mod(fragment_in.position.x + off - sin(fragment_in.position.z+cos(tscale * time + fragment_in.position.x + off)), 8.f));

    c = mix(vec4(0.6f, 0.7f, 0.9f, 1), mix(vec4(0.4f, 0.5f, 0.8f, 1), vec4(0.2f, 0.3f, 0.6f, 1), sst1), sst2).rgb;

    float ns = noise(0.2f*fragment_in.position.xz + vec2(sin(0.1f*time), cos(0.1f*time)));
    float dist = distance(camera_position, fragment_in.position.xyz);
    ns += noise(0.5f*fragment_in.position.xz + vec2(sin(0.5f*time+1.5f), cos(0.2f*time + 0.3f)));
    ns += noise(0.3f*fragment_in.position.xz + vec2(sin(0.2f*time+2.5f), cos(0.4f*time + 1.3f)));
    ns += noise(0.7f*fragment_in.position.xz + vec2(sin(0.1f*time+3.5f), cos(0.1f*time + 2.3f)));
    c = mix(mix(0.4f*vec3(0.0f, 0.3f, 0.9f), 0.5f*vec3(0, 0.4f, 1.f), 
        smoothstep(0.41f+0.03f*sqrt(dist), 0.4f, ns*0.5f) * smoothstep(0.39f-0.03f*sqrt(dist), 0.4f, ns*0.5f)
        ), texture(cubemap, reflect(camera_look, fragment_in.normal.xyz)).xyz, fresnel);

    vec3 thin = c;//getSeaColor(fragment_in.position.xyz, n, light, camera_look, ds);

    float ss_base = smoothstep(1.8f, 2.5f, fragment_in.position.y);
    color = vec4(mix(0.6f*color_sand, 0.5f*color_grass * (0.2f*texture(random, 0.2f*fragment_in.uv).r + 0.8f), ss_base), 1);
    
    vec3 cm_norm = textureLod(cubemap, fragment_in.normal.xyz, 16).rgb;
    color *= vec4(cm_norm, 1);

    //color = vec4(vec3((fragment_in.position.y+0.3f) * 0.3f), 1);
    
    vec3 base_normal = fragment_in.normal.xyz;
    if(fragment_in.position.y < 1.01f)
    {
        base_normal = n;
        color = mix(color, 0.1f*vec4(thin, 1), smoothstep(1.1f, 1.f, fragment_in.position.y));
    }
    out_normal_depth = vec4(base_normal.xyz, (1-gl_FragCoord.z)/gl_FragCoord.w);
}