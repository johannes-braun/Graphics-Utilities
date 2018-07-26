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
	_state |= state_flag::has_graphics_pipeline;
	_state &= ~state_flags(state_flag::has_compute_pipeline);
}

void commands::bind_pipeline(compute_pipeline& ppl) const
{
	implementation()->bind_compute_pipeline(ppl);
	_state |= state_flag::has_compute_pipeline;
	_state &= ~state_flags(state_flag::has_graphics_pipeline);
}

void commands::draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) const
{
	assert((_state & state_flag::has_graphics_pipeline) == state_flag::has_graphics_pipeline);
    implementation()->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void commands::draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                            ptrdiff_t first_instance) const
{
	assert((_state & state_flag::has_graphics_pipeline) == state_flag::has_graphics_pipeline);
	assert((_state & state_flag::has_index_buffer) == state_flag::has_index_buffer);
    implementation()->draw_indexed(index_count, instance_count, first_index, first_vertex, first_instance);
}

void commands::dispatch_compute(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) const
{
	assert((_state & state_flag::has_compute_pipeline) == state_flag::has_compute_pipeline);
	implementation()->dispatch_compute(groups_x, groups_y, groups_z);
}

void commands::bind_descriptors(span<descriptor_set> sets) const
{
    implementation()->bind_descriptors(sets.data(), int(sets.size()));
}

void commands::reset() const
{
    implementation()->reset();
}

void commands::execute() const
{
	assert((_state & state_flag::in_pass) != state_flag::in_pass);
    implementation()->execute(nullptr);
}

void commands::execute(fence& f) const
{
	assert((_state & state_flag::in_pass) != state_flag::in_pass);
    implementation()->execute(&f);
}

void commands::set_viewports(span<viewport> viewports, int first) const
{
	assert((_state & state_flag::has_graphics_pipeline) == state_flag::has_graphics_pipeline);
	assert((_state & state_flag::in_pass) == state_flag::in_pass);
    implementation()->set_viewports(viewports.data(), int(viewports.size()), first);
}

void commands::begin_pass(framebuffer& fbo) const
{
	assert((_state & state_flag::in_pass) != state_flag::in_pass);
	_state |= state_flag::in_pass;
    implementation()->begin_pass(fbo);
}

void commands::end_pass() const
{
	assert((_state & state_flag::in_pass) == state_flag::in_pass);
	_state &= ~state_flags(state_flag::in_pass);
    implementation()->end_pass();
}
}    // namespace v1
}    // namespace gfx
