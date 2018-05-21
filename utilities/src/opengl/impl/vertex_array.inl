#pragma once

namespace gl
{
template <typename T>
void vertex_array::vertex_buffer(int binding, const buffer<T>& buffer, ptrdiff_t offset,
                                 size_t stride)
{
    vertex_buffer(binding, static_cast<mygl::buffer>(buffer), offset, stride);
}

template <typename T> void vertex_array::element_buffer(const buffer<T>& buffer)
{
    element_buffer(static_cast<mygl::buffer>(buffer));
}
}