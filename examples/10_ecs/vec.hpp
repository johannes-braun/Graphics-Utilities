#pragma once

#include <array>
#include <complex>

namespace gfx {
inline namespace v1 {
namespace detail {
	constexpr size_t max_static_vector_size = 24;

template<typename T, size_t S>
struct vec_components
{
    T components[S]{ 0 };
};

template<typename T>
struct vec_components<T, 1>
{
    union
    {
		T components[1]{ 0 };
        struct
        {
            T x;
        };
        struct
        {
            T r;
        };
        struct
        {
            T u;
        };
    };
};

template<typename T>
struct vec_components<T, 2>
{
    union
    {
		T components[2]{ 0 };
        struct
        {
            T x, y;
        };
        struct
        {
            T r, g;
        };
        struct
        {
            T u, v;
        };
    };
};

template<typename T>
struct vec_components<T, 3>
{
    union
    {
		T components[3]{ 0 };
        struct
        {
            T x, y, z;
        };
        struct
        {
            T r, g, b;
        };
        struct
        {
            T u, v, w;
        };
    };
};

template<typename T>
struct vec_components<T, 4>
{
    union
    {
		T components[4]{ 0 };
        struct
        {
            T x, y, z, w;
        };
        struct
        {
            T r, g, b, a;
        };
        struct
        {
            T u, v, s, t;
        };
    };
};

}    // namespace detail

template<typename T, size_t S>
class vec;

template<typename T>
struct vec_tuple;

template<size_t I, typename T>
struct vec_tuple<vec<T, I>>
{
    template<typename... Args>
    using type = typename vec_tuple<vec<T, I - 1>>::template type<T, Args...>;
};

template<typename T>
struct vec_tuple<vec<T, 0>>
{
    template<typename... Args>
    using type = std::tuple<Args...>;
};
template<typename T>
using vec_tuple_t = typename vec_tuple<T>::template type<>;

template<typename T, size_t S>
class vec : public detail::vec_components<T, S>
{
    enum elem_position
    {
        idx_x = 0,
        idx_y = 1,
        idx_z = 2,
        idx_w = 3
    };

public:
    using reference       = T&;
    using const_reference = const T&;
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = T*;
    using const_pointer   = const T*;

    using real_imag_type = decltype(std::real(std::declval<T>()));

    using iterator               = typename std::array<value_type, S>::iterator;
    using const_iterator         = typename std::array<value_type, S>::const_iterator;
    using reverse_iterator       = typename std::array<value_type, S>::reverse_iterator;
    using const_reverse_iterator = typename std::array<value_type, S>::const_reverse_iterator;

    constexpr static size_t component_count = S;

    auto begin() { return iterator(this->components, 0); }
    auto end() { return iterator(this->components, S); }
    auto begin() const { return const_iterator(this->components, 0); }
    auto end() const { return const_iterator(this->components, S); }
    auto cbegin() const { return const_iterator(this->components, 0); }
    auto cend() const { return const_iterator(this->components, S); }
    auto rbegin() { return reverse_iterator(this->components, 0); }
    auto rend() { return reverse_iterator(this->components, S); }
    auto rbegin() const { return const_reverse_iterator(this->components, 0); }
    auto rend() const { return const_reverse_iterator(this->components, S); }
    auto crbegin() const { return const_reverse_iterator(this->components, 0); }
    auto crend() const { return const_reverse_iterator(this->components, S); }

    explicit constexpr vec() noexcept;
    template<typename X, size_type D, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(const vec<X, D>& other) noexcept;
    template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(const X* ptr);
    template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(X* ptr);
    explicit constexpr vec(T&& value) noexcept;
    template<typename... Ts, typename = std::enable_if_t<S == sizeof...(Ts) && (S > 1)>>
    explicit constexpr vec(Ts&&... ts) noexcept;

private:
	template<std::size_t... Is, typename UnaryConvertFun>
	constexpr auto apply(std::index_sequence<Is...>, UnaryConvertFun&& fun) const noexcept;
	template<std::size_t... Is, typename UnaryConvertFun>
	constexpr auto apply(std::index_sequence<Is...>, const vec& other, UnaryConvertFun&& fun) const noexcept;

public:
	constexpr vec<real_imag_type, S> real() const noexcept;
	constexpr vec<real_imag_type, S> imag() const noexcept;

    constexpr reference       at(size_type index);
    constexpr const_reference at(size_type index) const;
    constexpr reference       operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;
    constexpr pointer         data() noexcept;
    constexpr const_pointer   data() const noexcept;
    constexpr size_type       size() const noexcept;
    constexpr void            fill(const T& value);
	
    constexpr operator vec_tuple_t<vec>&() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
    constexpr operator const vec_tuple_t<vec>&() noexcept { return reinterpret_cast<const vec_tuple_t<vec>&>(*this); }
    constexpr vec_tuple_t<vec>& tuple() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
	constexpr const vec_tuple_t<vec>& tuple() const noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
	
private:

    template<std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, T&& value) noexcept;
    template<typename X, size_type D, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept;
    template<typename X, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const X* other) noexcept;
};

