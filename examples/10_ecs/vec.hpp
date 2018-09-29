#pragma once

#include <array>
#include <glm/glm.hpp>

namespace gfx {
inline namespace v1 {
namespace detail {
template<typename T, size_t S>
struct vec_components
{
    T components[S];
};

template<typename T>
struct vec_components<T, 1>
{
    union
    {
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
        T components[1];
    };
};

template<typename T>
struct vec_components<T, 2>
{
    union
    {
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
        T components[2];
    };
};

template<typename T>
struct vec_components<T, 3>
{
    union
    {
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
        T components[3];
    };
};

template<typename T>
struct vec_components<T, 4>
{
    union
    {
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
        T components[4]{0};
    };
};

}    // namespace detail

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

    using iterator               = typename std::array<value_type, S>::iterator;
    using const_iterator         = typename std::array<value_type, S>::const_iterator;
    using reverse_iterator       = typename std::array<value_type, S>::reverse_iterator;
    using const_reverse_iterator = typename std::array<value_type, S>::const_reverse_iterator;

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

    constexpr static size_t component_count = S;

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

    constexpr reference       at(size_type index);
    constexpr const_reference at(size_type index) const;
    constexpr reference       operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;
    constexpr pointer         data() noexcept;
    constexpr const_pointer   data() const noexcept;
    constexpr size_type       size() const noexcept;
    void fill(const T& value);

    constexpr operator const glm::vec<S, T>&() const noexcept;
    constexpr operator glm::vec<S, T>&() noexcept;

    constexpr vec(const glm::vec<S, T>& v) noexcept;

    constexpr vec& operator+=(const vec& other) noexcept;
    constexpr vec& operator-=(const vec& other) noexcept;
    constexpr vec& operator*=(const vec& other) noexcept;
    constexpr vec& operator/=(const vec& other) noexcept;
    constexpr vec& operator%=(const vec& other) const noexcept;

    constexpr vec operator+(const vec& other) const noexcept;
    constexpr vec operator-(const vec& other) const noexcept;
    constexpr vec operator*(const vec& other) const noexcept;
    constexpr vec operator/(const vec& other) const noexcept;
    constexpr vec operator%(const vec& other) const noexcept;

private:
    constexpr static struct glm_interop_t
    {
    } glm_interop = {};

    template<std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, T&& value) noexcept;
    template<typename X, size_type D, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept;
    template<typename X, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const X* other) noexcept;
    template<typename X, size_type D, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const glm_interop_t&, const glm::vec<D, X>& other) noexcept;

    template<std::size_t... Is>
    constexpr static auto op_add_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_sub_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_mul_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_div_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_mod_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_fmod_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_add_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_sub_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_mul_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_div_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_mod_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
    template<std::size_t... Is>
    constexpr static auto op_fmod_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept;
};

template<typename T>
class vec<T, 0> {};

template<typename T, std::size_t... Is>
constexpr T dot_impl(std::index_sequence<Is...>, const T* s1, const T* s2) noexcept
{
    return ((s1[Is] * s2[Is]) + ...);
}
template<typename T, size_t S>
constexpr auto dot(const vec<T, S>& a, const vec<T, S>& b) noexcept
{
    return dot_impl(std::make_index_sequence<S>(), a.components, b.components);
}

template<typename T>
constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept
{
    return vec<T, 3>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
}    // namespace v1
}    // namespace gfx

#include "vec.inl"