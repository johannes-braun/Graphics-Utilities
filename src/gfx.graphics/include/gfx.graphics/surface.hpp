#pragma once

#include <vulkan/vulkan.hpp>
#include <any>

namespace gfx
{
	inline namespace v1
	{
		class native_handle
		{
		public:
			template<typename T, typename = std::enable_if_t<(sizeof(T) <= sizeof(void*))>>
			native_handle(T hnd) : _hnd(reinterpret_cast<void*>(hnd))
			{
			}

		private:
			void* _hnd;
		};

		class instance;
		class surface
		{
		public:
			explicit surface(instance& i, native_handle surface_handle);

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