#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace res
{
    constexpr const char* default_prefix = "#version 450 core \n \
        #extension GL_ARB_separate_shader_objects : require \n \
        #extension GL_ARB_bindless_texture : require \n \
        #extension GL_NV_shader_buffer_load : require \n \
        #extension GL_ARB_shading_language_420pack : require \n \
    ";
    constexpr const char* default_postfix = "\n \
        layout(bindless_sampler) uniform; \n \
        layout(bindless_image) uniform; \n \
    ";

    constexpr static const char* shader_cache_dir = "../cache/shaders";
    constexpr static const char* default_flags = " -std=450core ";
    constexpr static const char* glslc_location = "glslc.exe";
    constexpr static const char* temp_file_name = "shader_temp";

    constexpr uint32_t make_tag(char name[4])
    {
        return (name[0] << 0) | (name[1] << 8) | (name[2] << 16) | (name[3] << 24);
    }

    enum class shader_format : uint32_t
    {
        gl_binary = make_tag("GBIN"),
        spirv = make_tag("SPRV")
    };

    struct definition_info
    {
        definition_info() = default;
        definition_info(std::string value);
        definition_info(std::vector<std::string> parameters, std::string replacement);

        std::string replacement;
        std::vector<std::string> parameters;
    };

    struct definition
    {
        std::string name;
        definition_info info;
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
