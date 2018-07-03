#pragma once

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{ 
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
  
    // solve the linear system
    vec3 dp2perp = cross( dp2, N ); 
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = normalize(dp2perp * duv1.x + dp1perp * duv2.x); 
    vec3 B = normalize(dp2perp * duv1.y + dp1perp * duv2.y);
  
    // construct a scale-invariant frame  
    T = normalize(T - dot(N, T)); 
    B = normalize(B - dot(N, B));
    return mat3(T, B, N);  
} 