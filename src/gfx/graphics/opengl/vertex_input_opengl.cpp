#include "formats.hpp"
#include "vertex_input_opengl.hpp"
#include <gfx/log.hpp>

namespace gfx
{
opengl::vertex_input_implementation::vertex_input_implementation() { glCreateVertexArrays(1, &_handle); }

opengl::vertex_input_implementation::~vertex_input_implementation()
{
    if(_handle != mygl::vertex_array::zero)
        glDeleteVertexArrays(1, &_handle);
}

uint32_t opengl::vertex_input_implementation::add_attribute(uint32_t binding, gfx::format fmt, size_t offset)
{
    uint32_t attr = _current_attrib++;
    glEnableVertexArrayAttrib(_handle, attr);

    format_info info = format_from(fmt);

    int count = [&]() {
        switch(info.external_format)
        {
        case GL_RED:
            return 1;
        case GL_RG:
            return 2;
        case GL_RGB:
            return 3;
        case GL_RGBA:
            return 4;
        default:
            return 0;
        }
    }();

    glVertexArrayAttribFormat(_handle, attr, count, info.external_type, info.normalized, static_cast<uint32_t>(offset));
    glVertexArrayAttribBinding(_handle, attr, binding);
    return attr;
}

void opengl::vertex_input_implementation::set_binding_info(uint32_t binding, size_t stride, input_rate rate)
{
    glVertexArrayBindingDivisor(_handle, binding, static_cast<uint32_t>(rate));
    _binding_strides[binding] = stride;
}

std::any opengl::vertex_input_implementation::api_handle() { return _handle; }

void opengl::vertex_input_implementation::bind_vertex_buffer(uint32_t binding, const std::any& buffer_handle, ptrdiff_t offset)
{
    mygl::buffer buf = std::any_cast<mygl::buffer>(buffer_handle);
    glVertexArrayVertexBuffer(_handle, binding, buf, static_cast<GLintptr>(offset), static_cast<int>(_binding_strides[binding]));
}

void opengl::vertex_input_implementation::bind_index_buffer(const std::any& buffer_handle, index_type type, ptrdiff_t offset)
{
    mygl::buffer buf = std::any_cast<mygl::buffer>(buffer_handle);
    glVertexArrayElementBuffer(_handle, buf);
    _index_buffer_offset = offset;
    _draw_index_type     = [&]() {
        switch(type)
        {
        case index_type::uint16:
            return GL_UNSIGNED_SHORT;
        case index_type::uint32:
            return GL_UNSIGNED_INT;
        default:
            return GLenum(0);
        }
    }();
    _draw_index_type_size = static_cast<uint32_t>(type);
}

void opengl::vertex_input_implementation::set_assembly(topology mode, bool enable_primitive_restart)
{
    _draw_mode = [&]() {
        switch(mode)
        {
        case topology::point_list:
            return GL_POINTS;
        case topology::line_list:
            return GL_LINES;
        case topology::line_strip:
            return GL_LINE_STRIP;
        case topology::triangle_list:
            return GL_TRIANGLES;
        case topology::triangle_strip:
            return GL_TRIANGLE_STRIP;
        case topology::triangle_fan:
            return GL_TRIANGLE_FAN;
        case topology::line_list_adj:
            return GL_LINES_ADJACENCY;
        case topology::line_strip_adj:
            return GL_LINE_STRIP_ADJACENCY;
        case topology::triangle_list_adj:
            return GL_TRIANGLES_ADJACENCY;
        case topology::triangle_strip_adj:
            return GL_TRIANGLE_STRIP_ADJACENCY;
        case topology::patch_list:
            return GL_PATCHES;
        default:
            return GLenum(0);
        }
    }();
    _enable_primitive_restart = enable_primitive_restart;
}

void opengl::vertex_input_implementation::draw(uint32_t vertices, uint32_t instances, uint32_t base_vertex, uint32_t base_instance)
{
    glBindVertexArray(_handle);
    glDrawArraysInstancedBaseInstance(_draw_mode, base_vertex, vertices, instances, base_instance);
    glBindVertexArray(mygl::vertex_array::zero);
}

void opengl::vertex_input_implementation::draw_indexed(uint32_t indices, uint32_t instances, uint32_t base_index, int32_t base_vertex,
                                                       uint32_t base_instance)
{
    _enable_primitive_restart ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
    glBindVertexArray(_handle);
    glDrawElementsInstancedBaseVertexBaseInstance(
            _draw_mode,
            indices,
            _draw_index_type,
            static_cast<std::byte*>(nullptr) + _draw_index_type_size * base_index + _index_buffer_offset,
            instances,
            base_vertex,
            base_instance);
    glBindVertexArray(mygl::vertex_array::zero);
    glDisable(GL_PRIMITIVE_RESTART);
}

void opengl::vertex_input_implementation::draw_indexed_indirect(const std::any& buffer_handle, size_t offset, uint32_t draw_count,
    uint32_t stride)
{
    _enable_primitive_restart ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
    glBindVertexArray(_handle);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, std::any_cast<mygl::buffer>(buffer_handle));
    glMultiDrawElementsIndirect(
            _draw_mode, _draw_index_type, reinterpret_cast<const void*>(offset), static_cast<int>(draw_count), static_cast<int>(stride));
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mygl::buffer::zero);
    glBindVertexArray(mygl::vertex_array::zero);
    glDisable(GL_PRIMITIVE_RESTART);
}
}
