#include "commands.hpp"
#include "opengl/commands_opengl.hpp"

namespace gfx
{
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

void commands::draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) const
{
    implementation()->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void commands::draw_indexed(size_t    index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                            ptrdiff_t first_instance) const
{
    implementation()->draw_indexed(index_count, instance_count, first_index, first_vertex, first_instance);
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
    implementation()->execute();
}

void commands::set_viewports(gfx::viewport* vps, int count, int first)
{
    implementation()->set_viewports(vps, count, first);
}

void commands::begin_pass(clear_value* clear_values, int value_count, std::any fbo) const
{
    implementation()->begin_pass(clear_values, value_count, fbo);
}
}
