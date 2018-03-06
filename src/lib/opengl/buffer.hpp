#pragma once

#include <array>
#include <glad/glad.h>
#include <jpu/memory>
#include <jpu/flags>
#include <jpu/log>

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
        template<typename... T>
        explicit buffer(const std::vector<T...>& data, buffer_flags flags = {});

        template<typename T, size_t S>
        explicit buffer(const std::array<T, S>& data, buffer_flags flags = {});

        template<typename T, typename = std::enable_if_t<!std::is_arithmetic_v<T>>>
        explicit buffer(const T& object, buffer_flags flags = {}) : buffer(&object, 1, flags) {}

        template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        explicit buffer(T size, buffer_flags flags = {}) : buffer(static_cast<uint8_t*>(nullptr), size, flags) {};

        template<typename TValue>
        buffer(TValue* data, size_t count, buffer_flags flags = {});

        ~buffer();
        operator unsigned() const;

        template<typename TContainer, typename = decltype(std::data(std::declval<TContainer>()))>
        void assign(TContainer data, size_t offset_bytes = 0) const;
        template<typename TValue>
        void assign(TValue* data, size_t count, size_t offset_bytes = 0) const;

        void clear_to_float(float value) const;

        template<typename T>
        T& at(size_t position) const;
        
        template<typename T>
        T* data_as() const;

        void map(uint32_t map_access);
        void unmap() const;
        void flush(size_t size_bytes, size_t offset_bytes) const;
        void bind(uint32_t binding_point, GLenum type) const;

        size_t size() const;
        uint64_t address() const;

    private:
        void allocate(size_t size, const void* data, buffer_flags flags) const;
        void map_if_needed();
        void make_persistent_address();

        uint32_t _id;
        size_t _size;
        void* _mapped_data = nullptr;
        buffer_flags _flags;
        uint32_t _map_access{ 0 };
        uint64_t _persistent_address;
    };
}

#include "buffer.inl"