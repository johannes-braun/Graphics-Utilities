#pragma once

#include "descriptor.hpp"
#include "pipeline.hpp"

namespace gfx
{
struct depth_stencil
{
    float    depth;
    uint32_t stencil;
};
using clear_value = std::variant<glm::vec4, depth_stencil>;

namespace detail
{
class commands_implementation
{
public:
    virtual ~commands_implementation()                                                                              = default;
    virtual void bind_graphics_pipeline(graphics_pipeline& pipeline)                                                = 0;
    virtual void bind_vertex_buffer(uint32_t binding, std::any buffer_handle, ptrdiff_t offset, uint32_t stride)    = 0;
    virtual void bind_index_buffer(std::any buffer_handle, index_type type, ptrdiff_t offset)                       = 0;
    virtual void draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) = 0;
    virtual void draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                              ptrdiff_t first_instance)                                                             = 0;
    virtual void reset()                                                                                            = 0;
    virtual void execute()                                                                                          = 0;

    // TODO(s):
    virtual void bind_descriptors(descriptor_set* sets, int count)                             = 0;
    virtual void begin_pass(clear_value* values, int value_count, /*tmp*/ std::any fbo_handle) = 0;
    virtual void set_viewports(gfx::viewport* vps, int count, int first)                                  = 0;
};
std::unique_ptr<commands_implementation> make_commands_implementation();
}

class commands
{
public:
    commands() : _implementation(detail::make_commands_implementation()) {}


    template<typename T>
    void bind_vertex_buffer(uint32_t binding, device_buffer<T>& buffer, ptrdiff_t offset = 0, uint32_t stride = sizeof(T));

    template<typename T>
    void bind_index_buffer(device_buffer<T>& buffer, index_type type, ptrdiff_t offset = 0);

    void bind_pipeline(graphics_pipeline& ppl) const;
    void draw(size_t vertex_count, size_t instance_count = 1, ptrdiff_t first_vertex = 0, ptrdiff_t first_instance = 0) const;
    void draw_indexed(size_t index_count, size_t instance_count = 1, ptrdiff_t first_index = 0, ptrdiff_t first_vertex = 0,
                      ptrdiff_t first_instance = 0) const;

    void reset() const;
    void execute() const;

    // TODO:
    void set_viewports(gfx::viewport* vps, int count, int first);
    void bind_descriptors(descriptor_set* sets, int count) const;    // etc...
    void begin_pass(clear_value* clear_values, int value_count, /*tmp*/ std::any fbo) const;

private:
    std::unique_ptr<detail::commands_implementation> _implementation;
};

template<typename T>
void commands::bind_vertex_buffer(uint32_t binding, device_buffer<T>& buffer, ptrdiff_t offset, uint32_t stride)
{
    _implementation->bind_vertex_buffer(binding, buffer.api_handle(), offset, stride);
}

template<typename T>
void commands::bind_index_buffer(device_buffer<T>& buffer, index_type type, ptrdiff_t offset)
{
    _implementation->bind_index_buffer(buffer.api_handle(), type, offset);
}
}
