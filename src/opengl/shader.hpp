#pragma once

#include "uniform.hpp"
#include <mygl/mygl.hpp>

#include <vector>
#include <string_view>
#include <map>
#include <unordered_set>
#include <mutex>
#include <glsp/glsp.hpp>
#include <filesystem>

namespace gl
{
    namespace files = std::filesystem;
    using glsp::definition;

    constexpr const char* opengl_prefix = R"glsl(
        #version 460 core
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
        static void set_include_directory(files::path include_directories);
        static void set_include_directories(std::vector<files::path> include_directories);

        explicit shader(const files::path& path, const std::vector<definition>& definitions = {});
        explicit shader(const std::string& source, const std::string& name, GLenum type, const std::vector<definition>& definitions ={});
        shader(const shader& other) noexcept;
        shader(shader&& other) noexcept;
        shader& operator=(const shader& other) noexcept;
        shader& operator=(shader&& other) noexcept;
        ~shader() noexcept;

        void reload(const std::vector<definition>& definitions);
        void reload();

        GLenum type() const noexcept;
        operator mygl::shader_program() const noexcept;

        template<typename T>
        uniform<T> uniform(const std::string& name);

    private:
        static std::vector<files::path> _include_directories;

        mygl::shader_program _id = mygl::shader_program::zero;
        GLenum _type;
        files::path _path;
        std::map<std::string, all_uniform_types> _uniforms;
        std::vector<definition> _definitions;
        bool _enable_reload = false;
    };
}

#include "impl/shader.inl"