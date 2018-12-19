#ifndef DEF_HPP
#define DEF_HPP

#ifndef __cplusplus
#define CONSTEXPR const
#define INIT_AS_1(A)
#define INIT_AS_2(A, B)
#define INIT_AS_3(A, B, C)
#define INIT_AS_4(A, B, C, D)
#define u8vec4 uint
#else
#define CONSTEXPR constexpr
#include <cstdint>
#include <glm/glm.hpp>
namespace def {
using namespace glm;
using uint = std::uint32_t;
#define INIT_AS_1(A) {(A)}
#define INIT_AS_2(A, B) {(A), (B)}
#define INIT_AS_3(A, B, C) {(A), (B), (C)}
#define INIT_AS_4(A, B, C, D) {(A), (B), (C), (D)}
#endif

#define DEF_use_rt_shadows 0

CONSTEXPR uint buffer_binding_vertex  = 1;
CONSTEXPR uint buffer_binding_element = 2;
CONSTEXPR uint buffer_binding_bvh     = 3;

struct mesh_info
{
    u8vec4 color INIT_AS_4(255, 50, 0, 255);
};

#ifdef __cplusplus
}    // namespace def
#endif

#undef CONSTEXPR
#undef INIT_AS_1
#undef INIT_AS_2
#undef INIT_AS_3
#undef INIT_AS_4
#endif    // DEF_HPP