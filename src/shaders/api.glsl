#ifndef API_GLSL
#define API_GLSL

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#ifndef gl_VertexID
#define gl_VertexID gl_VertexIndex
#endif
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

#endif //API_GLSL