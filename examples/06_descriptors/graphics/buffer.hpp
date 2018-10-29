#pragma once

#include "types.hpp"
#include <unordered_set>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "queue.hpp"

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
	explicit mapped(device& d, gsl::span<const T> ilist);

	mapped(const mapped& other);
	mapped(mapped&& other) noexcept;
	mapped& operator=(const mapped& other);
	mapped& operator=(mapped&& other) noexcept;
	~mapped();

	void reserve(size_type capacity);
	void resize(size_type size, T&& value);
	void resize(size_type size, const T& value);

	void push_back(T&& value);
	void push_back(const T& value);
	void pop_back();

    template<typename... Args>
	T& emplace_back(Args&&... args);

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
	void resize(size_type size);

	value_type&       front();
	value_type&       back();
	const value_type& front() const;
	const value_type& back() const;
	size_type         capacity() const;
	void              shrink_to_fit();
	void              clear();

	iterator insert(const_iterator at, const T& value);
	iterator insert(const_iterator at, T&& value);
    template<class InputIt>
	iterator insert(const_iterator at, InputIt begin, InputIt end);
	iterator insert(const_iterator at, std::initializer_list<T> ilist);
	iterator erase(const_iterator at);
	iterator erase(const_iterator begin, const_iterator end);

	const vk::Buffer& get_buffer() const;

private:
	void allocate(size_type capacity, bool force = false);
	void init_range(iterator begin, iterator end, T&& value);
	void reset_storage(value_type* storage, size_type size);

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
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using index_type = std::ptrdiff_t;
	using pointer = element_type * ;
	using reference = element_type & ;
	using size_type = index_type;
	using difference_type = ptrdiff_t;

    buffer(device& d);
    buffer(device& d, const mapped<T>& source);
    buffer(device& d, const mapped<T>& source, commands& transfer_cmd);
    buffer(device& d, const vk::ArrayProxy<const T>& proxy);
    buffer(device& d, const vk::ArrayProxy<const T>& proxy, commands& transfer_cmd);
    buffer(device& d, const std::initializer_list<T>& source);
    buffer(device& d, const std::initializer_list<T>& source, commands& transfer_cmd);

    ~buffer();

    buffer(const buffer& other);
    buffer(buffer&& other) noexcept;
    buffer& operator=(const buffer& other);
    buffer& operator=(buffer&& other) noexcept;

    mapped<T> download() const;
    size_type size() const;
    size_type capacity() const;
    const vk::Buffer& get_buffer() const;

private:
    void allocate(size_type capacity, bool force, vk::CommandBuffer copy_cmd);

	std::vector<u32> _families;
	vk::Buffer       _buffer = nullptr;
	VmaAllocation    _allocation = nullptr;
	size_type        _capacity = 0;
	size_type		 _size = 0;
	device*          _device = nullptr;
};

template<typename Buf> constexpr bool is_buffer_v = false;
template<typename T> constexpr bool is_buffer_v<mapped<T>> = true;
template<typename T> constexpr bool is_buffer_v<buffer<T>> = true;
}    // namespace v1
}    // namespace gfx

#include "commands.hpp"
#include "buffer.inl"