#include "../context.hpp"
#include "binding_set.hpp"
#include "vulkan/binding_set_vulkan.hpp"
#include "opengl/binding_set_opengl.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::binding_set_implementation> detail::binding_set_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::binding_set_implementation>();
	case gapi::vulkan: return std::make_unique<vulkan::binding_set_implementation>();
    default: return nullptr;
    }
}

binding_set::binding_set(const binding_layout& layout)
{
	_layout = &layout;
    implementation()->initialize(layout);
}

void binding_set::bind(u32 binding, const image_view& view, const sampler& sampler)
{
	std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*> p(&view.implementation(), &sampler.implementation());
	implementation()->bind(binding, 0, _layout->types()[binding], p);
}

void binding_set::bind(u32 binding, const image_view& view)
{
	implementation()->bind(binding, 0, _layout->types()[binding], &view.implementation());
}

void binding_set::bind(u32 binding, u32 array_element, const image_view& view, const sampler& sampler)
{
	std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*> p(&view.implementation(), &sampler.implementation());
	implementation()->bind(binding, array_element, _layout->types()[binding], p);
}

void binding_set::bind(u32 binding, u32 array_element, const image_view& view)
{
	implementation()->bind(binding, array_element, _layout->types()[binding], &view.implementation());
}
}    // namespace v1
}    // namespace gfx