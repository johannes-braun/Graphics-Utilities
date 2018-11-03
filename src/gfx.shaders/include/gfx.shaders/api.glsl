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

vec2 get_uv(const in vec2 u)
{
	return vec2(u.x, 1.0-u.y);
}

#endif
#endif //API_GLSL