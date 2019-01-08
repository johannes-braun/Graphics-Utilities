#include "gfx.core/log.hpp"
#include "opengl/context.hpp"
#include <iostream>
#include <memory>
#include <mygl/mygl.hpp>

#if defined(WIN32)
#pragma comment(lib, "opengl32.lib")
#include <Windows.h>
#include <cassert>

namespace gfx {
inline namespace v1 {
namespace opengl {
namespace {
thread_local context const* _current_context = nullptr;
}
HDC native_dc(native_handle h)
{
    return reinterpret_cast<HDC&>(h);
}
native_handle native_dc(HDC h)
{
    return reinterpret_cast<native_handle&>(h);
}

void context::check_handle(native_handle hnd)
{
    assert(hnd == native_handle(0) || IsWindow(reinterpret_cast<HWND&>(hnd)));
}

void* get_address(const char* name)
{
    struct free_module_t
    {
        void operator()(HMODULE m) const noexcept { FreeLibrary(m); }
    };
    static std::unique_ptr<HINSTANCE__, free_module_t> opengl32_lib {LoadLibraryA("opengl32")};
    if (const auto addr = wglGetProcAddress(name); addr) return reinterpret_cast<void*>(addr);
    return reinterpret_cast<void*>(GetProcAddress(opengl32_lib.get(), name));
}

struct context_native_t
{
    HGLRC (*wglCreateContextAttribsARB)(HDC, HGLRC, const int*) = nullptr;
    BOOL(*wglChoosePixelFormatARB)
    (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats) = nullptr;
    BOOL(*wglGetPixelFormatAttribivARB)
    (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, int* piValues) = nullptr;
    int (*wglSwapIntervalEXT)(int)                                                                         = nullptr;
};

context const* context::current()
{
    return _current_context;
}

void context::clear_current() noexcept
{
    if (_current_context) wglMakeCurrent(native_dc(_current_context->_device_context), nullptr);
    _current_context = nullptr;
}

void context::make_current(context const* c) noexcept
{
    c->make_current();
}

context::context(native_handle window, std::vector<std::pair<context_attribs, int>> context_attributes,
                 std::vector<std::pair<pixel_format_attribs, int>> attributes, const context* const shared)
      : _native(std::make_unique<context_native_t>())
{
    check_handle(window);
    PIXELFORMATDESCRIPTOR desc {sizeof(PIXELFORMATDESCRIPTOR),
                                1,
                                DWORD((window == native_handle(0) ? PFD_DRAW_TO_WINDOW : 0) | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER),
                                PFD_TYPE_RGBA,
                                32,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                24,
                                8,
                                8,
                                PFD_MAIN_PLANE,
                                0,
                                0,
                                0,
                                0};
    if (window == native_handle(0))
    {
        WNDCLASSW wnd {0};
        wnd.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        };
        wnd.hInstance     = GetModuleHandle(nullptr);
        wnd.lpszClassName = L"temp_win_wgl";
        // Register window class
        RegisterClassW(&wnd);
        HWND win         = CreateWindowExW(0, L"temp_win_wgl", L"", 0, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        _own_window      = reinterpret_cast<native_handle&>(win);
        _is_window_owner = true;
    }
    else
    {
        _own_window      = window;
        _is_window_owner = false;
    }

    {
        // Load wgl extension functions using a dummy window.
        const std::unique_ptr<HWND__, decltype(&DestroyWindow)> temp_wnd(
            CreateWindow("STATIC", "_dummy_", WS_DISABLED, 0, 0, 0, 0, NULL, NULL, NULL, NULL), &DestroyWindow);
        const HDC temp_ctx = GetDC(temp_wnd.get());
        SetPixelFormat(temp_ctx, ChoosePixelFormat(temp_ctx, &desc), &desc);
        const std::unique_ptr<HGLRC__, decltype(&wglDeleteContext)> temp_context(wglCreateContext(temp_ctx), &wglDeleteContext);
        wglMakeCurrent(temp_ctx, temp_context.get());
#define load_fun(Fname) _native->Fname = reinterpret_cast<decltype(_native->Fname)>(wglGetProcAddress(#Fname));
        load_fun(wglCreateContextAttribsARB);
        load_fun(wglChoosePixelFormatARB);
        load_fun(wglGetPixelFormatAttribivARB);
        load_fun(wglSwapIntervalEXT);
#undef load_fun;
    }

    _device_context = native_dc(GetDC(reinterpret_cast<HWND&>(_own_window)));
    context_attributes.emplace_back(context_attribs(0), 0);
    int*          attr          = reinterpret_cast<int*>(&context_attributes[0].first);
    native_handle shared_handle = shared ? shared->_context : native_handle(0);
    set_pixel_format(attributes);
    _context = static_cast<native_handle>(reinterpret_cast<uint64_t>(
        _native->wglCreateContextAttribsARB(native_dc(_device_context), reinterpret_cast<HGLRC&>(shared_handle), attr)));
    make_current();

    get_proc_address = reinterpret_cast<decltype(get_proc_address)>(&get_address);
    mygl::load(get_proc_address);
}

void context::set_pixel_format(std::vector<std::pair<pixel_format_attribs, int>> attributes) const
{
    float                                             pixel_attribs_f[] = {0};
    std::vector<std::pair<pixel_format_attribs, int>> default_attributes {{GL_DRAW_TO_WINDOW_ARB, true},
                                                                          {GL_SUPPORT_OPENGL_ARB, true},
                                                                          {GL_PIXEL_TYPE_ARB, GL_TYPE_RGBA_ARB},
                                                                          {GL_ACCELERATION_ARB, GL_FULL_ACCELERATION_ARB},
                                                                          {GL_COLOR_BITS_ARB, 32},
                                                                          {GL_ALPHA_BITS_ARB, 8},
                                                                          {GL_DEPTH_BITS_ARB, 24},
                                                                          {GL_STENCIL_BITS_ARB, 8},
                                                                          {GL_DOUBLE_BUFFER_ARB, true},
                                                                          {GL_SAMPLE_BUFFERS_ARB, true},
                                                                          {GL_SAMPLES_ARB, 1}};

    default_attributes.insert(default_attributes.end(), attributes.begin(), attributes.end());
    default_attributes.emplace_back(pixel_format_attribs(0), 0);
    const auto attr = reinterpret_cast<int*>(&default_attributes[0].first);

    UINT format_count     = 0;
    int  pixel_format[16] = {0};
    _native->wglChoosePixelFormatARB(native_dc(_device_context), attr, pixel_attribs_f, std::size(pixel_format), pixel_format,
                                     &format_count);
    assert(format_count != 0);
    if (format_count == 0) return;

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    int chosen_format = 0;
    if (!DescribePixelFormat(native_dc(_device_context), pixel_format[chosen_format], sizeof(pfd), &pfd))
        gfx::elog << "Failed to describe pixel format.";
    if (!SetPixelFormat(native_dc(_device_context), pixel_format[chosen_format], &pfd)) gfx::elog << "Failed to set pixel format.";
}

void context::set_swap_interval(int i) const noexcept
{
    _native->wglSwapIntervalEXT(i);
}

context::get_address_fun context::get_loader() const noexcept
{
    return get_proc_address;
}

context::~context()
{
    if (_context != native_handle(0)) wglDeleteContext(reinterpret_cast<HGLRC&>(_context));
    if (_is_window_owner) DestroyWindow(reinterpret_cast<HWND&>(_own_window));
}

void context::make_current() const noexcept
{
    wglMakeCurrent(native_dc(_device_context), reinterpret_cast<const HGLRC&>(_context));
    _current_context = this;
}

void context::swap_buffers() const noexcept
{
    SwapBuffers(native_dc(_device_context));
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx

#endif    // defined(WIN32)