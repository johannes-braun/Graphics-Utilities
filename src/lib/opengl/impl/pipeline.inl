#pragma once

namespace gl
{
    template<typename T>
    void pipeline::bind_attribute(uint32_t index, const buffer<T>& buffer, int components, GLenum type, bool normalized, size_t offset, size_t stride) const noexcept
    {
        glEnableVertexAttribArray(index);
        glVertexAttribFormatNV(index, components, type, normalized, int(stride));
        glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, index, buffer.handle() + offset, buffer.size() * sizeof(decltype(buffer[0])));
    }

    template<typename T>
    void pipeline::bind_attribute(uint32_t index, const buffer<T>& buffer, int components, GLenum type, size_t offset, size_t stride) const noexcept
    {
        bind_attribute(index, buffer, components, type, false, offset, stride);
    }

    template<typename T>
    void pipeline::bind_uniform_buffer(uint32_t index, const buffer<T>& buffer, size_t offset, size_t size) const noexcept
    {
        glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, index, buffer.handle() + offset, (size == ~0) ? buffer.size() * sizeof(decltype(buffer[0])) : size);
    }

    template<typename T>
    void pipeline::draw(GLenum primitive, const buffer<T>& index_buffer, GLenum type, size_t count, size_t first_index, size_t first_vertex) const noexcept
    {
        glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, 0ui64, std::numeric_limits<int64_t>::max());
        glDrawElementsBaseVertex(primitive, int(count == ~0 ? index_buffer.size() : count), type,
            reinterpret_cast<const void*>(index_buffer.handle() + first_index * std::max<size_t>(1, type - GL_UNSIGNED_BYTE)), int(first_vertex));
        glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, 0ui64, 1ui64);
    }

    template<typename T>
    void compute_pipeline::bind_uniform_buffer(uint32_t index, const buffer<T>& buffer, size_t offset, size_t size) const noexcept
    {
        glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, index, buffer.handle() + offset, (size == ~0) ? buffer.size() * sizeof(decltype(buffer[0])) : size);
    }
}
