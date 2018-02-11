#pragma once

#include "glad/glad.h"

namespace glshader
{
    constexpr static const char* shader_cache_dir = "../cache/shaders";

    constexpr const char* opengl_prefix = R"glsl(
        #version 450 core
        #extension GL_ARB_separate_shader_objects : require
        #extension GL_ARB_bindless_texture : require
        #extension GL_NV_shader_buffer_load : require
        #extension GL_ARB_shading_language_420pack : require
    )glsl";

    constexpr const char* opengl_postfix = R"glsl(
        layout(bindless_sampler) uniform;
        layout(bindless_image) uniform;
    )glsl";

    constexpr static const char* glslc_default_flags = " -std=450core ";
    constexpr static const char* glslc_location = "glslc.exe";
    constexpr static const char* glslc_temp_file_name = "shader_temp";
}