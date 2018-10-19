#if _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "surface.hpp"
#include "instance.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <QWidget>

namespace gfx
{
	inline namespace v1
	{
		surface::surface(instance& i, GLFWwindow* glfw_window)
		{
			_surface = i.get_instance().createWin32SurfaceKHRUnique({ {}, nullptr, glfwGetWin32Window(glfw_window) });
		}

		surface::surface(instance& i, QWidget* qt_window) { _surface = i.get_instance().createWin32SurfaceKHRUnique({ {}, nullptr, HWND(qt_window->winId()) }); }

		const vk::SurfaceKHR& surface::get_surface() const noexcept { return _surface.get(); }
	}
}