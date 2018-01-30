#pragma once
#include <cinttypes>

namespace jpu
{
    struct position_attribute
    {
        uint32_t stride;
        uint32_t offset;
    };

    template<typename T>
    struct index_attribute
    {
        uint32_t stride = sizeof(T);
        uint32_t offset = 0;
        uint32_t count = 0;
        uint32_t per_object = 0;
    };

    template<typename T>
    struct data_ref
    {
        T* data_ptr;
        size_t size;
        size_t buffer_offset;
    };

    template<typename T, typename TMember>
    constexpr position_attribute make_position_attrib(TMember T::* field)
    {
        position_attribute attr;
        attr.stride = sizeof(T);
        attr.offset = static_cast<uint32_t>(reinterpret_cast<size_t>(&(reinterpret_cast<T*>(nullptr)->*field)));
        return attr;
    }

    template<typename T>
    constexpr index_attribute<T> make_index_attrib(uint32_t count, uint32_t per_object)
    {
        index_attribute<T> attr;
        attr.count = count;
        attr.per_object = per_object;
        return attr;
    }
}