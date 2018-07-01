#pragma once

#include "host_image.hpp"
#include <gfx/api.hpp>

namespace gfx
{
namespace detail
{
    class device_image_implementation
    {
    public:
        virtual ~device_image_implementation()                                                                         = default;
        virtual void     initialize(uint32_t layer_dimensions, img_format format, const extent& size, uint32_t levels) = 0;
        virtual void     fill_from(const host_image& image, uint32_t level, uint32_t layer)                            = 0;
        virtual std::any api_handle()                                                                                  = 0;
        virtual void     generate_mipmaps() = 0;
    };
    std::unique_ptr<device_image_implementation> make_device_image_implementation();
} // namespace detail

GFX_api_cast_type(gapi::opengl, device_image, mygl::texture)

class device_image
{
public:
    class img_reference
    {
    public:
        void operator<<(const host_image& image) const;

    private:
        friend class device_image;
        img_reference(uint32_t level, uint32_t layer, device_image& img);
        uint32_t      _level;
        uint32_t      _layer;
        device_image& _img;
    };

    device_image(uint32_t layer_dimensions, img_format format, const extent& size, uint32_t levels);
    img_reference operator[](uint32_t layer);
    img_reference level(uint32_t level);
    img_reference layer(uint32_t layer);
    img_reference sub_image(uint32_t level, uint32_t layer);
    void          generate_mipmaps();

    GFX_api_cast_op(gapi::opengl, device_image)

private: 
    std::unique_ptr<detail::device_image_implementation> _implementation;
};

GFX_api_cast_impl(gapi::opengl, device_image)
}
