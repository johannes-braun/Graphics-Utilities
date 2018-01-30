#pragma once

#include <glad/glad.h>
#include <stdexcept>
#include <jpu/memory>
#include <jpu/flags>

namespace gl
{
    enum class buffer_flag_bits : uint32_t
    {
        dynamic_storage = GL_DYNAMIC_STORAGE_BIT,
        map_read = GL_MAP_READ_BIT,
        map_write = GL_MAP_WRITE_BIT,
        map_persistent = GL_MAP_PERSISTENT_BIT,
        map_coherent = GL_MAP_COHERENT_BIT,
        client_storage = GL_CLIENT_STORAGE_BIT,
        map_dynamic_persistent = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT,
        map_dynamic_persistent_read = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT,
        map_dynamic_persistent_write = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT,
    };
    using buffer_flags = jpu::flags<uint32_t, buffer_flag_bits>;

    class buffer : public jpu::ref_count
    {
    public:
        buffer(const size_t size, const buffer_flags flags = {});
        
        template<typename TContainer, typename = decltype(std::data(std::declval<TContainer>()))>
        buffer(TContainer data, const buffer_flags flags = {});

        template<typename TValue>
        buffer(TValue* data, const size_t count, const buffer_flags flags = {});

        ~buffer();
        operator bool() const;
        operator unsigned() const;

        template<typename TContainer, typename = decltype(std::data(std::declval<TContainer>()))>
        void assign(TContainer data, size_t offset_bytes = 0) const;
        template<typename TValue>
        void assign(TValue* data, const size_t count, size_t offset_bytes = 0) const;

        void clear_to_float(const float value) const;

        template<typename T>
        T& at(size_t position) const;
        
        template<typename T>
        T* data_as() const;

        void map();
        void unmap() const;
        void flush(const size_t size_bytes, const size_t offset_bytes) const;
        void bind(const uint32_t binding_point, const GLenum type) const;

        size_t size() const;
        uint64_t address() const;

    private:
        void allocate(const size_t size, const void* data, const buffer_flags flags) const;
        void map_if_needed();
        void make_persistent_address();

        uint32_t _id;
        size_t _size;
        void* _mapped_data = nullptr;
        buffer_flags _flags;
        uint32_t _map_access{ 0 };
        uint64_t _persistent_address;
    };

    template <typename TContainer, typename>
    buffer::buffer(TContainer data, const buffer_flags flags): buffer(std::data(data), std::size(data), flags)
    {
    }

    template <typename TValue>
    buffer::buffer(TValue* data, const size_t count, const buffer_flags flags): _size(count * sizeof(TValue)),
                                                                                _flags(flags)
    {
        glCreateBuffers(1, &_id);
        allocate(count * sizeof(TValue), data, flags);
        map_if_needed();
        make_persistent_address();
    }

    template <typename TContainer, typename>
    void buffer::assign(TContainer data, size_t offset_bytes) const
    {
        glNamedBufferSubData(_id, offset_bytes, std::size(data), std::data(data));
    }

    template <typename TValue>
    void buffer::assign(TValue* data, const size_t count, size_t offset_bytes) const
    {
        glNamedBufferSubData(_id, offset_bytes, count * sizeof(TValue), data);
    }

    template <typename T>
    T& buffer::at(size_t position) const
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
