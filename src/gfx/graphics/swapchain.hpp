#pragma once
#include "implementation.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include "image_view.hpp"

namespace gfx
{
namespace detail
{
class swapchain_implementation
{
public:
    static std::unique_ptr<swapchain_implementation> make();

    virtual ~swapchain_implementation()                      = default;
    virtual uint32_t current_image() const noexcept          = 0;
    virtual void     present()                               = 0;
    virtual void     resize(uint32_t width, uint32_t height) = 0;
	virtual const std::vector<image_view>& image_views() const = 0;
    virtual std::any api_handle()                            = 0;
};
}    // namespace detail

class swapchain : public detail::base::implements<detail::swapchain_implementation>
{
public:
    uint32_t current_image() const noexcept;
    void     present();
    void     resize(uint32_t width, uint32_t height);

	const std::vector<image_view>& image_views() const { return implementation()->image_views(); }
};
}    // namespace gfx
