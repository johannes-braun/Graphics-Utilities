#pragma once

#include "gl.hpp"

#include <functional>

namespace gl
{
    enum class debug_source
    {
        api = GL_DEBUG_SOURCE_API,
        window_system = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
        shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
        third_party = GL_DEBUG_SOURCE_THIRD_PARTY,
        application = GL_DEBUG_SOURCE_APPLICATION
    };

    enum class debug_type
    {
        error = GL_DEBUG_TYPE_ERROR,
        deprecated_behavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
        undefined_behavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
        portability = GL_DEBUG_TYPE_PORTABILITY,
        performance = GL_DEBUG_TYPE_PERFORMANCE,
        marker = GL_DEBUG_TYPE_MARKER,
        push_group = GL_DEBUG_TYPE_PUSH_GROUP,
        pop_group = GL_DEBUG_TYPE_POP_GROUP,
        other = GL_DEBUG_TYPE_OTHER,
    };

    enum class debug_severity
    {
        high = GL_DEBUG_SEVERITY_HIGH,
        medium = GL_DEBUG_SEVERITY_MEDIUM,
        low = GL_DEBUG_SEVERITY_LOW,
        notification = GL_DEBUG_SEVERITY_NOTIFICATION
    };

    const char* get_debug_enum_desc(debug_source e) noexcept;
    const char* get_debug_enum_desc(debug_type e) noexcept;
    const char* get_debug_enum_desc(debug_severity e) noexcept;

    using debug_function = std::function<void(debug_source source, debug_type type, uint32_t id, debug_severity severity, const char* message)>;
    void set_debug_callback(debug_function function) noexcept;

    void set_debug_callback_enabled(debug_source d, bool enable) noexcept;
    void set_debug_callback_enabled(debug_type d, bool enable) noexcept;
    void set_debug_callback_enabled(debug_severity d, bool enable) noexcept;
    void set_debug_callback_enabled(uint32_t* ids, size_t count, bool enable) noexcept;

    inline void set_debug_callback_enabled(std::vector<uint32_t> ids, const bool enable) noexcept { set_debug_callback_enabled(ids.data(), ids.size(), enable); }
    template<size_t I> void set_debug_callback_enabled(std::array<uint32_t, I> ids, const bool enable) noexcept { set_debug_callback_enabled(ids.data(), ids.size(), enable); }
}