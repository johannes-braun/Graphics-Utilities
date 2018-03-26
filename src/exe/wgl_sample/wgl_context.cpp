#define WGL_PUBLIC_HANDLES
#define WIN32_WINDOW_PUBLIC_HANDLES
#define APIENTRY *
#define WINGDIAPI
#include <gl/GL.h>
#undef APIENTRY
#undef WINGDIAPI
#include "wgl.h"
#include "wglext.h"
#include "wgl_context.hpp"

namespace wgl
{
    context::context(const win32::window& window)
    {
        PIXELFORMATDESCRIPTOR desc{
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0, 0,
            0, 0, 0, 0, 0,
            24,
            8,
            8,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };
        
        _device_context = GetDC(static_cast<HWND>(window._handle));
        int pixel_format = ChoosePixelFormat(static_cast<HDC>(_device_context), &desc);
        SetPixelFormat(static_cast<HDC>(_device_context), pixel_format, &desc);

        HGLRC temp_context = wglCreateContext(static_cast<HDC>(_device_context));
        wglMakeCurrent(static_cast<HDC>(_device_context), temp_context);

        auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        auto wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
        wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));

        int context_attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        float pixel_attribs_f[] = { 0 };
        int pixel_attribs_i[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_COLOR_BITS_ARB, 32,
            WGL_RED_BITS_ARB, 8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB, 8,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_SAMPLES_ARB, 8,
            0 };

        UINT format_count = 0;
        wglChoosePixelFormatARB(static_cast<HDC>(_device_context), pixel_attribs_i, pixel_attribs_f, 1, &pixel_format, &format_count);
        if (format_count == 0) throw std::runtime_error("Could not find suitable pixel formats.");
        _context = wglCreateContextAttribsARB(static_cast<HDC>(_device_context), nullptr, context_attribs);

        wglDeleteContext(temp_context);
    }

    void context::set_swap_interval(int i) const noexcept 
    {
        wglSwapIntervalEXT(i);
    }

    context::~context()
    {
        if (_context)
            wglDeleteContext(static_cast<HGLRC>(_context));
    }

    void context::make_current() const noexcept
    {
        wglMakeCurrent(static_cast<HDC>(_device_context), static_cast<HGLRC>(_context));
    }

    void context::swap_buffers() const noexcept
    {
        SwapBuffers(static_cast<HDC>(_device_context));
    }
}