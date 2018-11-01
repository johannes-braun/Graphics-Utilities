#pragma once
#include <string_view>
#include <filesystem>

namespace gfx { inline namespace v1 { namespace shader_paths {
#ifndef GFX_HAS_LOCAL_BASE_PATH
#define GFX_HAS_LOCAL_BASE_PATH
    const std::filesystem::path local_base_path = "C:/Users/johan/Documents/cpp/graphics_utilities/build/x64-RelWithDebInfo/examples/06_descriptors";
#endif //GFX_HAS_LOCAL_BASE_PATH
    namespace spectral {
    constexpr std::string_view spectral_frag = "spectral_frag.spv";
}}}}