#include "ext/bsdf.h"

float map(vec3 pos);
#define MAP_FUNCTION(pos) map(pos)
#include "ray_march.h"

layout(location=0) in vec2 uv;
layout(location=0) out vec4 color;

uniform float clip_near = 0.01f;
uniform float clip_far = 100.f;
uniform int max_march_steps = 400;
uniform mat4 view_mat;
uniform mat4 inv_view_mat;
uniform samplerCube cubemap;
uniform vec3 cam_position;

float smin( float a, float b, float k )
{
    float h = clamp(0.5 + 0.5f * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0 - h);
}

float opS( float d1, float d2 )
{
    return max(-d1,d2);
}

float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float opU( float d1, float d2 )
{
    return min(d1,d2);
}

float opBlend(float d1, float d2)
{
    return smin(d1, d2, 0.2);
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdPlane( vec3 p, vec4 n )
{
  // n must be normalized
  return dot(p,n.xyz) + n.w;
}

float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,0.0))-r;
}

float map(vec3 pos)
{
    float res = 0;
    
    // ...
    res = udBox(pos + vec3(0, 1, 0), vec3(10, 1, 10));
    res = opBlend(res, sdSphere(pos + vec3(0, -0.2f, 1.f), 0.5f));
    res = opBlend(res, sdSphere(pos + vec3(0, -1.5f, 1.f), 1.0f));
    res = opS(sdSphere(pos + vec3(0, -1.5f, 1.6f), 0.7f), res);
    res = opBlend(res, sdSphere(pos + vec3(0, -2.5f, 1.f), 0.7f));
    res = opBlend(res, sdSphere(pos + vec3(0, -3.5f, 1.f), 0.3f));
    res = opBlend(res, sdSphere(pos + vec3(0, -4.f, 1.f), 0.2f));
    res = opBlend(res, sdTorus(pos + vec3(0, -1.5f, 1.f), vec2(1.2f, 0.2f)));
    res = opBlend(res, sdTorus(pos + vec3(0, -4.f, 1.8f), vec2(0.8f, 0.1f)));
    res = opBlend(res, sdTorus(pos + vec3(0, -4.f, 1.5f), vec2(0.3f, 0.1f)));

    res = opBlend(res, udBox(pos + vec3(0, 0.3f, 1.f), vec3(1.f, 0.6f, 1.f)));
    res = opS(sdSphere(pos + vec3(0, -1.5f, 2.2f), 0.5f), res);
    res = opS(sdTorus(pos + vec3(0, -1.5f, 1.f), vec2(1.5f, 0.3f)) + 0.1f*sin(10 * pos.x)*sin(10 * pos.y)*sin(10 * pos.z), res);
    res = opBlend(res, sdTorus(pos + vec3(0, -1.3f, 1.f), vec2(1.3f, 0.01f)));
    res = opBlend(res, sdTorus(pos + vec3(0, -1.5f, 1.f), vec2(1.3f, 0.04f)));
    res = opBlend(res, sdTorus(pos + vec3(0, -1.7f, 1.f), vec2(1.3f, 0.01f)));
    res = opBlend(res, sdSphere(pos + vec3(0, -1.5f, 2.3f), 0.2f));
    res = opBlend(res, udBox(pos + vec3(0, -4.3f, 1.f), vec3(0.2f, 1.6f, 0.2f))-0.05f);
    res = opBlend(res, udBox(pos + vec3(0, -5.3f, 1.f), vec3(0.1f, 1.8f, 0.1f)));
    res = opBlend(res, udBox(pos + vec3(0, -6.7f, 2.f), vec3(0.04f, 0.3f, 0.9f)));
    // ...

    return res;
}

void main()
{
    // Force Multisampling at every pixel
   gl_SampleMask[0] = 0xf;

    vec3 cam_pos = inverse(view_mat)[3].xyz;
    vec4 draargh = (inv_view_mat * vec4(2.f*(uv + gl_SamplePosition.xy / vec2(1280, 720) -0.5f), 0, 1));
    vec3 c_pos = cam_pos;
    vec3 c_dir = draargh.xyz / draargh.w;
    c_dir.xyz = normalize(c_dir.xyz - c_pos);

    float dist_next = march_cast_ray(c_pos, c_dir, clip_near, clip_far, max_march_steps);

    if(dist_next > 0.f)
    {
        vec3 pt = c_pos + c_dir * dist_next;
        vec3 normal = march_get_normal(pt);
        const float eps = 0.1f;
        float shad = march_soft_shadow(pt + eps * normal, normalize(vec3(0.1f, 0.5, -1)), eps, 50.f, 3);

        float fresnel = bsdf_fresnel(c_dir, normal, 0.4f, 0.f);

        float ao_fac = 0.25f;
        float ds = map(pt + ao_fac * normal);
        float ao = 1 - (ao_fac - ds);

        color = vec4(vec3(1.f)*shad*ao, 1);

        color *= textureLod(cubemap, normal, 16);
        
        c_dir = reflect(c_dir, normal);
        dist_next = march_cast_ray(pt, c_dir, clip_near, clip_far, int(max_march_steps * 0.8f));

        if(dist_next > 0)
        {
            normal = march_get_normal(pt+dist_next * c_dir);
            color = mix(color, textureLod(cubemap, reflect(c_dir, normal), 2), fresnel);
        }
        else
        {
            color = mix(color, clamp(textureLod(cubemap, c_dir, 2), 0.f, 1.f), fresnel);
        }
    }
    else
    {
        color = texture(cubemap, c_dir);
    }
    
    color = pow(color, vec4(1/1.6f));
}