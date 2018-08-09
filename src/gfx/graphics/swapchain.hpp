#pragma once
#include "image_view.hpp"
#include "implementation.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace gfx {
inline namespace v1 {
namespace detail {
class swapchain_implementation
{
public:
    static std::unique_ptr<swapchain_implementation> make();

    virtual ~swapchain_implementation()                                            = default;
    virtual uint32_t                       current_image() const noexcept          = 0;
    virtual void                           present()                               = 0;
    virtual void                           resize(uint32_t width, uint32_t height) = 0;
    virtual const std::vector<image_view>& image_views() const                     = 0;
    virtual handle                         api_handle()                            = 0;
};
}    // namespace detail

class swapchain : public impl::implements<detail::swapchain_implementation>
{
public:
    uint32_t current_image() const noexcept;
    void     present();
    void     resize(uint32_t width, uint32_t height);

    const std::vector<image_view>& image_views() const { return implementation()->image_views(); }
};
}    // namespace v1
}    // namespace gfx