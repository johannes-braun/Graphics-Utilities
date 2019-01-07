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

    namespace cubemapper {
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_skybox_frag = {0u}; }
    namespace shaders { dynamic_export std::vector<uint32_t> shaders_skybox_vert = {0u}; }
}}}}
