#pragma once
#include <array>
#include <cinttypes>
#include <vector>

namespace gfx { inline namespace v1 { namespace spirv {

#if defined(_MSC_VER)
#define dynamic_export extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
#define dynamic_export extern "C" __attribute__((visibility("default")))
#endif

    namespace normalization {
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_center_point_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cube_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cube_front_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_gizmo_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_image_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_points_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_center_point_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cube_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_gizmo_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_points_vert = {0u}; }
}}}}
