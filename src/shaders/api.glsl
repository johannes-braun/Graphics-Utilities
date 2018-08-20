#ifndef API_GLSL
#define API_GLSL

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#define get_uv(UV) (UV)
#ifndef gl_VertexID
#define gl_VertexID gl_VertexIndex
#endif
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#define get_uv(UV) (vec2(UV.x, 1-UV.y))
#endif

#endif //API_GLSL