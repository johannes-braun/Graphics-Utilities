#pragma once
/* This header contains the needed basic types for OpenGL.
INFO: the GLenum type is defined externally in mygl_enums.hpp.
*/

#include "mygl_enums.hpp"
#include <cinttypes>
#include <type_traits>
#include "KHR/khrplatform.h"
//Define a couple of GL types for compatibility.
using GLboolean = bool;
using GLhalfNV = unsigned short;
using GLubyte = khronos_uint8_t;
using GLshort = khronos_int16_t;
using GLvoid = void;
using GLbyte = khronos_int8_t;
using GLcharARB = char;
using GLushort = khronos_uint16_t;
using GLint = int;
using GLintptrARB = khronos_intptr_t;
using GLhalfARB = khronos_uint16_t;
using GLeglClientBufferEXT = void *;
using GLuint = unsigned int;
using GLsizeiptrARB = khronos_ssize_t;
using GLclampx = khronos_int32_t;
using GLsizei = int;
using GLfloat = khronos_float_t;
using GLclampf = khronos_float_t;
using GLclampd = double;
using GLdouble = double;
using GLeglImageOES = void *;
using GLchar = char;
using GLuint64EXT = khronos_uint64_t;
using GLintptr = khronos_intptr_t;
using GLhalf = khronos_uint16_t;
using GLfixed = khronos_int32_t;
using GLsizeiptr = khronos_ssize_t;
using GLsync = struct __GLsync *;
using GLint64 = khronos_int64_t;
using GLint64EXT = khronos_int64_t;
using GLuint64 = khronos_uint64_t;
using GLvdpauSurfaceNV = khronos_intptr_t;

//All internal function pointer types
using GLVULKANPROCNV = void (*) (void);
using GLDEBUGPROCAMD = void (*) (GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar *message, void *userParam);
using GLDEBUGPROC = void (*) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
using GLDEBUGPROCARB = void (*) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
using GLDEBUGPROCKHR = void (*) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

//custom rule types:
namespace mygl{
    template<typename Ident>
    struct basic_handle
    {
        using identifier_type = Ident;
        using handle_type = std::underlying_type_t<identifier_type>;
        using value_type = handle_type;

        constexpr static basic_handle zero() noexcept;
        constexpr static basic_handle from(handle_type h) noexcept;

        constexpr operator handle_type() const noexcept { return handle; }
        constexpr operator bool() const noexcept { return handle != 0; }
        constexpr bool operator ==(basic_handle other) const noexcept { return handle == other.handle; }
        constexpr bool operator !=(basic_handle other) const noexcept { return handle != other.handle; }
        
        handle_type handle;
    };

    template<typename Ident>
    constexpr basic_handle<Ident> basic_handle<Ident>::zero() noexcept
    {
        return from(0);
    }

    template<typename Ident>
    constexpr basic_handle<Ident> basic_handle<Ident>::from(handle_type h) noexcept
    {
        return basic_handle<Ident>{ static_cast<handle_type>(h) };
    }
    namespace ident { enum class buffer_identifier : uint32_t {  }; }
    using buffer = basic_handle<ident::buffer_identifier>;
    namespace ident { enum class shader_program_identifier : uint32_t {  }; }
    using shader_program = basic_handle<ident::shader_program_identifier>;
    namespace ident { enum class shader_identifier : uint32_t {  }; }
    using shader = basic_handle<ident::shader_identifier>;
    namespace ident { enum class query_identifier : uint32_t {  }; }
    using query = basic_handle<ident::query_identifier>;
    namespace ident { enum class framebuffer_identifier : uint32_t {  }; }
    using framebuffer = basic_handle<ident::framebuffer_identifier>;
    namespace ident { enum class texture_identifier : uint32_t {  }; }
    using texture = basic_handle<ident::texture_identifier>;
    namespace ident { enum class renderbuffer_identifier : uint32_t {  }; }
    using renderbuffer = basic_handle<ident::renderbuffer_identifier>;
    namespace ident { enum class sampler_identifier : uint32_t {  }; }
    using sampler = basic_handle<ident::sampler_identifier>;
    namespace ident { enum class pipeline_identifier : uint32_t {  }; }
    using pipeline = basic_handle<ident::pipeline_identifier>;
    namespace ident { enum class vertex_array_identifier : uint32_t {  }; }
    using vertex_array = basic_handle<ident::vertex_array_identifier>;
    namespace ident { enum class command_list_identifier : uint32_t {  }; }
    using command_list = basic_handle<ident::command_list_identifier>;
    namespace ident { enum class state_identifier : uint32_t {  }; }
    using state = basic_handle<ident::state_identifier>;
    namespace ident { enum class path_identifier : uint32_t {  }; }
    using path = basic_handle<ident::path_identifier>;
    namespace ident { enum class transform_feedback_identifier : uint32_t {  }; }
    using transform_feedback = basic_handle<ident::transform_feedback_identifier>;
}
