#include "shader.hpp"
#include <array>
#include <jpu/log>

namespace gl
{
    void setup_shader_paths(const std::experimental::filesystem::path root,
                            const std::vector<std::experimental::filesystem::path>& include_directories)
    {
        shader_root_path = root;
        shader_include_directories.clear();
        shader_include_directories.push_back(root);
        shader_include_directories.insert(shader_include_directories.end(), include_directories.begin(),
                                          include_directories.end());
    }

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

    void shader::reload(bool force)
    {
        tlog_i("GL Shader") << "Load shader from " << _path;

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
            tlog_e("GL Shader") << "Shader compilation failed: " << exception.what();
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
