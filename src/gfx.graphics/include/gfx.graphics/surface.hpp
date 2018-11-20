#pragma once

#include <any>
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class native_handle
{
public:
    template<typename T, typename = std::enable_if_t<(sizeof(T) <= sizeof(void*))>>
    native_handle(T hnd) : _hnd(reinterpret_cast<void*>(hnd))
    {}

private:
    void* _hnd;
};

class instance;
class surface
{
public:
    explicit surface(instance& i, native_handle surface_handle);

    surface(surface const& other) = delete;
    surface& operator=(surface const& other) = delete;
    surface(surface&& other)                 = default;
    surface& operator=(surface&& other) = default;

    [[nodiscard]] auto get_surface() const noexcept -> vk::SurfaceKHR const&;

private:
    vk::UniqueSurfaceKHR _surface;
};
}    // namespace v1
}    // namespace gfx