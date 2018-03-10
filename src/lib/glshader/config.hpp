#pragma once


#if __has_include(<mygl/gl.hpp>)
#   include <mygl/gl.hpp>
#else
#   if __has_include(<GL/glew.h>)
#       include <GL/glew.h>
#   elif __has_include(<glad/glad.h>)
#       include <glad/glad.h>
#   elif __has_include(<glbinding/gl/gl.h>)
#       include <glbinding/gl/gl.h>
#   endif
#endif

#if __has_include(<jpu/log>)
#   include <jpu/log>
#   define LOG_OUTPUT log_i
#else
#   include <iostream>
#   define LOG_OUTPUT std::cout
#endif

namespace glshader
{
    constexpr static const char* shader_cache_dir = "../cache/shaders";

    constexpr const char* opengl_prefix = R"glsl(
        #version 450 core
        #extension GL_ARB_separate_shader_objects : require
        #extension GL_ARB_bindless_texture : require
        #extension GL_ARB_shading_language_420pack : require
        #extension GL_NV_shader_buffer_load : require
    )glsl";

    constexpr const char* opengl_postfix = R"glsl(
        layout(bindless_sampler) uniform;
        layout(bindless_image) uniform;
    )glsl";

    constexpr static const char* glslc_default_flags = " -std=450core ";
    constexpr static const char* glslc_location = "glslc";
    constexpr static const char* glslc_temp_file_name = "shader_temp";
}
