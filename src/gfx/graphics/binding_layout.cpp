#include "binding_layout.hpp"
#include "../context.hpp"

#include "vulkan/binding_layout_vulkan.hpp"

namespace gfx {
inline namespace v1 {

std::unique_ptr<detail::binding_layout_implementation> detail::binding_layout_implementation::make()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl: return nullptr;
	case gapi::vulkan: return std::make_unique<vulkan::binding_layout_implementation>();
    default: return nullptr;
    }
}
binding_layout & binding_layout::push(binding_type type, u32 array_size)
{
	implementation()->push(type, array_size);
	_types.push_back(type);
	return *this;
}

const std::vector<binding_type>& binding_layout::types() const
{
	return _types;
}
}    // namespace v1
}    // namespace gfx