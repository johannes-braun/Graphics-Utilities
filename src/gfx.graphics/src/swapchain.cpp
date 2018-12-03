#include "swapchain.hpp"
#include "device.hpp"
#include "surface.hpp"
#include "sync.hpp"
#include <iostream>
#include <unordered_set>
#include "image.hpp"

namespace gfx {
inline namespace v1 {
swapchain::swapchain(device& d, surface& s, bool general_images)
      : _general_images(general_images)
      , _dispatcher(&d.get_dispatcher())
      , _device(d.get_device())
      , _gpu(d.get_physical_device())
      , _surface(s.get_surface())
{
    std::unordered_set<u32> families;
    families.emplace(d.present_family());
    if (_general_images)
    {
        families.emplace(d.compute_family());
        families.emplace(d.transfer_family());
        families.emplace(d.graphics_family());
    }
    _condensed_families = std::vector<u32>(families.begin(), families.end());

    if (!d.get_physical_device().getSurfaceSupportKHR(d.present_family(), s.get_surface())) throw "";

    recreate(std::nullopt);
}

bool swapchain::recreate()
{
    return recreate(*this);
}

vk::SwapchainKHR const& swapchain::get_swapchain() const noexcept
{
    return _swapchain.get();
}

u32 swapchain::count() const noexcept
{
    return u32(_images.size());
}

std::vector<gfx::image> const& swapchain::images() const noexcept
{
    return _images;
}

auto swapchain::present_mode() const noexcept -> vk::PresentModeKHR
{
    return _present_mode;
}

auto swapchain::color_space() const noexcept -> vk::ColorSpaceKHR
{
    return _color_space;
}

auto swapchain::format() const noexcept -> vk::Format
{
    return _format;
}

auto swapchain::extent() const noexcept -> vk::Extent2D
{
    return _extent;
}

std::optional<acquire_error> swapchain::swap(opt_ref<semaphore const> sem, opt_ref<fence const> fen,
                                                                   std::chrono::nanoseconds timeout)
{
    u32  img    = 0;
    auto result = _device.acquireNextImageKHR(_swapchain.get(), timeout.count(), sem ? sem->get().get_semaphore() : nullptr,
                                              fen ? fen->get().get_fence() : nullptr, &img);
    _current_image = img;
    return result == vk::Result::eSuccess ? std::nullopt : std::optional<acquire_error>(acquire_error(u32(result)));
}

auto swapchain::current_index() const noexcept -> uint32_t
{
    return _current_image.load();
}

auto swapchain::current_image() const noexcept -> gfx::image const&
{
    return _images[current_index()];
}

bool swapchain::has_resized() const noexcept
{
    vk::SurfaceCapabilitiesKHR scaps{0};
    auto const                 result = _gpu.getSurfaceCapabilitiesKHR(_surface, &scaps);
    if (result == vk::Result::eErrorSurfaceLostKHR) return true;
    return _extent != scaps.currentExtent;
}

bool swapchain::recreate(std::optional<std::reference_wrapper<swapchain>> old)
{
    vk::SwapchainCreateInfoKHR sc;
    sc.clipped        = true;
    sc.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SurfaceCapabilitiesKHR scaps{0};
    auto const                 result = _gpu.getSurfaceCapabilitiesKHR(_surface, &scaps);
    if (result == vk::Result::eErrorSurfaceLostKHR) return false;

    sc.preTransform  = scaps.currentTransform;
    sc.minImageCount = scaps.minImageCount;
    sc.imageExtent = _extent = scaps.currentExtent;
    sc.imageArrayLayers      = 1;

    using usg     = vk::ImageUsageFlagBits;
    sc.imageUsage = scaps.supportedUsageFlags;


    sc.queueFamilyIndexCount = u32(_condensed_families.size());
    sc.pQueueFamilyIndices   = _condensed_families.data();

    _present_mode            = find_present_mode(_gpu.getSurfacePresentModesKHR(_surface));
    const auto [cspace, fmt] = find_image_properties(_gpu.getSurfaceFormatsKHR(_surface));

    sc.presentMode     = _present_mode;
    sc.imageColorSpace = _color_space = cspace;
    sc.imageFormat = _format = fmt;

    sc.imageSharingMode = _condensed_families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;

    if (old) sc.oldSwapchain = (*old).get().get_swapchain();
    sc.surface = _surface;

    auto newSwapchain = _device.createSwapchainKHRUnique(sc, nullptr, *_dispatcher);
    _swapchain.reset();
    _swapchain = std::move(newSwapchain);

    _images.clear();
    for(auto const img : _device.getSwapchainImagesKHR(_swapchain.get()))
    {
        image pre_init;
        pre_init._image = img;
        _images.push_back(std::move(pre_init));
    }
    return true;
}

vk::PresentModeKHR swapchain::find_present_mode(gsl::span<vk::PresentModeKHR const> modes)
{
    using pres  = vk::PresentModeKHR;
    pres result = pres::eMailbox;
    for (auto& p : modes)
    {
        if (p == pres::eMailbox)
            return p;
        else if (p == pres::eFifo)
            result = p;
    }
    return result;
}

std::pair<vk::ColorSpaceKHR, vk::Format> swapchain::find_image_properties(gsl::span<vk::SurfaceFormatKHR const> modes)
{
    std::pair<vk::ColorSpaceKHR, vk::Format> result{vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
    for (auto const& sfmt : modes)
    {
        if (sfmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && sfmt.format == vk::Format::eB8G8R8A8Unorm)
            return {vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
    }
    assert(false);
    return result;
}
}    // namespace v1
}    // namespace gfx