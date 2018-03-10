#include "debug.hpp"

namespace gl
{
    const char* get_debug_enum_desc(const debug_source e) noexcept
    {
        switch (e) {
        case debug_source::api: return "API";
        case debug_source::window_system: return "Window System";
        case debug_source::shader_compiler: return "Shader Compiler";
        case debug_source::third_party: return "Third Party";
        case debug_source::application: return "Application";
        default: return "Other";
        }
    }

    const char* get_debug_enum_desc(const debug_type e) noexcept
    {
        switch (e) {
        case debug_type::error: return "Error";
        case debug_type::deprecated_behavior: return "Deprecated Behavior";
        case debug_type::undefined_behavior: return "Undefined Behavior";
        case debug_type::portability: return "Portability";
        case debug_type::performance: return "Performance";
        case debug_type::marker: return "Marker";
        case debug_type::push_group: return "Push Group";
        case debug_type::pop_group: return "Pop Group";
        default: return "Other";
        }
    }

    const char* get_debug_enum_desc(const debug_severity e) noexcept
    {
        switch(e) {
        case debug_severity::high: return "High";
        case debug_severity::medium: return "Medium";
        case debug_severity::low: return "Low";
        case debug_severity::notification: return "Notification";
        default: return "Other";
        }
    }

    void debug_callback(GLenum source, GLenum type, const uint32_t id, const GLenum severity, int /*length*/, const char* message, const void* user_param) noexcept
    {
        if (user_param)
            (*static_cast<const debug_function*>(user_param))(
                static_cast<debug_source>(source),
                static_cast<debug_type>(type),
                id,
                static_cast<debug_severity>(severity),
                message);
    }

    void set_debug_callback(const debug_function function) noexcept
    {
        static debug_function global_debug_callback;
        global_debug_callback = function;
        glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(&debug_callback), &global_debug_callback);
    }

    void set_debug_callback_enabled(debug_source d, const bool enable) noexcept
    {
        glDebugMessageControl(static_cast<GLenum>(d), GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, enable);
    }

    void set_debug_callback_enabled(debug_type d, const bool enable) noexcept
    {
        glDebugMessageControl(GL_DONT_CARE, static_cast<GLenum>(d), GL_DONT_CARE, 0, nullptr, enable);
    }

    void set_debug_callback_enabled(debug_severity d, const bool enable) noexcept
    {
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, static_cast<GLenum>(d), 0, nullptr, enable);
    }

    void set_debug_callback_enabled(uint32_t* ids, const size_t count, const bool enable) noexcept
    {
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, static_cast<int>(count), ids, enable);
    }
}
