#pragma once

#include "../vertex_input.hpp"
#include <mygl/mygl.hpp>
#include <unordered_map>

namespace gfx::opengl
{
class vertex_input_implementation : public detail::vertex_input_implementation
{
public:
    vertex_input_implementation();
    ~vertex_input_implementation();
    uint32_t add_attribute(uint32_t binding, gfx::format fmt, size_t offset) override;
    void     set_binding_info(uint32_t binding, size_t stride, input_rate rate) override;
    std::any api_handle() override;
    void     bind_vertex_buffer(uint32_t binding, const std::any& buffer_handle, ptrdiff_t offset) override;
    void     bind_index_buffer(const std::any& buffer_handle, index_type type, ptrdiff_t offset) override;

    void set_assembly(topology mode, bool enable_primitive_restart) override;
    void draw(uint32_t vertices, uint32_t instances, uint32_t base_vertex, uint32_t base_instance) override;
    void draw_indexed(uint32_t indices, uint32_t instances, uint32_t base_index, int32_t base_vertex, uint32_t base_instance) override;
    void draw_indexed_indirect(const std::any& buffer_handle, size_t offset, uint32_t draw_count, uint32_t stride) override;
private:
    mygl::vertex_array                   _handle;
    uint32_t                             _current_attrib = 0;
    std::unordered_map<uint32_t, size_t> _binding_strides;
    ptrdiff_t                            _index_buffer_offset;
    GLenum                               _draw_index_type;
    uint32_t                             _draw_index_type_size;

    GLenum _draw_mode                = GL_TRIANGLES;
    bool   _enable_primitive_restart = false;
};
}
