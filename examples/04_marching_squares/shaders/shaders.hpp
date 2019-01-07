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

    namespace msq {
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_seg_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_seg_vert = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_seg_geom = {0u}; }
}}}}
