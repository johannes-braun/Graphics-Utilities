#include "../shader.hpp"
#include <array>
#include <jpu/log.hpp>

namespace gl
{
    std::vector<std::experimental::filesystem::path> shader::_include_directories = { "../shaders" };

    void shader::reload(const std::vector<glshader::definition>& definitions)
    {
        _definitions = definitions;
        reload(true);
    }

    shader_type shader::type_of(const std::experimental::filesystem::path& extension)
    {
        if (extension == ".vert")
            return shader_type::vertex;
        if (extension == ".frag")
            return shader_type::fragment;
        if (extension == ".geom")
            return shader_type::geometry;
        if (extension == ".tesc")
            return shader_type::tesselation_control;
        if (extension == ".tese")
            return shader_type::tesselation_evaluation;
        if (extension == ".comp")
            return shader_type::compute;

        throw std::invalid_argument("File extension " + extension.string() + " does not refer to any shader type.");
    }

    void shader::reload(bool force) noexcept
    {
        tlog_i("GL Shader") << "Load shader from " << _path;

        try
        {
            auto bin = load_binary_shader(glshader::shader_format::gl_binary, _path, _include_directories, _definitions);
            if (glIsProgram(_id))
                glDeleteProgram(_id);
            _id = glCreateProgram();
            glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glProgramBinary(_id, GLenum(bin.format), bin.data.data(), static_cast<int>(bin.data.size()));
        }
        catch (const std::runtime_error& exception)
        {
            tlog_e("GL Shader") << "Shader compilation failed: " << exception.what();
        }

        _uniforms.clear();
    }

    void shader::set_include_directories(const std::vector<std::experimental::filesystem::path> include_directories)
    {
        _include_directories = include_directories;
    }

    void shader::set_include_directories(std::experimental::filesystem::path include_directories)
    {
        _include_directories = { include_directories };
    }

    shader::shader(const std::experimental::filesystem::path& path, const std::vector<glshader::definition>& definitions)
        : _type(type_of(path.extension())), _path(path), _definitions(definitions)
    {
        if(!_path.is_absolute())
        {
            for(auto&& inc : _include_directories)
            {
                if(exists(inc / _path))
                {
                    _path = inc / _path;
                    break;
                }
            }
        }
        reload(true);
    }

    shader::~shader() noexcept
    {
        glDeleteProgram(_id);
    }

    shader::operator gl_shader_program_t() const noexcept
    {
        return _id;
    }

    shader_type shader::type() const noexcept
    {
        return _type;
    }
}
