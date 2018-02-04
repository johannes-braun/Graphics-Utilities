#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace xspirv
{
    constexpr static uint32_t file_version = 100;
    constexpr static const char* file_extension = ".xspv";
    constexpr static const char* default_flags = " -std=450core ";
    constexpr static const char* glslc_location = "glslc.exe";
    constexpr static const char* temp_file_name = "xspirv_temp_comp";

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

    std::vector<uint32_t> load_file(const std::experimental::filesystem::path& file_path,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions);
}
