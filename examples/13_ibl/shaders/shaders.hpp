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

    namespace ibl {
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_brdf_lut_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cubemap_filter_diffuse_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cubemap_filter_specular_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_mesh_unique_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_skybox_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cubemap_filter_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_mesh_unique_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_skybox_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_cubemap_filter_geom = {0u}; }
}}}}
