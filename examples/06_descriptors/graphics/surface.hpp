#pragma once

#include <vulkan/vulkan.hpp>

struct GLFWwindow;
class QWidget;

namespace gfx
{
	inline namespace v1
	{
		class instance;
		class surface
		{
		public:
			explicit surface(instance& i, GLFWwindow* glfw_window);
			explicit surface(instance& i, QWidget* qt_window);

			const vk::SurfaceKHR& surf() const noexcept;

		private:
			vk::UniqueSurfaceKHR _surface;
		};
	}
}