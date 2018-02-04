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

    shader::shader(shader_type type, const std::experimental::filesystem::path& path, const std::vector<res::definition>& definitions)
        : _type(type), _path(path), _definitions(definitions)
    {
        reload(true);
    }

    void shader::reload(const std::vector<res::definition>& definitions)
    {
        _definitions = definitions;
        reload(true);
    }

    void shader::reload(bool force)
    {
        log_i << "Load shader from " << _path;

        if (glIsProgram(_id))
            glDeleteProgram(_id);
        _id = glCreateProgram();
        glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, GL_TRUE);

        auto bin = res::load_binary_shader(res::shader_format::gl_binary, _path, shader_include_directories, _definitions);
        glProgramBinary(_id, bin.format, bin.data.data(), static_cast<int>(bin.data.size()));

        _uniforms.clear();
    }

    shader::shader(const std::experimental::filesystem::path& path, const std::vector<res::definition>& definitions)
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
