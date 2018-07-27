#pragma once

#include "host_image.hpp"
#include "implementation.hpp"
#include <gfx/api.hpp>
#include "sampler.hpp"

namespace gfx {
inline namespace v1 {
class device_image;
enum class sample_count;

namespace detail {
class device_image_implementation
{
public:
    static std::unique_ptr<device_image_implementation> make();

    virtual ~device_image_implementation() = default;
    virtual void     initialize(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels, sample_count samples) = 0;
    virtual void     fill_from(const host_image& image, uint32_t level, uint32_t layer)                                              = 0;
    virtual void     fill_to(const host_image& image, uint32_t level, uint32_t layer)                                                = 0;
    virtual std::any api_handle()                                                                                                    = 0;
    virtual void     generate_mipmaps()                                                                                              = 0;
};
}    // namespace detail

enum class img_type
{
    image1d = 1,
    image2d = 2,
    image3d = 3
};

class image_view;
enum class imgv_type;
class device_image : public impl::implements<detail::device_image_implementation>
{
public:
    constexpr static uint32_t max_levels = ~0u;

    class img_reference
    {
    public:
		friend class image_view;

        void operator<<(const host_image& image) const;
        void operator>>(const host_image& image) const;

		image_view view(imgv_type type);

    private:
        friend class device_image;
        img_reference(uint32_t level, uint32_t layer, device_image& img);
        uint32_t      _level;
        uint32_t      _layer;
        device_image& _img;
    };

    device_image(img_type type, format format, const extent& size, uint32_t levels);
    device_image(img_type type, format format, const extent& size, sample_count samples);
    device_image(const host_image& image, uint32_t levels = max_levels);

	device_image(device_image&&) = default;
	device_image& operator=(device_image&&) = default;
	device_image(const device_image&) = default;
	device_image& operator=(const device_image&) = default;

    img_reference operator[](uint32_t layer);
    img_reference level(uint32_t level);
    img_reference layer(uint32_t layer);
	image_view view(imgv_type type);
    img_reference sub_image(uint32_t level, uint32_t layer);
    void          generate_mipmaps();

    const uint32_t&     dimensions() const noexcept;
    const format&       pixel_format() const noexcept;
    const extent&       extents() const noexcept;
    const uint32_t&     levels() const noexcept;
    const sample_count& samples() const noexcept { return _samples; }

private:
    uint32_t     _layer_dimensions;
    format       _format;
    extent       _extent;
    uint32_t     _levels;
    sample_count _samples;
};
}    // namespace v1
}    // namespace gfx
