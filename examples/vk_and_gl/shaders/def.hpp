#ifndef DEF_HPP
#define DEF_HPP

#ifndef __cplusplus
#define CONSTEXPR const
#else
#define CONSTEXPR constexpr
#include <cstdint>
namespace def {
using uint = std::uint32_t;
#endif

#define DEF_use_rt_shadows                0

CONSTEXPR uint buffer_binding_vertex  = 1;
CONSTEXPR uint buffer_binding_element = 2;
CONSTEXPR uint buffer_binding_bvh     = 3;

#ifdef __cplusplus
}    // namespace def
#endif

#undef CONSTEXPR
#endif    // DEF_HPP