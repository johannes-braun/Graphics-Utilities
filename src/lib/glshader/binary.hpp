#pragma once

#include <jpu/log.hpp>
#define LOG_OUTPUT(x) tlog_i("GLShader") << (x)
#define ERR_OUTPUT(x) tlog_e("GLShader") << (x)

#include <mygl/gl.hpp>
#include <glsp/definition.hpp>
#include <glsp/glsp.hpp>

#include <vector>
#include <filesystem>

namespace glshader
{
    constexpr static const char* shader_cache_dir = "../cache/shaders";

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

    constexpr static const char* glslc_default_flags = " -std=450core ";
    constexpr static const char* glslc_location = "glslc";
    constexpr static const char* glslc_temp_file_name = "shader_temp";
}


namespace glshader
{
    constexpr uint32_t make_tag(const char name[4])
    {
        return (name[0] << 0) | (name[1] << 8) | (name[2] << 16) | (name[3] << 24);
    }

    enum class shader_format : uint32_t
    {
        gl_binary = make_tag("GBIN"),
        spirv = make_tag("SPRV")
    };
     
    struct binary_shader
    {
        shader_format type;
        uint32_t format;
        std::vector<uint8_t> data;
    };

    binary_shader load_binary_shader(shader_format type,
        const std::experimental::filesystem::path& src,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<glsp::definition>& definitions,
        bool force = false);

    struct shader_binary
    {
        uint32_t format;
        std::vector<uint8_t> data;
    };

    class compiler : public glsp::state
    {
    public:
        compiler(const std::string& extension, const glsp::files::path& cache_dir);
        void set_extension(const std::string& ext);
        void set_cache_dir(const glsp::files::path& dir);
        void set_default_prefix(const std::string& prefix);
        void set_default_postfix(const std::string& postfix);

        void compile(const glsp::files::path& shader, shader_format format, const std::vector<glsp::files::path>& includes ={}, const std::vector<glsp::definition>& definitions ={});

    private:
        std::string _default_prefix;
        std::string _default_postfix;
        std::string _extension;
        glsp::files::path _cache_dir;
    };

}
