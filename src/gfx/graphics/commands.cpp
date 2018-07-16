#include "commands.hpp"
#include "opengl/commands_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::commands_implementation> detail::make_commands_implementation()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::commands_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::host_buffer_implementation>();
    default: break;
    }
    return nullptr;
}

void commands::bind_pipeline(graphics_pipeline& ppl) const
{
    _implementation->bind_graphics_pipeline(ppl);
}

void commands::draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) const
{
    _implementation->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void commands::draw_indexed(size_t    index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                            ptrdiff_t first_instance) const
{
    _implementation->draw_indexed(index_count, instance_count, first_index, first_vertex, first_instance);
}

void commands::bind_descriptors(descriptor_set* sets, int count) const
{
    _implementation->bind_descriptors(sets, count);
}

void commands::reset() const
{
    _implementation->reset();
}

void commands::execute() const
{
    _implementation->execute();
}

void commands::set_viewports(gfx::viewport* vps, int count, int first)
{
    _implementation->set_viewports(vps, count, first);
}

void commands::begin_pass(clear_value* clear_values, int value_count, std::any fbo) const
{
    _implementation->begin_pass(clear_values, value_count, fbo);
}
}
