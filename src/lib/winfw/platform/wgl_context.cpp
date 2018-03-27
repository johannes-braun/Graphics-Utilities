#define APIENTRY *
#define WINGDIAPI
#include <gl/GL.h>
#undef APIENTRY
#undef WINGDIAPI
#include "gl/wgl.h"

#include <Windows.h>

#include "../gl_context.hpp"

namespace winfw::gl
{
    HDC native_dc(native_handle h) { return reinterpret_cast<HDC>(h); }
    native_handle native_dc(HDC h) { return reinterpret_cast<native_handle&>(h); }

    context::context(const winfw::window& window, std::vector<std::pair<context_attribs, int>> context_attributes)
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
        
        _device_context = native_dc(GetDC(reinterpret_cast<HWND>(window.get_native_handle())));
        int pixel_format = ChoosePixelFormat(native_dc(_device_context), &desc);
        SetPixelFormat(native_dc(_device_context), pixel_format, &desc);

        HGLRC temp_context = wglCreateContext(native_dc(_device_context));
        wglMakeCurrent(native_dc(_device_context), temp_context);

        auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        auto wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
        swapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));

        context_attributes.emplace_back(context_attribs(0), 0);
        int* attr = reinterpret_cast<int*>(&context_attributes[0].first);

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
        wglChoosePixelFormatARB(native_dc(_device_context), pixel_attribs_i, pixel_attribs_f, 1, &pixel_format, &format_count);
        
        if (format_count == 0) throw std::runtime_error("Could not find suitable pixel formats.");
        _context = static_cast<native_handle>(reinterpret_cast<uint64_t>(wglCreateContextAttribsARB(native_dc(_device_context), nullptr, attr)));
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(native_dc(_device_context), pixel_format, sizeof(pfd), &pfd);
        SetPixelFormat(native_dc(_device_context), pixel_format, &pfd);

        wglDeleteContext(temp_context);
    }

    void context::set_swap_interval(int i) const noexcept 
    {
        swapIntervalEXT(i);
    }

    context::~context()
    {
        if (_context != native_handle::null)
            wglDeleteContext(reinterpret_cast<HGLRC>(_context));
    }

    void context::make_current() const noexcept
    {
        wglMakeCurrent(native_dc(_device_context), reinterpret_cast<HGLRC>(_context));
    }

    void context::swap_buffers() const noexcept
    {
        SwapBuffers(native_dc(_device_context));
    }
}