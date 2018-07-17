#include "descriptor.hpp"
#include "opengl/descriptor_opengl.hpp"
#include <gfx/context.hpp>

namespace gfx
{
std::unique_ptr<detail::descriptor_set_implementation> detail::descriptor_set_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::descriptor_set_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::host_buffer_implementation>();
    default: break;
    }
    return nullptr;
}

descriptor_set::descriptor_set()
    : _bindings(4)
{
}

void descriptor_set::set(descriptor_type type, uint32_t binding, const image_view& view, const sampler& sampler)
{
    get_at_or_create(type, binding) = std::make_pair(view.api_handle(), sampler.api_handle());
    implementation()->update(type, binding, {view.api_handle(), sampler.api_handle()});
}

std::any& descriptor_set::get_at_or_create(descriptor_type t, uint32_t binding)
{
    auto& vec = _bindings[static_cast<int>(t)];

    if (vec.empty()) return vec.emplace_back(binding, std::any()).second;

    auto it = std::prev(
        std::upper_bound(vec.begin(), vec.end(), binding, [](uint32_t b, const std::pair<uint32_t, std::any>& p) { return b < p.first; }));

    if (it == vec.end() || it->first != binding) it = vec.insert(it == vec.end() ? it : std::next(it), std::make_pair(binding, std::any()));
    return it->second;
}
}
