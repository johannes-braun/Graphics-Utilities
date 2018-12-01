#pragma once

#include "types.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <unordered_set>
#include "device.hpp"

namespace gfx {
inline namespace v1 {
template<typename T>
class mapped : public gsl::span<T>
{
public:
    using element_type           = T;
    using value_type             = std::remove_cv_t<T>;
    using index_type             = std::ptrdiff_t;
    using pointer                = element_type*;
    using reference              = element_type&;
    using iterator               = gsl::details::span_iterator<gsl::span<T, gsl::dynamic_extent>, false>;
    using const_iterator         = gsl::details::span_iterator<gsl::span<T, gsl::dynamic_extent>, true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type              = index_type;
    using difference_type        = ptrdiff_t;

    explicit mapped(device& d);

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
    explicit mapped(device& d, size_type size);
    explicit mapped(device& d, size_type size, T&& value);
    explicit mapped(device& d, std::initializer_list<T> ilist);
    explicit mapped(device& d, gsl::span<T const> ilist);

    mapped(mapped const& other);
    mapped(mapped&& other) noexcept;
    mapped& operator=(mapped const& other);
    mapped& operator=(mapped&& other) noexcept;
    ~mapped();

    void reserve(size_type capacity);
    void resize(size_type size, T&& value);
    void resize(size_type size, T const& value);

    void push_back(T&& value);
    void push_back(T const& value);
    void pop_back();

    template<typename... Args>
    [[maybe_unused]] auto emplace_back(Args&&... args) -> T&;

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
    void resize(size_type size);

    [[nodiscard]] auto front() -> value_type&;
    [[nodiscard]] auto back() -> value_type&;
    [[nodiscard]] auto front() const -> const value_type&;
    [[nodiscard]] auto back() const -> const value_type&;
    [[nodiscard]] auto capacity() const noexcept -> size_type;
    void               shrink_to_fit();
    void               clear();

    [[maybe_unused]] auto insert(const_iterator at, T const& value) -> iterator;
    [[maybe_unused]] auto insert(const_iterator at, T&& value) -> iterator;
    template<class InputIt>
    [[maybe_unused]] auto insert(const_iterator at, InputIt begin, InputIt end) -> iterator;
    [[maybe_unused]] auto insert(const_iterator at, std::initializer_list<T> ilist) -> iterator;
    [[maybe_unused]] auto erase(const_iterator at) -> iterator;
    [[maybe_unused]] auto erase(const_iterator begin, const_iterator end) -> iterator;

    [[nodiscard]] auto get_buffer() const -> vk::Buffer const&;

private:
    void allocate(size_type capacity, bool force = false);
    void init_range(iterator begin, iterator end, T&& value);
    void init_range(iterator begin, iterator end, T const& value);
    void reset_storage(value_type* storage, size_type size) noexcept;

    std::vector<u32> _families;
    vk::Buffer       _buffer     = nullptr;
    VmaAllocation    _allocation = nullptr;
    size_type        _capacity   = 0;
    device*          _device     = nullptr;
};

template<typename T>
class buffer
{
public:
    using element_type    = T;
    using value_type      = std::remove_cv_t<T>;
    using index_type      = std::ptrdiff_t;
    using pointer         = element_type*;
    using reference       = element_type&;
    using size_type       = index_type;
    using difference_type = ptrdiff_t;

    buffer(device& d);
    buffer(device& d, mapped<T> const& source);
    buffer(device& d, mapped<T> const& source, commands& transfer_cmd);
    buffer(device& d, vk::ArrayProxy<T const> const& proxy);
    buffer(device& d, vk::ArrayProxy<T const> const& proxy, commands& transfer_cmd);
    buffer(device& d, std::initializer_list<T> const& source);
    buffer(device& d, std::initializer_list<T> const& source, commands& transfer_cmd);

    ~buffer();

    buffer(buffer const& other);
    buffer(buffer&& other) noexcept;
    buffer& operator=(buffer const& other);
    buffer& operator=(buffer&& other) noexcept;

    [[nodiscard]] auto download() const -> mapped<T>;
    [[nodiscard]] auto size() const -> size_type;
    [[nodiscard]] auto capacity() const -> size_type;
    [[nodiscard]] auto get_buffer() const -> vk::Buffer const&;

private:
    void allocate(size_type capacity, bool force, vk::CommandBuffer copy_cmd);

    std::vector<u32> _families;
    vk::Buffer       _buffer     = nullptr;
    VmaAllocation    _allocation = nullptr;
    size_type        _capacity   = 0;
    size_type        _size       = 0;
    device*          _device     = nullptr;
};

template<typename Buf>
constexpr bool is_buffer_v = false;
template<typename T>
constexpr bool is_buffer_v<mapped<T>> = true;
template<typename T>
constexpr bool is_buffer_v<buffer<T>> = true;
}    // namespace v1
}    // namespace gfx

#include "buffer.inl"