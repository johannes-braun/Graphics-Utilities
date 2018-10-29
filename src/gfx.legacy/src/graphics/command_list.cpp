#include <graphics/command_list.hpp>
#include "opengl/command_list_opengl.hpp"
#include "vulkan/command_list_vulkan.hpp"
#include <context.hpp>

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::commands_implementation> detail::commands_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::commands_implementation>();
    case gapi::vulkan: return std::make_unique<vulkan::commands_implementation>();
    }
    return nullptr;
}

commands::commands(commands_type type)
{
    implementation()->initialize(type);
}

void commands::reset() const
{
    implementation()->reset();
}

void commands::begin() const
{
    implementation()->begin();
}

void commands::end() const
{
    implementation()->end();
}

void commands::execute() const
{
    implementation()->execute(nullptr);
}

void commands::execute(fence& f) const
{
    implementation()->execute(&f);
}

void commands::execute_sync_after(const commands& cmd) const
{
    implementation()->execute_sync_after(cmd, nullptr);
}

void commands::execute_sync_after(const commands& cmd, fence& f) const
{
    implementation()->execute_sync_after(cmd, &f);
}

void commands::bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings) const
{
    implementation()->bind_pipeline(p, bindings);
}

void commands::dispatch(u32 x, u32 y, u32 z) const
{
    implementation()->dispatch(x, y, z);
}

void commands::begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area) const
{
    implementation()->begin_pass(fbo, render_area);
}

void commands::end_pass() const
{
    implementation()->end_pass();
}

void commands::bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings) const
{
    implementation()->bind_pipeline(p, bindings);
}

void commands::draw(const drawcmd& cmd) const
{
    implementation()->draw(cmd.vertex_count, cmd.instance_count, cmd.base_vertex, cmd.base_instance);
}

void commands::draw_indexed(const drawcmd_indexed& cmd) const
{
    implementation()->draw_indexed(cmd.index_count, cmd.instance_count, cmd.base_index, cmd.base_vertex, cmd.base_instance);
}

void commands::push_binding(u32 set, u32 binding, binding_type type, const image_view& view, const sampler& sampler) const
{
    push_binding(set, binding, 0, type, view, sampler);
}

void commands::push_binding(u32 set, u32 binding, binding_type type, const image_view& view) const
{
    push_binding(set, binding, 0, type, view);
}

void commands::push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view, const sampler& sampler) const
{
    std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*> p(
        &view.implementation(), &sampler.implementation());
    implementation()->push_binding(set, binding, array_element, type, p, 0, 0);
}

void commands::push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view) const
{
    implementation()->push_binding(set, binding, array_element, type, &view.implementation(), 0, 0);
}

void commands::set_viewports(u32 first, gsl::span<viewport> vp, gsl::span<rect2f> scissors) const
{
    implementation()->set_viewports(first, vp, scissors);
}
}    // namespace v1
}    // namespace gfx