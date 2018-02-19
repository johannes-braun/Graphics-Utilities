#pragma once

#include "definition.hpp"

#include <vector>
#include <filesystem>

namespace glshader
{
    constexpr uint32_t make_tag(const char name[4])
    {
        return (name[0] << 0) | (name[1] << 8) | (name[2] << 16) | (name[3] << 24);
    }

    enum class shader_format : uint32_t
    {
        gl_binary = make_tag("GBIN"),
        spirv = make_tag("SPRV")
    };
    
    struct binary_shader
    {
        shader_format type;
        uint32_t format;
        std::vector<uint8_t> data;
    };

    binary_shader load_binary_shader(shader_format type,
        const std::experimental::filesystem::path& src,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions,
        bool force = false);
}
