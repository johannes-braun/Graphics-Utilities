#include "shader.hpp"
#include <array>
#include <jpu/log>

namespace gl
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

    shader::shader(shader_type type, const std::experimental::filesystem::path& path, const std::vector<glpp::definition>& definitions)
        : _type(type), _path(path), _definitions(definitions)
    {
        reload(true);
    }

    void shader::reload(const std::vector<glpp::definition>& definitions)
    {
        _definitions = definitions;
        reload(true);
    }

    void shader::reload(bool force)
    {
        bool changed = false;
        for (const auto& dep_time : _dependency_change_times)
        {
            if (dep_time.second != std::experimental::filesystem::last_write_time(dep_time.first))
            {
                changed = true;
                break;
            }
        }
        if (!changed && !force)
        {
            log_i << "Not reloading shader at " << _path << ", because it's up-to-date.";
            return;
        }

        _dependency_change_times.clear();

        log_i << "Loading with new shader id! From " << _path;
        auto file = glpp::load_file(_path, shader_include_directories, _definitions);

        _dependency_change_times[_path] = std::experimental::filesystem::last_write_time(_path);
        for (const auto dep : file.dependencies)
            _dependency_change_times[dep] = std::experimental::filesystem::last_write_time(dep);

        const std::array<const char*, 3> strings
        {
            default_prefix,
            file.contents.c_str(),
            default_postfix
        };

        if (glIsProgram(_id))
            glDeleteProgram(_id);
        _id = glCreateShaderProgramv(static_cast<GLenum>(_type), static_cast<uint32_t>(std::size(strings)), std::data(strings));

        if (int success = 0; glGetProgramiv(_id, GL_LINK_STATUS, &success), !success)
        {
            int log_length;
            glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramInfoLog(_id, log_length, &log_length, log.data());
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, -1, log.c_str());

             throw std::runtime_error("Program linking failed: " + log);
        }

        _uniforms.clear();
    }

    shader::shader(const std::experimental::filesystem::path& path, const std::vector<glpp::definition>& definitions)
        : shader(type_of(path.extension()), path, definitions)
    {

    }

    shader::~shader()
    {
        glDeleteProgram(_id);
    }

    shader::operator bool() const
    {
        return _id != 0;
    }

    shader::operator unsigned() const
    {
        return _id;
    }

    shader_type shader::type() const
    {
        return _type;
    }
}
