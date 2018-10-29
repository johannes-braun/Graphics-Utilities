#include <graphics/framebuffer.hpp>
#include "opengl/framebuffer_opengl.hpp"
#include "vulkan/framebuffer_vulkan.hpp"
#include <context.hpp>

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::framebuffer_implementation> detail::framebuffer_implementation::make()
{
    switch(context::current()->options().graphics_api)
    {
	case gapi::opengl: return std::make_unique<opengl::framebuffer_implementation>();
	case gapi::vulkan: return std::make_unique<vulkan::framebuffer_implementation>();
    default: break;
    }
	return nullptr;
}

depth_stencil::depth_stencil(float d, u32 s)
    : depth(d)
    , stencil(s)
{
}

framebuffer::framebuffer(u32 width, u32 height, u32 layers, const renderpass_layout& layout)
    : _width(width)
    , _height(height), _layers(layers)
{
    implementation()->create(_width, _height, _layers, layout);
}

void framebuffer::attach(attachment att, uint32_t index, const image_view& img_view, std::optional<clear_value> clear)
{
    switch (att) {
    case attachment::color: _color_clear_values.resize(std::max<size_t>(_color_clear_values.size(), index + 1));
        _color_clear_values[index] = clear;
        break;
    case attachment::resolve: break;
    case attachment::depth_stencil: _depth_clear_value = clear;
        break;
    }
    implementation()->attach(att, index, img_view, clear);
}

void framebuffer::detach(attachment att, uint32_t index)
{
    switch (att) {
    case attachment::color: _color_clear_values[index] = {};
        break;
    case attachment::resolve: break;
    case attachment::depth_stencil: _depth_clear_value = {};
        break;
    }
    implementation()->detach(att, index);
}

const std::vector<std::optional<clear_value>>& framebuffer::color_clear_values() const noexcept
{
    return _color_clear_values;
}

const std::optional<clear_value>& framebuffer::depth_clear_value() const noexcept
{
    return _depth_clear_value;
}
}
}    // namespace gfx