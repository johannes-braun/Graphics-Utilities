#pragma once

#include "uniform.hpp"

#include <mygl/gl.hpp>
#include <filesystem>
#include <jpu/memory.hpp>
#include <any>
#include <map>
#include <glshader/binary.hpp>

namespace gl
{
    enum class shader_type
    {
        vertex = GL_VERTEX_SHADER,
        fragment = GL_FRAGMENT_SHADER,
        geometry = GL_GEOMETRY_SHADER,
        tesselation_evaluation = GL_TESS_EVALUATION_SHADER,
        tesselation_control = GL_TESS_CONTROL_SHADER,
        compute = GL_COMPUTE_SHADER,
    };

    class shader : public jpu::ref_count
    {
    public:
        static void set_include_directories(std::vector<std::experimental::filesystem::path> include_directories);
        static void set_include_directories(std::experimental::filesystem::path include_directories);

        explicit shader(const std::experimental::filesystem::path& path, const std::vector<glshader::definition>& definitions = {});

        ~shader() noexcept;
        operator gl_shader_program_t() const noexcept;
        shader_type type() const noexcept;

        void reload(bool force = false) noexcept;
        void reload(const std::vector<glshader::definition>& definitions);

        template<typename T>
        uniform<T> get_uniform(std::string_view name);

    private:
        static shader_type type_of(const std::experimental::filesystem::path& extension) noexcept;
        static std::vector<std::experimental::filesystem::path> _include_directories;

        gl_shader_program_t _id;
        shader_type _type;
        std::map<std::string_view, all_uniform_types> _uniforms;
        std::experimental::filesystem::path _path;
        std::vector<glshader::definition> _definitions;
    };
}

#include "shader.inl"