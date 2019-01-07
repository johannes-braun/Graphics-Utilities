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

    namespace meshing {
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_mesh_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_outline_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_points_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_mesh_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_outline_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_points_vert = {0u}; }
}}}}
