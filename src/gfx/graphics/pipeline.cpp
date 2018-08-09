#include "../context.hpp"
#include "pipeline.hpp"
#include "opengl/pipeline_opengl.hpp"
#include "vulkan/pipeline_vulkan.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::graphics_pipeline_implementation> detail::graphics_pipeline_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::graphics_pipeline_implementation>();
    case gapi::vulkan: return std::make_unique<vulkan::graphics_pipeline_implementation>();
    default:;
    }
    return nullptr;
}

std::unique_ptr<detail::compute_pipeline_implementation> detail::compute_pipeline_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::compute_pipeline_implementation>();
    case gapi::vulkan: return std::make_unique<vulkan::compute_pipeline_implementation>();
    default:;
    }
    return nullptr;
}

graphics_pipeline::graphics_pipeline(const pipe_state& state, const renderpass_layout& renderpass,
                                     std::initializer_list<v1::shader> shaders)
{
    std::vector<const v1::shader*> shs;
    for (auto& s : shaders) shs.push_back(&s);
    implementation()->initialize(state, renderpass, shs);
}

graphics_pipeline::graphics_pipeline(const pipe_state& state, const renderpass_layout& renderpass,
                                     std::initializer_list<const v1::shader* const> shaders)
      : graphics_pipeline(state, renderpass, span<const v1::shader* const>(std::data(shaders), std::size(shaders)))
{}

graphics_pipeline::graphics_pipeline(const pipe_state& state, const renderpass_layout& renderpass,
                                     span<const v1::shader* const> shaders)
{
    implementation()->initialize(state, renderpass, shaders);
}

compute_pipeline::compute_pipeline(const pipe_state::binding_layouts& layout, const v1::shader& cs)
{
    implementation()->initialize(layout, cs);
}
}    // namespace v2
}    // namespace gfx