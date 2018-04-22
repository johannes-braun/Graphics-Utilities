#include "../shader.hpp"
#include <array>
#include <gfx/log.hpp>

namespace gl
{
    std::vector<files::path> shader::_include_directories = { "../shaders" };
    glsp::compiler& compiler()
    {
        static glsp::compiler c = []() {
            glsp::compiler compiler(".gbs", "../cache/shaders");
            compiler.set_default_prefix(opengl_prefix);
            compiler.set_default_postfix(opengl_postfix);
            return compiler;
        }();
        return c;
    }

    void shader::set_include_directories(const std::vector<files::path> include_directories)
    {
        _include_directories = include_directories;
    }

    void shader::set_include_directory(files::path include_directories)
    {
        _include_directories = { include_directories };
    }


    shader::shader(const std::string& source, const std::string& name, const GLenum type, const std::vector<definition>& definitions)
        : _type(type), _definitions(definitions), _enable_reload(false)
    {
        glsp::processed_file processed = glsp::preprocess_source(source, name, _include_directories, definitions);

        const char* sources[3] = {
            opengl_prefix,
            processed.contents.c_str(),
            opengl_postfix
        };

        if (!processed)
        {
            log_e << "Failed to process source for " << name;
            return;
        }

        _id = glCreateShaderProgramv(type, 3, sources);
        int success = 0;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            int log_length;
            glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramInfoLog(_id, log_length, &log_length, log.data());
            glDeleteProgram(_id);
            log_e << "Failed to compile source for " << name;
            return;
        }
    }

    shader::shader(const files::path& path, const std::vector<definition>& definitions)
        : _path(path), _definitions(definitions), _enable_reload(true)
    {   
        bool path_valid = _path.is_absolute();
        if (!path_valid)
            for (auto&& inc : _include_directories)
                if (files::exists(inc / _path))
                {
                    _path = inc / _path;
                    path_valid = true;
                    break;
                }
        if (!path_valid)
            log_e << "Shader not found: " << path;

        files::path extension = path.extension();
        if (extension == ".comp")
            _type = GL_COMPUTE_SHADER;
        else if (extension == ".vert")
            _type = GL_VERTEX_SHADER;
        else if (extension == ".frag")
            _type = GL_FRAGMENT_SHADER;
        else if (extension == ".geom")
            _type = GL_GEOMETRY_SHADER;
        else if (extension == ".tesc")
            _type = GL_TESS_CONTROL_SHADER;
        else if (extension == ".tese")
            _type = GL_TESS_EVALUATION_SHADER;

        reload();
    }

    shader::shader(const shader& other) noexcept
    {
        operator=(std::forward<const shader&>(other));
    }

    shader::shader(shader&& other) noexcept
    {
        operator=(std::forward<shader&&>(other));
    }

    shader& shader::operator=(const shader& other) noexcept
    {
        _path = other._path;
        _definitions = other._definitions;
        _type = other._type;
        reload();
        return *this;
    }

    shader& shader::operator=(shader&& other) noexcept
    {
        if (_id != gl_shader_program_t::zero)
            glDeleteProgram(_id);

        _id = other._id;
        _path = std::move(other._path);
        _definitions = std::move(other._definitions);
        _type = other._type;
        _uniforms = std::move(other._uniforms);

        other._id = gl_shader_program_t::zero;
        other._type = GL_ZERO;
        return *this;
    }

    shader::~shader() noexcept
    {
        if (_id != gl_shader_program_t::zero)
            glDeleteProgram(_id);
    }

    void shader::reload()
    {
        do
        {
            glsp::shader_binary bin = compiler().compile(_path, glsp::format::gl_binary, false, _include_directories, _definitions);
            if(!bin.data.empty())
            {
                if (_id != gl_shader_program_t::zero)
                    glDeleteProgram(_id);
                _id = glCreateProgram();
                glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
                glProgramBinary(_id, GLenum(bin.format), bin.data.data(), static_cast<int>(bin.data.size()));
            }
            else
            {
                if (_id == gl_shader_program_t::zero)
                {
                    tlog_h("GL Shader") << "Press [ENTER] to try again...";
                    std::cin.get();
                }
            }
        } while (_id == gl_shader_program_t::zero);

        _uniforms.clear();
    }

    void shader::reload(const std::vector<definition>& definitions)
    {
        _definitions = definitions;
        reload();
    }

    shader::operator gl_shader_program_t() const noexcept
    {
        return _id;
    }

    GLenum shader::type() const noexcept
    {
        return _type;
    }
}
