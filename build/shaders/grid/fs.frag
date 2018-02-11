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

void main()
{
    vec3 camera_position = inv_view[3].xyz;
	vec3 camera_look = normalize(fragment_in.position.xyz - camera_position);

    SEA_TIME = time;
    vec3 ds = fragment_in.position.xyz - camera_position;
    vec3 n = getNormal(fragment_in.position.xyz, dot(ds,ds) * EPSILON_NRM);
    vec3 light = normalize(vec3(0.0,1.0,0.8));
    vec3 thin = getSeaColor(fragment_in.position.xyz, n, light, camera_look, ds);

    color = vec4(vec3((fragment_in.position.y+0.3f) * 0.3f), 1);
    
    vec3 base_normal = fragment_in.normal.xyz;
    if(fragment_in.position.y < 1.1f)
    {
        base_normal = n;
        color = mix(color, vec4(thin, 1), smoothstep(1.1f, 1.f, fragment_in.position.y));
    }
    out_normal_depth = vec4(base_normal.xyz, (1-gl_FragCoord.z)/gl_FragCoord.w);
}