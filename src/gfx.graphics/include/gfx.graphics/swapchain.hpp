#pragma once

#include "gsl/span"
#include "types.hpp"
#include <chrono>
#include <atomic>

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
class image;
class fence;
struct extension_dispatch;

class swapchain
{
public:
    explicit swapchain(device& d, surface& s, bool general_images = true);
    swapchain(swapchain const&) = delete;
    swapchain& operator=(swapchain const&) = delete;
    swapchain(swapchain&&)                 = default;
    swapchain& operator=(swapchain&&) = default;
    ~swapchain()                      = default;

    [[nodiscard]] auto get_swapchain() const noexcept -> vk::SwapchainKHR const&;
    [[nodiscard]] auto images() const noexcept -> std::vector<gfx::image> const&;
    [[nodiscard]] auto present_mode() const noexcept -> vk::PresentModeKHR;
    [[nodiscard]] auto color_space() const noexcept -> vk::ColorSpaceKHR;
    [[nodiscard]] auto format() const noexcept -> vk::Format;
    [[nodiscard]] auto extent() const noexcept -> vk::Extent2D;
    [[nodiscard]] auto count() const noexcept -> u32;
    [[nodiscard]] auto has_resized() const noexcept -> bool;

    [[maybe_unused]] auto recreate() -> bool;
    [[nodiscard]] auto    swap(opt_ref<semaphore const> sem = std::nullopt, opt_ref<fence const> fen = std::nullopt,
                                     std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max())
        -> std::optional<acquire_error>;
    [[nodiscard]] auto current_index() const noexcept ->uint32_t;
    [[nodiscard]] auto current_image() const noexcept->gfx::image const&;

private:
    [[maybe_unused]] auto     recreate(std::optional<std::reference_wrapper<swapchain>> old) -> bool;
    [[nodiscard]] static auto find_present_mode(gsl::span<const vk::PresentModeKHR> modes) -> vk::PresentModeKHR;
    [[nodiscard]] static auto find_image_properties(gsl::span<const vk::SurfaceFormatKHR> modes)
        -> std::pair<vk::ColorSpaceKHR, vk::Format>;

    bool                                                   _general_images;
    extension_dispatch const*                              _dispatcher = nullptr;
    vk::Device                                             _device;
    vk::PhysicalDevice                                     _gpu;
    vk::SurfaceKHR                                         _surface;
    vk::UniqueHandle<vk::SwapchainKHR, extension_dispatch> _swapchain;
    std::vector<u32>                                       _condensed_families;
    std::vector<gfx::image>                                _images;
    vk::PresentModeKHR                                     _present_mode = {};
    vk::ColorSpaceKHR                                      _color_space  = {};
    vk::Format                                             _format       = {};
    vk::Extent2D                                           _extent       = {};
    std::atomic_uint32_t                                   _current_image = 0;
};
}    // namespace v1
}    // namespace gfx
