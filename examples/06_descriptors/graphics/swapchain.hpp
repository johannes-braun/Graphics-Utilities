#pragma once

#include "gsl/span"
#include "types.hpp"
#include <chrono>

namespace gfx {
inline namespace v1 {
enum class acquire_error
{
    not_ready   = 1,
    timeout     = 2,
    out_of_date = -1000001004,
    suboptimal  = 1000001003
};

class device;
class surface;
class semaphore;
class fence;
struct extension_dispatch;

class swapchain
{
public:
    explicit swapchain(device& d, surface& s, bool general_images = true);
    bool                    recreate();
    const vk::SwapchainKHR& chain() const noexcept;

    u32                                          count() const noexcept;
    const std::vector<vk::Image>&                imgs() const noexcept;
    std::pair<u32, std::optional<acquire_error>> next_image(opt_ref<const semaphore> sem     = std::nullopt,
                                                            opt_ref<const fence>     fen     = std::nullopt,
                                                            std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max());

private:
    [[maybe_unused]] bool recreate(std::optional<std::reference_wrapper<swapchain>> old);
    static vk::PresentModeKHR find_present_mode(gsl::span<const vk::PresentModeKHR> modes);
    static std::pair<vk::ColorSpaceKHR, vk::Format> find_image_properties(gsl::span<const vk::SurfaceFormatKHR> modes);

    bool                                                   _general_images;
    const extension_dispatch*                              _dispatcher = nullptr;
    vk::Device                                             _device;
    vk::PhysicalDevice                                     _gpu;
    vk::SurfaceKHR                                         _surface;
    vk::UniqueHandle<vk::SwapchainKHR, extension_dispatch> _swapchain;
    std::vector<u32>                                       _condensed_families;
    std::vector<vk::Image>                                 _images;
    vk::PresentModeKHR                                     _present_mode = {};
    vk::ColorSpaceKHR                                      _color_space  = {};
    vk::Format                                             _format       = {};
    vk::Extent2D                                           _extent       = {};
};
}    // namespace v1
}    // namespace gfx
