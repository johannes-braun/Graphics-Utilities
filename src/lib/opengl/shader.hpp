#pragma once

#include "uniform.hpp"
#include "gl.hpp"

#include <vector>
#include <filesystem>
#include <string_view>
#include <any>
#include <map>
#include <glshader/binary.hpp>
#include <jpu/memory.hpp>

namespace gl
{
    namespace files = std::experimental::filesystem;
    using glshader::definition;

    class shader
    {
    public:
        static void set_include_directories(files::path include_directories);
        static void set_include_directories(std::vector<files::path> include_directories);

        explicit shader(const files::path& path, const std::vector<definition>& definitions = {});
        shader(const shader& other) noexcept;
        shader(shader&& other) noexcept;
        shader& operator=(const shader& other) noexcept;
        shader& operator=(shader&& other) noexcept;
        ~shader() noexcept;

        void reload(const std::vector<definition>& definitions);
        void reload();

        GLenum type() const noexcept;
        operator gl_shader_program_t() const noexcept;

        template<typename T>
        uniform<T> uniform(const std::string& name);

    private:
        static std::vector<files::path> _include_directories;

        gl_shader_program_t _id = gl_shader_program_t::zero;
        GLenum _type;
        files::path _path;
        std::map<std::string, all_uniform_types> _uniforms;
        std::vector<definition> _definitions;
    };
}

#include "impl/shader.inl"