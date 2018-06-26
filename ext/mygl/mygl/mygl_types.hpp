#pragma once
/* This header contains the needed basic types for OpenGL.
INFO: the GLenum type is defined externally in mygl_enums.hpp.
*/

#include "mygl_enums.hpp"
#include <cinttypes>
#include <type_traits>
typedef void *GLeglClientBufferEXT;
typedef void *GLeglImageOES;
typedef struct __GLsync *GLsync;
typedef void ( *GLDEBUGPROC)(GLenum source,GLenum type,unsigned int id,GLenum severity,int length,const char *message,const void *userParam);
typedef void ( *GLDEBUGPROCARB)(GLenum source,GLenum type,unsigned int id,GLenum severity,int length,const char *message,const void *userParam);
typedef void ( *GLDEBUGPROCKHR)(GLenum source,GLenum type,unsigned int id,GLenum severity,int length,const char *message,const void *userParam);
typedef void ( *GLDEBUGPROCAMD)(unsigned int id,GLenum category,GLenum severity,int length,const char *message,void *userParam);
typedef void ( *GLVULKANPROCNV)(void);
using GLboolean = bool;
using GLintptr         = intptr_t;
using GLsizeiptr       = ptrdiff_t;
using GLvdpauSurfaceNV = intptr_t;
using GLhalfNV = unsigned short;
using GLint64EXT = int64_t;
using GLsizeiptrARB = ptrdiff_t;
using GLintptrARB = ptrdiff_t;
using GLuint64 = uint64_t;
using GLhalfARB = unsigned short;
using GLfixed = int;
using GLvoid = void;
using GLbyte = signed char;
using GLint = int;
using GLsizei = int;
using GLclampx = int;
using GLchar = char;
using GLubyte = unsigned char;
using GLushort = unsigned short;
using GLuint = unsigned int;
using GLhalf = unsigned short;
using GLfloat = float;
using GLclampf = float;
using GLint64 = int64_t;
using GLdouble = double;
using GLuint64EXT = uint64_t;
using GLshort = short;
using GLclampd = double;
using GLcharARB = char;

//custom rule types:
namespace mygl{
    enum class buffer : uint32_t { zero = 0 };
    enum class shader_program : uint32_t { zero = 0 };
    enum class shader : uint32_t { zero = 0 };
    enum class query : uint32_t { zero = 0 };
    enum class framebuffer : uint32_t { zero = 0 };
    enum class texture : uint32_t { zero = 0 };
    enum class renderbuffer : uint32_t { zero = 0 };
    enum class sampler : uint32_t { zero = 0 };
    enum class pipeline : uint32_t { zero = 0 };
    enum class vertex_array : uint32_t { zero = 0 };
    enum class command_list : uint32_t { zero = 0 };
    enum class state : uint32_t { zero = 0 };
    enum class path : uint32_t { zero = 0 };
    enum class transform_feedback : uint32_t { zero = 0 };
}