template<typename T>
class vec<T, 0>
{};

template<typename T>
using tvec1 = vec<T, 1>;
template<typename T>
using tvec2 = vec<T, 2>;
template<typename T>
using tvec3 = vec<T, 3>;
template<typename T>
using tvec4 = vec<T, 4>;

template<typename T>
using tcvec1 = vec<std::complex<T>, 1>;
template<typename T>
using tcvec2 = vec<std::complex<T>, 2>;
template<typename T>
using tcvec3 = vec<std::complex<T>, 3>;
template<typename T>
using tcvec4 = vec<std::complex<T>, 4>;

using vec1    = tvec1<float>;
using vec2    = tvec2<float>;
using vec3    = tvec3<float>;
using vec4    = tvec4<float>;
using dvec1   = tvec1<double>;
using dvec2   = tvec2<double>;
using dvec3   = tvec3<double>;
using dvec4   = tvec4<double>;
using ivec1   = tvec1<int32_t>;
using ivec2   = tvec2<int32_t>;
using ivec3   = tvec3<int32_t>;
using ivec4   = tvec4<int32_t>;
using uvec1   = tvec1<uint32_t>;
using uvec2   = tvec2<uint32_t>;
using uvec3   = tvec3<uint32_t>;
using uvec4   = tvec4<uint32_t>;
using bvec1   = tvec1<bool>;
using bvec2   = tvec2<bool>;
using bvec3   = tvec3<bool>;
using bvec4   = tvec4<bool>;
using i8vec1  = tvec1<int8_t>;
using i8vec2  = tvec2<int8_t>;
using i8vec3  = tvec3<int8_t>;
using i8vec4  = tvec4<int8_t>;
using u8vec1  = tvec1<uint8_t>;
using u8vec2  = tvec2<uint8_t>;
using u8vec3  = tvec3<uint8_t>;
using u8vec4  = tvec4<uint8_t>;
using i16vec1 = tvec1<int16_t>;
using i16vec2 = tvec2<int16_t>;
using i16vec3 = tvec3<int16_t>;
using i16vec4 = tvec4<int16_t>;
using u16vec1 = tvec1<uint16_t>;
using u16vec2 = tvec2<uint16_t>;
using u16vec3 = tvec3<uint16_t>;
using u16vec4 = tvec4<uint16_t>;

template<typename T, size_t S>
constexpr T dot(const vec<T, S>& a, const vec<T, S>& b) noexcept;
template<typename T, size_t S>
constexpr auto length(const vec<T, S>& a) noexcept;
template<typename T, size_t S>
constexpr auto normalize(const vec<T, S>& a) noexcept;
template<typename T>
constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept;
template<size_t S>
constexpr bool any_of(const vec<bool, S>& v);
template<size_t S>
constexpr bool all_of(const vec<bool, S>& v);
template<size_t S>
constexpr bool none_of(const vec<bool, S>& v);
template<typename T, size_t S>
constexpr auto min_element(const vec<T, S>& v);
template<typename T, size_t S>
constexpr auto max_element(const vec<T, S>& v);
template<typename T, size_t S>
constexpr auto minmax_element(const vec<T, S>& v);
}    // namespace v1
}    // namespace gfx

#include "vec/vec_apply.inl"
#include "vec/vec.inl"
#include "vec/vec_get.inl"
#include "vec/vec_ops.inl"
#include "vec/vec_std.inl"