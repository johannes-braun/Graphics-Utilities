#pragma once
#include <string_view>
#include <filesystem>

namespace gfx { inline namespace v1 { namespace shader_paths {
    const std::filesystem::path local_base_path = "D:/johannesbraun/C++/graphics_utilities/build/x64-Debug/src/gfx.shaders";
    namespace imgui { constexpr std::string_view imgui_frag = "imgui/imgui_frag.spv"; }
    namespace spectral_renderer { constexpr std::string_view spectral_frag = "spectral_renderer/spectral_frag.spv"; }
    namespace imgui { constexpr std::string_view imgui_vert = "imgui/imgui_vert.spv"; }
    constexpr std::string_view screen_vert = "screen_vert.spv";
}}}