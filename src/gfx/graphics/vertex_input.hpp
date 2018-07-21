#pragma once

#include "device_buffer.hpp"
#include "formats.hpp"
#include <any>
#include <cinttypes>
#include <gfx/api.hpp>
#include <memory>

namespace gfx {
inline namespace v1 {
enum class index_type
{
    uint16 = 2,
    uint32 = 4
};

enum class input_rate
{
    vertex,
    instance
};

enum class topology
{
    point_list,
    line_list,
    line_strip,
    triangle_list,
    triangle_strip,
    triangle_fan,
    line_list_adj,
    line_strip_adj,
    triangle_list_adj,
    triangle_strip_adj,
    patch_list
};

namespace detail {
class vertex_input_implementation
{
public:
    static std::unique_ptr<vertex_input_implementation> make();

    virtual std::any api_handle()                                                                              = 0;
    virtual ~vertex_input_implementation()                                                                     = default;
    virtual uint32_t add_attribute(uint32_t binding, gfx::format fmt, size_t offset)                           = 0;
    virtual void     set_binding_info(uint32_t binding, size_t stride, input_rate rate)                        = 0;
    virtual void     bind_vertex_buffer(uint32_t binding, const std::any& buffer_handle, ptrdiff_t offset)     = 0;
    virtual void     bind_index_buffer(const std::any& buffer_handle, index_type type, ptrdiff_t offset)       = 0;
    virtual void     set_assembly(topology mode, bool enable_primitive_restart = true)                         = 0;
    virtual void     draw(uint32_t vertices, uint32_t instances, uint32_t base_vertex, uint32_t base_instance) = 0;
    virtual void draw_indexed(uint32_t indices, uint32_t instances, uint32_t base_index, int32_t base_vertex, uint32_t base_instance) = 0;
    virtual void draw_indexed_indirect(const std::any& buffer_handle, size_t offset, uint32_t draw_count, uint32_t stride)            = 0;
};
}    // namespace detail

GFX_api_cast_type(gapi::opengl, vertex_input, mygl::vertex_array);

// TODO: for VK: combine to provider for VkPipelineVertexInputStateCreateInfo and VkPipelineInputAssemblyStateCreateInfo + draw
class vertex_input : public impl::implements<detail::vertex_input_implementation>
{
public:
    explicit vertex_input(topology mode = topology::triangle_list, bool enable_primitive_restart = false);
    vertex_input(vertex_input&&) = default;
    vertex_input& operator=(vertex_input&&) = default;

    uint32_t add_attribute(uint32_t binding, gfx::format fmt, size_t offset);
    void     set_binding_info(uint32_t binding, size_t stride, input_rate rate);

    template<typename T>
    void bind_vertex_buffer(uint32_t binding, const device_buffer<T>& buffer, ptrdiff_t offset = 0) const;
    template<typename T>
    void bind_index_buffer(const device_buffer<T>& buffer, index_type type, ptrdiff_t offset = 0) const;

    void set_assembly(topology mode, bool enable_primitive_restart = false);

    [[deprecated("Should be replaced by some pipeline state system.")]] void
                                                                             draw(uint32_t vertices, uint32_t instances = 1, uint32_t base_vertex = 0, uint32_t base_instance = 0) const;
    [[deprecated("Should be replaced by some pipeline state system.")]] void draw_indexed(uint32_t indices, uint32_t instances = 1,
                                                                                          uint32_t base_index = 0, int32_t base_vertex = 0,
                                                                                          uint32_t base_instance = 0) const;

    template<typename T>
    [[deprecated("Should be replaced by some pipeline state system.")]] void
        draw_indexed_indirect(const device_buffer<T>& buffer, size_t offset, uint32_t draw_count, uint32_t stride) const;

    topology assembly_topology() const noexcept { return _topology; }

private:
    topology _topology;

public:
    GFX_api_cast_op(gapi::opengl, vertex_input);
};

template<typename T>
void vertex_input::bind_vertex_buffer(uint32_t binding, const device_buffer<T>& buffer, ptrdiff_t offset) const
{
    _implementation->bind_vertex_buffer(binding, buffer._implementation->api_handle(), offset);
}

template<typename T>
void vertex_input::bind_index_buffer(const device_buffer<T>& buffer, index_type type, ptrdiff_t offset) const
{
    _implementation->bind_index_buffer(buffer._implementation->api_handle(), type, offset);
}

template<typename T>
void vertex_input::draw_indexed_indirect(const device_buffer<T>& buffer, size_t offset, uint32_t draw_count, uint32_t stride) const
{
    _implementation->draw_indexed_indirect(buffer._implementation->api_handle(), offset, draw_count, stride);
}

GFX_api_cast_impl(gapi::opengl, vertex_input)
}    // namespace v1
}    // namespace gfx
