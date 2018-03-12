#pragma once

#if __has_include(<mygl/gl.hpp>)
#   include <mygl/gl.hpp>
#else
#   if __has_include(<GL/glew.h>)
#       include <gl/glew.h>
#   elif __has_include(<glad/glad.h>)
#       include <glad/glad.h>
#   elif __has_include(<glbinding/gl/gl.h>)
#       include <glbinding/gl/gl.h>
#   endif
using gl_texture_t = GLuint;
using gl_buffer_t = GLuint;
using gl_framebuffer_t = GLuint;
using gl_sampler_t = GLuint;
using gl_program_pipeline_t = GLuint;
using gl_shader_program_t = GLuint;
using gl_vertex_array_t = GLuint;
#endif