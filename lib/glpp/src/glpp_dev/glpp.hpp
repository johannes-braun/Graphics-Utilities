#pragma once

#include <experimental/filesystem>
#include <map>
#include <set>

namespace glpp
{
    enum class shader_profile
    {
        core = 0,
        compat
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

    struct processed_file
    {
        // GLSL Shader version in integral form (e.g. 450) or 0 if no version tag is found.
        uint32_t version{0};

        // GLSL Shader profile (core or compatibility) or core if none specified.
        shader_profile profile{shader_profile::core};

        // The original path of the loaded file.
        std::experimental::filesystem::path file_path;

        // All files included while loading the shader.
        std::set<std::experimental::filesystem::path> dependencies;

        // All explicitly enabled/required glsl extensions. 
        // Maps to a bool which shows whether the extension is required (false if only "enabled").
        std::map<std::string, bool> extensions;

        // All definitions which have been defined in the shader (and not undefined after that).
        // Name maps to replacement + parameter pack.
        std::map<std::string, definition_info> definitions;

        // The fully expanded and parsed shader code string.
        std::string contents;
    };

    // Loads a shader file and decodes it.
    // include_directories give the parser a hint on where to look for included files.
    // You can also add pre-defined definitions.
    // 
    // All OpenGL extensions on the system are automatically added.
    processed_file load_file(const std::experimental::filesystem::path& file_path, 
        const std::vector<std::experimental::filesystem::path>& include_directories, 
        const std::vector<definition>& definitions);
}
