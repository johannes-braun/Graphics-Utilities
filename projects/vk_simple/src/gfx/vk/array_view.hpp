#pragma once

#include <vulkan/vulkan.h>
#include <type_traits>
#include <iterator>

namespace vk
{
    template<typename T>
    struct array_view
    {
        template<typename C, typename = std::enable_if_t<std::is_same_v<decltype(std::data(std::declval<const C>())), const T*>>>
        constexpr array_view(const C& container) noexcept : _data(std::data(container)), _size(std::size(container)) {}
        constexpr array_view(const std::initializer_list<float>& container) noexcept : _data(std::data(container)), _size(std::size(container)) {}
        constexpr array_view(const T& elem) noexcept : _data(&elem), _size(1) {}
        template<size_t S>
        constexpr array_view(const T (&elem)[S]) noexcept : _data(elem), _size(S) {}
        constexpr array_view(nullptr_t) noexcept : _data(nullptr), _size(0) {}
        constexpr array_view() noexcept : array_view(nullptr) {}

        size_t size() const noexcept { return _size; }
        const T* data() const noexcept { return _data; }

        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }

    private:
        const T* _data;
        size_t _size;
    };
}