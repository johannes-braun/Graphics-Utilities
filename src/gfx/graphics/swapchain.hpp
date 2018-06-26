#pragma once
#include <cstdint>
#include <memory>

namespace gfx
{
namespace detail
{
    class swapchain_implementation
    {
    public:
        virtual ~swapchain_implementation()                      = default;
        virtual uint32_t current_image() const noexcept          = 0;
        virtual void     present()                               = 0;
        virtual void     resize(uint32_t width, uint32_t height) = 0;
    };

    std::unique_ptr<swapchain_implementation> make_swapchain_implementation();
} // namespace detail

class swapchain
{
public:
    swapchain();
    uint32_t current_image() const noexcept;
    void     present();
    void     resize(uint32_t width, uint32_t height);

private:
    std::unique_ptr<detail::swapchain_implementation> _implementation;
};
} // namespace gfx
