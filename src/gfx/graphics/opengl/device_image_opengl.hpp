#pragma once

#include "../device_image.hpp"
#include "../sampler.hpp"
#include <any>
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {

class device_image_implementation : public detail::device_image_implementation
{
public:
	~device_image_implementation();
	device_image_implementation() = default;
	device_image_implementation(device_image_implementation&& other) noexcept;
	device_image_implementation& operator=(device_image_implementation&& other) noexcept;

    void     initialize(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels, sample_count samples) override;
    void     fill_from(const host_image& image, uint32_t level, uint32_t layer) override;
    void     fill_to(const host_image& image, uint32_t level, uint32_t layer) override;
    std::any api_handle() override;
    void     generate_mipmaps() override;

private:
    mygl::texture _handle;
    GLenum        _internal_format;
    GLenum        _external_format;
    GLenum        _external_type;
    extent        _extent;
    GLenum        _type;
    uint32_t      _levels;
    format        _format;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx