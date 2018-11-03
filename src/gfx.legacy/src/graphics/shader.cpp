#include <graphics/shader.hpp>
#include <context.hpp>
#include "shader_include.hpp"
#include <glsp/glsp.hpp>
#include <fstream>

namespace gfx {
inline namespace v1 {
shader::shader(shader_type stage, const gsl::span<const uint32_t>& spirv) : _stage(stage)
{
	_data.resize(spirv.size_bytes());
	memcpy(_data.data(), spirv.data(), spirv.size_bytes());
}

shader::shader(shader_type stage, const std::filesystem::path& path) : _stage(stage), _path(path)
{
    std::string gapi_str;
    bool        enable_text = false;
    switch (gfx::context::current()->options().graphics_api)
    {
    case gfx::gapi::opengl:
        gapi_str    = ".gl.spv";
        enable_text = true;
        break;
    case gfx::gapi::vulkan: gapi_str = ".vk.spv"; break;
    }
    std::filesystem::path combined  = (path.string() + gapi_str);
    std::filesystem::path text_file = path.string();

    bool path_valid = path.is_absolute();
    if (!path_valid)
        for (auto&& inc : gfx::shader_includes::directories) {
            if (exists(inc / combined) && !path_valid) {
                _format    = gfx::shader_format::spirv;
                _path      = inc / combined;
                path_valid = true;
            }
            else if (exists(inc / path)) {
                text_file = inc / path;
                if (enable_text && !path_valid) {
                    _format    = gfx::shader_format::text;
                    _path      = text_file;
                    path_valid = true;
                }
            }
            if (path_valid) break;
        }

    if (!path_valid) {
        elog << "Shader not found: " << path;
        return;
    }

    switch (_format)
    {
    case shader_format::text:
    {
        std::vector<glsp::files::path> includes;
        for (const auto& p : shader_includes::directories) includes.push_back(p.string());
        const auto ppf = glsp::preprocess_file(_path.string(), includes);
        _data.resize(ppf.contents.size() + 1);
        memcpy(_data.data(), ppf.contents.data(), ppf.contents.size() * sizeof(char));
        _data.back() = std::byte(0);
    }
    break;
    case shader_format::spirv:
    {
        std::ifstream in(_path, std::ios::binary | std::ios::in);
        if (in) {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);
            _data.resize(size);
            in.read(reinterpret_cast<char*>(_data.data()), size);
        }
    }
    break;
    }

    if (exists(text_file) && path_valid && _format != shader_format::text) {
        const auto text_stamp = last_write_time(text_file);
        const auto main_stamp = last_write_time(_path);

        if (text_stamp > main_stamp) {
            wlog << "Found raw source for " << text_file << ", which has been edited after compiling to SPIR-V (different time stamp).";
        }
    }
}

const std::vector<std::byte>& shader::data() const noexcept
{
    return _data;
}

shader_format shader::format() const noexcept
{
    return _format;
}
}    // namespace v1
}    // namespace gfx