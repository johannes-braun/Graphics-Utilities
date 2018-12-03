#pragma once
/* This header contains the needed basic types for OpenGL.
INFO: the GLenum type is defined externally in mygl_enums.hpp.
*/

#include "mygl_enums.hpp"
#include <cinttypes>
#include <type_traits>
#if __has_include("KHR/khrplatform.h")
#include "KHR/khrplatform.h"
#endif

//All internal function pointer types
using GLVULKANPROCNV = void (*) (void);
using GLDEBUGPROCAMD = void (*) (std::uint32_t id, gl_enum category, gl_enum severity, std::int32_t length, const char *message, void *userParam);
using GLDEBUGPROC = void (*) (gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char *message, const void *userParam);
using GLDEBUGPROCARB = void (*) (gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char *message, const void *userParam);
using GLDEBUGPROCKHR = void (*) (gl_enum source, gl_enum type, std::uint32_t id, gl_enum severity, std::int32_t length, const char *message, const void *userParam);

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
    namespace ident { enum class buffer_identifier : std::uint32_t {  }; }
    using buffer = basic_handle<ident::buffer_identifier>;
    namespace ident { enum class shader_program_identifier : std::uint32_t {  }; }
    using shader_program = basic_handle<ident::shader_program_identifier>;
    namespace ident { enum class shader_identifier : std::uint32_t {  }; }
    using shader = basic_handle<ident::shader_identifier>;
    namespace ident { enum class query_identifier : std::uint32_t {  }; }
    using query = basic_handle<ident::query_identifier>;
    namespace ident { enum class framebuffer_identifier : std::uint32_t {  }; }
    using framebuffer = basic_handle<ident::framebuffer_identifier>;
    namespace ident { enum class texture_identifier : std::uint32_t {  }; }
    using texture = basic_handle<ident::texture_identifier>;
    namespace ident { enum class renderbuffer_identifier : std::uint32_t {  }; }
    using renderbuffer = basic_handle<ident::renderbuffer_identifier>;
    namespace ident { enum class sampler_identifier : std::uint32_t {  }; }
    using sampler = basic_handle<ident::sampler_identifier>;
    namespace ident { enum class pipeline_identifier : std::uint32_t {  }; }
    using pipeline = basic_handle<ident::pipeline_identifier>;
    namespace ident { enum class vertex_array_identifier : std::uint32_t {  }; }
    using vertex_array = basic_handle<ident::vertex_array_identifier>;
    namespace ident { enum class command_list_identifier : std::uint32_t {  }; }
    using command_list = basic_handle<ident::command_list_identifier>;
    namespace ident { enum class state_identifier : std::uint32_t {  }; }
    using state = basic_handle<ident::state_identifier>;
    namespace ident { enum class path_identifier : std::uint32_t {  }; }
    using path = basic_handle<ident::path_identifier>;
    namespace ident { enum class transform_feedback_identifier : std::uint32_t {  }; }
    using transform_feedback = basic_handle<ident::transform_feedback_identifier>;
    namespace ident { enum class sync_identifier : std::uint64_t {  }; }
    using sync = basic_handle<ident::sync_identifier>;
}
