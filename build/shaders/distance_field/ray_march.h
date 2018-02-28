#pragma once

#ifndef MAP_FUNCTION
#define MAP_FUNCTION(pos) 0
#endif

const float step_epsilon = 0.0001f;

float march_cast_ray(vec3 origin, vec3 direction, float mint, float maxt, const int max_steps)
{
    float dist = mint;
    for(int i=0; i<max_steps; ++i)
    {
        float next = MAP_FUNCTION((origin + dist * direction));
        if(next < step_epsilon * dist)
            break;
                
        dist += next;
        if(dist > maxt)
        {
            dist = -1.f;
            break;
        }
    }
    return dist;
}

float march_soft_shadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    for( float t=mint; t < maxt; )
    {
        float h = MAP_FUNCTION((ro + rd * t));
        if( h<0.001 )
            return 0.0;
        res = min( res, k*h/t );
        t += h;
    }
    return res;
}

vec3 march_get_normal( vec3 pos )
{
    vec3 eps = vec3(step_epsilon, 0.0, 0.0);
	vec3 nor = vec3(MAP_FUNCTION(pos + eps.xyy) - MAP_FUNCTION(pos - eps.xyy),
                    MAP_FUNCTION(pos + eps.yxy) - MAP_FUNCTION(pos - eps.yxy),
                    MAP_FUNCTION(pos + eps.yyx) - MAP_FUNCTION(pos - eps.yyx));
	return normalize(nor); 
}