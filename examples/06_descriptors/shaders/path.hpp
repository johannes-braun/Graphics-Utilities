#pragma once
#include <string_view>
#include <filesystem>

namespace gfx { inline namespace v1 { namespace shader_paths {
#ifndef GFX_HAS_LOCAL_BASE_PATH
#define GFX_HAS_LOCAL_BASE_PATH
    const std::filesystem::path local_base_path = "D:/johannesbraun/C++/graphics_utilities/build/x64-Debug/examples/06_descriptors";
#endif //GFX_HAS_LOCAL_BASE_PATH
    namespace spectral {
#ifndef GFX_HAS_PATH_EXAMPLES_06_DESCRIPTORS_SHADERS_SPECTRAL_FRAG
#define GFX_HAS_PATH_EXAMPLES_06_DESCRIPTORS_SHADERS_SPECTRAL_FRAG
    namespace shaders { constexpr std::string_view spectral_frag = "shaders/spectral_frag.spv"; }
#endif //GFX_HAS_PATH_EXAMPLES_06_DESCRIPTORS_SHADERS_SPECTRAL_FRAG
}}}}