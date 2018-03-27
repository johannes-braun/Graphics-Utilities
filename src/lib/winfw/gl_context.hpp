#pragma once

#include "types.hpp"
#include "window.hpp"

enum context_profile_mask
{
    GL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001,
    GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002
};

enum context_release_behavior
{
    GL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB = 0,
    GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB = 0x2098
};

enum context_flags
{
    GL_CONTEXT_DEBUG_BIT_ARB = 0x00000001,
    GL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x00000002,
    GL_CONTEXT_ROBUST_ACCESS_BIT_ARB = 0x00000004,
    GL_CONTEXT_RESET_ISOLATION_BIT_ARB = 0x00000008,
};

enum context_attribs
{
    GL_CONTEXT_MAJOR_VERSION_ARB = 0x2091,
    GL_CONTEXT_MINOR_VERSION_ARB = 0x2092,
    GL_CONTEXT_FLAGS_ARB = 0x2094,
    GL_CONTEXT_OPENGL_NO_ERROR_ARB = 0x31B3,
    GL_CONTEXT_PROFILE_MASK_ARB = 0x9126,
    GL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB = 0x8256,
    GL_CONTEXT_RELEASE_BEHAVIOR_ARB = 0x2097,
};

namespace winfw::gl
{
    class context
    {
    public:
        context(const winfw::window& window, std::vector<std::pair<context_attribs, int>> context_attributes = {});
        ~context();

        void make_current() const noexcept;
        void swap_buffers() const noexcept;
        void set_swap_interval(int i) const noexcept;

    private:
        int(*swapIntervalEXT)(int) = nullptr;
        native_handle _device_context;
        native_handle _context;
    };
}