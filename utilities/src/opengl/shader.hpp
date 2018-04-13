#pragma once

#include "uniform.hpp"
#include "gl.hpp"

#include <vector>
#include <filesystem>
#include <string_view>
#include <any>
#include <map>
#include <glsp/glsp.hpp>

namespace gl
{
    namespace files = std::experimental::filesystem;
    using glsp::definition;

    constexpr const char* opengl_prefix = R"glsl(
        #version 450 core
        #extension GL_ARB_separate_shader_objects : require
        #extension GL_ARB_bindless_texture : require
        #extension GL_ARB_shading_language_420pack : require
        #extension GL_NV_gpu_shader5 : require
    )glsl";

    constexpr const char* opengl_postfix = R"glsl(
        layout(bindless_sampler) uniform;
        layout(bindless_image) uniform;
    )glsl";

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