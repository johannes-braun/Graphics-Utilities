#pragma once

#include "implementation.hpp"
#include <algorithm>
#include <any>
#include <execution>
#include "../api.hpp"
#include <gfx.core/types.hpp>
#include <gsl/span>
#include <memory>
#include <numeric>

namespace std {
enum class byte : uint8_t;
}

namespace gfx {
inline namespace v1 {
namespace detail {
template<typename C, typename TBase>
using enable_if_container = std::void_t<decltype(std::data(std::declval<C>())), decltype(std::size(std::declval<C>())),
                                        std::enable_if_t<!std::is_same_v<std::initializer_list<TBase>, std::decay_t<C>>>>;

class host_buffer_implementation
{
public:
    static std::unique_ptr<host_buffer_implementation> make();

    using size_type                       = uint64_t;
    using difference_type                 = int64_t;
    virtual ~host_buffer_implementation() = default;

    struct allocation
    {
        void*    data;
        std::any handle;
    };

    virtual allocation allocate(size_type size)            = 0;
    virtual void       deallocate(const allocation& alloc) = 0;

    virtual std::any api_handle() = 0;
};
}    // namespace detail

template<typename T>
class device_buffer;

// buffer for host memory (mapped/contiguous)
template<typename T>
class host_buffer : public impl::implements<detail::host_buffer_implementation>
{
public:
    friend class device_buffer<T>;
    using iterator_category                    = std::random_access_iterator_tag;
    using size_type                            = int64_t;
    using difference_type                      = int64_t;
    using value_type                           = T;
    using pointer                              = value_type*;
    using const_pointer                        = const value_type*;
    using reference                            = value_type&;
    using const_reference                      = const value_type&;
    constexpr const static size_type type_size = sizeof(value_type);

    using iterator               = typename gsl::span<T>::iterator;
    using const_iterator         = typename gsl::span<T>::const_iterator;
    using reverse_iterator       = typename gsl::span<T>::reverse_iterator;
    using const_reverse_iterator = typename gsl::span<T>::const_reverse_iterator;

    host_buffer();
    template<typename = std::enable_if_t<std::is_default_constructible_v<value_type>>>
    host_buffer(size_type elements);
    host_buffer(size_type elements, const_reference base);
    template<typename Iter>
    host_buffer(Iter begin, Iter end);
    host_buffer(const std::initializer_list<T>& elements);
    template<typename Container, typename = detail::enable_if_container<Container, T>>
    host_buffer(const Container& elements);
    ~host_buffer()
    {
		if (implementation())
		{
			implementation()->deallocate(_allocation);
			for (auto& elem : *this) {
				elem.~value_type();
			}
		}
    }

    host_buffer(host_buffer&&) = default;
    host_buffer& operator=(host_buffer&&) = default;

    reference       at(size_type index);
    const_reference at(size_type index) const;
    reference       operator[](size_type index);
    const_reference operator[](size_type index) const;

    size_type     size() const noexcept;
    size_type     capacity() const noexcept;
    bool          empty() const noexcept;
    pointer       data() noexcept;
    const_pointer data() const noexcept;

    iterator               begin() noexcept;
    iterator               end() noexcept;
    const_iterator         begin() const noexcept;
    const_iterator         end() const noexcept;
    const_iterator         cbegin() const noexcept;
    const_iterator         cend() const noexcept;
    reverse_iterator       rbegin() noexcept;
    reverse_iterator       rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // Utility functions
    iterator       find(const_reference object) noexcept;
    const_iterator find(const_reference object) const noexcept;
    bool           contains(const_reference object) const noexcept;

    template<typename = std::enable_if_t<std::is_default_constructible_v<value_type>>>
    void resize(size_type elements);
    void resize(size_type elements, const_reference base);
    void reserve(size_type size);

	void clear() { _data_span = gsl::make_span(_data_span.data(), 0); }

    void shrink_to_fit();

    void push_back(value_type&& value);
	void push_back(const value_type& value);
	void pop_back() { 
		_data_span[size()-1].~value_type();
		_data_span ={ _data_span.data(), _data_span.size()-1 };
	}

    template<typename... Args, typename = decltype(value_type(std::declval<Args>()...))>
    T& emplace_back(Args&&... args);

private: 
    static const std::byte* byte_ptr(const_pointer ptr);
    static std::byte*    byte_ptr(pointer ptr);
    static const_pointer type_ptr(const std::byte* ptr);
    static pointer       type_ptr(std::byte* ptr);

    detail::host_buffer_implementation::allocation _allocation = {nullptr, {}};
    size_type                                      _capacity   = 0;
    gsl::span<T>                                        _data_span;
};

}    // namespace v1
}    // namespace gfx

#include "host_buffer.inl"
