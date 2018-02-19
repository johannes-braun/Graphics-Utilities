#include "shader.hpp"
#include <array>
#include <jpu/log>

namespace gl
{
    shader::shader(const shader_type type, const std::experimental::filesystem::path& path, const std::vector<glshader::definition>& definitions)
        : _type(type), _path(path), _definitions(definitions)
    {
        reload(true);
    }

    void shader::reload(const std::vector<glshader::definition>& definitions)
    {
        _definitions = definitions;
        reload(true);
    }

    void shader::reload(bool force)
    {
        log_i << "Load shader from " << _path;

        try
        {
            auto bin = load_binary_shader(glshader::shader_format::gl_binary, _path, shader_include_directories, _definitions);
            if (glIsProgram(_id))
                glDeleteProgram(_id);
            _id = glCreateProgram();
            glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glProgramBinary(_id, bin.format, bin.data.data(), static_cast<int>(bin.data.size()));
        }
        catch (const std::runtime_error& exception)
        {
            tlog_e("GL Shader") << "Shader compilation failed.";
        }

        _uniforms.clear();
    }

    shader::shader(const std::experimental::filesystem::path& path, const std::vector<glshader::definition>& definitions)
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
