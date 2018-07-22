#include "commands.hpp"
#include "opengl/commands_opengl.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::commands_implementation> detail::commands_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::commands_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::host_bufferimplementation()>();
    default: break;
    }
    return nullptr;
}

void commands::bind_pipeline(graphics_pipeline& ppl) const
{
    implementation()->bind_graphics_pipeline(ppl);
}

void commands::bind_pipeline(compute_pipeline& ppl) const
{
	implementation()->bind_compute_pipeline(ppl);
}

void commands::draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) const
{
    implementation()->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void commands::draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                            ptrdiff_t first_instance) const
{
    implementation()->draw_indexed(index_count, instance_count, first_index, first_vertex, first_instance);
}

void commands::dispatch_compute(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) const
{
	implementation()->dispatch_compute(groups_x, groups_y, groups_z);
}

void commands::bind_descriptors(descriptor_set* sets, int count) const
{
    implementation()->bind_descriptors(sets, count);
}

void commands::reset() const
{
    implementation()->reset();
}

void commands::execute() const
{
    implementation()->execute(nullptr);
}

void commands::execute(fence& f) const
{
    implementation()->execute(&f);
}

void commands::set_viewports(gfx::viewport* vps, int count, int first) const
{
    implementation()->set_viewports(vps, count, first);
}

void commands::begin_pass(framebuffer& fbo) const
{
    implementation()->begin_pass(fbo);
}

void commands::end_pass() const
{
    implementation()->end_pass();
}
}    // namespace v1
}    // namespace gfx
