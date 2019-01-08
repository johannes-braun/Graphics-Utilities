#if __has_include(<windows.h>)
#include <Windows.h>
#define GFX_USE_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "vulkan/instance.hpp"
#include "vulkan/surface.hpp"
#include "native_handle.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
surface::surface(instance& i, native_handle surface_handle) : _surface
{
#if defined(GFX_USE_WIN32)
    i.get_instance().createWin32SurfaceKHRUnique(
        {{}, nullptr, (assert(IsWindow(*reinterpret_cast<HWND*>(&surface_handle))), *reinterpret_cast<HWND*>(&surface_handle))})
#else
    nullptr
#endif
}
{}

vk::SurfaceKHR const& surface::get_surface() const noexcept
{
    return _surface.get();
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx