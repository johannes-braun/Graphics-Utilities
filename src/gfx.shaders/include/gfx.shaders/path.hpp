#pragma once
#include <string_view>
#include <filesystem>

namespace gfx { inline namespace v1 { namespace shader_paths {
#ifndef GFX_HAS_LOCAL_BASE_PATH
#define GFX_HAS_LOCAL_BASE_PATH
    const std::filesystem::path local_base_path = "C:/Users/johan/Documents/cpp/graphics_utilities/build/x64-RelWithDebInfo/src/gfx.shaders";
#endif //GFX_HAS_LOCAL_BASE_PATH
    namespace core {
#ifndef GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_FRAG
#define GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_FRAG
    namespace imgui { constexpr std::string_view imgui_frag = "imgui/imgui_frag.spv"; }
#endif //GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_FRAG
#ifndef GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_VERT
#define GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_VERT
    namespace imgui { constexpr std::string_view imgui_vert = "imgui/imgui_vert.spv"; }
#endif //GFX_HAS_PATH_SRC_GFX_SHADERS_SRC_IMGUI_IMGUI_VERT
    constexpr std::string_view screen_vert = "screen_vert.spv";
}}}}