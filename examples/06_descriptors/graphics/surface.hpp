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

			surface(const surface& other) = delete;
			surface& operator=(const surface& other) = delete;
		    surface(surface&& other) = default;
			surface& operator=(surface&& other) = default;

			const vk::SurfaceKHR& get_surface() const noexcept;

		private:
			vk::UniqueSurfaceKHR _surface;
		};
	}
}