#include "swapchain.hpp"
#include "device.hpp"
#include "surface.hpp"
#include "sync.hpp"
#include <unordered_set>
#include <iostream>

namespace gfx
{
    inline namespace v1
    {

    swapchain::swapchain(device& d, surface& s, bool general_images)
        : _general_images(general_images)
        , _dispatcher(&d.get_dispatcher())
        , _device(d.get_device())
        , _gpu(d.get_physical_device())
        , _surface(s.get_surface())
    {
        std::unordered_set<u32> families;
        families.emplace(d.present_family());
        if (_general_images) {
            families.emplace(d.compute_family());
            families.emplace(d.transfer_family());
            families.emplace(d.graphics_family());
        }
        _condensed_families = std::vector<u32>(families.begin(), families.end());

		if (!d.get_physical_device().getSurfaceSupportKHR(d.present_family(), s.get_surface()))
			throw "";

        recreate(std::nullopt);
    }

    bool swapchain::recreate()
    {
        return recreate(*this);
    }

    const vk::SwapchainKHR& swapchain::chain() const noexcept
    {
        return _swapchain.get();
    }

    u32 swapchain::count() const noexcept
    {
        return _images.size();
    }

    const std::vector<vk::Image>& swapchain::imgs() const noexcept
    {
        return _images;
    }

    std::pair<u32, std::optional<acquire_error>> swapchain::next_image(opt_ref<const semaphore> sem, opt_ref<const fence> fen,
                                                                       std::chrono::nanoseconds timeout)
    {
		u32 img = 0;
        auto result = _device.acquireNextImageKHR(_swapchain.get(), timeout.count(), sem ? sem->get().sem() : nullptr,
                                                         fen ? fen->get().fen() : nullptr, &img);
        return {img, result == vk::Result::eSuccess ? std::nullopt : std::optional<acquire_error>(acquire_error(u32(result)))};
    }

	bool swapchain::has_resized() const noexcept
	{
		vk::SurfaceCapabilitiesKHR scaps{ 0 };
		const auto                 result = _gpu.getSurfaceCapabilitiesKHR(_surface, &scaps);
		if (result == vk::Result::eErrorSurfaceLostKHR) return true;
		return _extent != scaps.currentExtent;
	}

    bool swapchain::recreate(std::optional<std::reference_wrapper<swapchain>> old)
    {
        vk::SwapchainCreateInfoKHR sc;
        sc.clipped        = true;
        sc.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SurfaceCapabilitiesKHR scaps{0};
        const auto                 result = _gpu.getSurfaceCapabilitiesKHR(_surface, &scaps);
        if (result == vk::Result::eErrorSurfaceLostKHR) return false;
        sc.preTransform     = scaps.currentTransform;
        sc.minImageCount    = scaps.minImageCount;
        sc.imageExtent      = _extent = scaps.currentExtent;
        sc.imageArrayLayers = 1;

        using usg = vk::ImageUsageFlagBits;
        sc.imageUsage = scaps.supportedUsageFlags;


        sc.queueFamilyIndexCount = _condensed_families.size();
        sc.pQueueFamilyIndices   = _condensed_families.data();

        _present_mode            = find_present_mode(_gpu.getSurfacePresentModesKHR(_surface));
        const auto [cspace, fmt] = find_image_properties(_gpu.getSurfaceFormatsKHR(_surface));

        sc.presentMode     = _present_mode;
        sc.imageColorSpace = _color_space = cspace;
        sc.imageFormat     = _format      = fmt;

        sc.imageSharingMode = _condensed_families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;

        if (old) sc.oldSwapchain = (*old).get().chain();
        sc.surface               = _surface;

        auto newSwapchain = _device.createSwapchainKHRUnique(sc, nullptr, *_dispatcher);
		_swapchain.reset();
		_swapchain = std::move(newSwapchain);

        _images = _device.getSwapchainImagesKHR(_swapchain.get());
        return true;
    }

    vk::PresentModeKHR swapchain::find_present_mode(gsl::span<const vk::PresentModeKHR> modes)
    {
        using pres = vk::PresentModeKHR;
        pres       result = pres::eMailbox;
        for (auto& p : modes) {
            if (p == pres::eMailbox)
                return p;
            else if (p == pres::eFifo)
                result = p;
        }
        return result;
    }

    std::pair<vk::ColorSpaceKHR, vk::Format> swapchain::find_image_properties(gsl::span<const vk::SurfaceFormatKHR> modes)
    {
        std::pair<vk::ColorSpaceKHR, vk::Format> result{vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
        for (const auto&                         sfmt : modes) {
            if (sfmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && sfmt.format == vk::Format::eB8G8R8A8Unorm)
                return {vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
        }
        assert(false);
        return result;
    }
    }
}