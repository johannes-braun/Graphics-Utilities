#pragma once
#include <stdexcept>

namespace gl
{
    template <typename TContainer, typename>
    buffer::buffer(TContainer data, const buffer_flags flags) : buffer(std::data(data), std::size(data), flags)
    {}

    template <typename TValue>
    buffer::buffer(TValue* data, const size_t count, const buffer_flags flags) : _size(count * sizeof(TValue)),
        _flags(flags)
    {
        glCreateBuffers(1, &_id);
        allocate(count * sizeof(TValue), data, flags);
        map_if_needed();
        make_persistent_address();
    }

    template <typename TContainer, typename>
    void buffer::assign(TContainer data, const size_t offset_bytes) const
    {
        glNamedBufferSubData(_id, offset_bytes, std::size(data), std::data(data));
    }

    template <typename TValue>
    void buffer::assign(TValue* data, const size_t count, const size_t offset_bytes) const
    {
        glNamedBufferSubData(_id, offset_bytes, count * sizeof(TValue), data);
    }

    template <typename T>
    T& buffer::at(const size_t position) const
    {
        if (!_mapped_data || position * sizeof(T) >= _size)
            throw std::range_error("Position references an invalid position.");

        return static_cast<T*>(_mapped_data)[position];
    }

    template <typename T>
    T* buffer::data_as() const
    {
        return static_cast<T*>(_mapped_data);
    }
}