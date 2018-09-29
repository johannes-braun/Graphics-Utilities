#pragma once

#include <array>
#include <complex>
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
    constexpr auto convert_unary_imag_real(std::index_sequence<Is...>, UnaryConvertFun&& fun) const noexcept
    {
        return vec<real_imag_type, S>(fun(this->components[Is])...);
    }

public:
    constexpr vec<real_imag_type, S> real() const noexcept
    {
        return convert_unary_imag_real(std::make_index_sequence<S>{}, [](const auto& x) { return std::real(x); });
    }
    constexpr vec<real_imag_type, S> imag() const noexcept
    {
        return convert_unary_imag_real(std::make_index_sequence<S>{}, [](const auto& x) { return std::real(x); });
    }

    constexpr reference       at(size_type index);
    constexpr const_reference at(size_type index) const;
    constexpr reference       operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;
    constexpr pointer         data() noexcept;
    constexpr const_pointer   data() const noexcept;
    constexpr size_type       size() const noexcept;
    constexpr void            fill(const T& value);

    constexpr operator const glm::vec<S, T>&() const noexcept;
    constexpr operator glm::vec<S, T>&() noexcept;

    constexpr operator vec_tuple_t<vec>&() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
    constexpr operator const vec_tuple_t<vec>&() noexcept { return reinterpret_cast<const vec_tuple_t<vec>&>(*this); }

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

template<typename T, std::size_t... Is>
constexpr T dot_impl(std::index_sequence<Is...>, const T* s1, const T* s2) noexcept
{
    return ((s1[Is] * s2[Is]) + ...);
}
template<typename T, size_t S>
constexpr T dot(const vec<T, S>& a, const vec<T, S>& b) noexcept
{
    return dot_impl(std::make_index_sequence<S>(), a.components, b.components);
}
template<typename T, size_t S>
constexpr auto length(const vec<T, S>& a) noexcept
{
    return std::sqrt<T>(dot<T, S>(a, a)).real();
}

namespace detail {
template<typename UnaryConvertFun, typename T, size_t S, std::size_t... Is>
constexpr auto apply_for_each_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun());
    return vec<return_type, S>((static_cast<void>(Is), fun())...);
}
template<typename T, size_t S, typename UnaryConvertFun>
constexpr auto apply_for_each(UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_impl<UnaryConvertFun, T, S>(std::make_index_sequence<S>{}, std::forward<UnaryConvertFun&&>(fun));
}

template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(vector.components[0]));
    return vec<return_type, S>(fun(vector.components[Is])...);
}
template<typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each(const vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_impl(std::make_index_sequence<S>{}, vector, fun);
}
template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector1, const vec<T, S>& vector2,
                                   UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(vector1.components[0], vector2.components[0]));
    return vec<return_type, S>(fun(vector1.components[Is], vector2.components[Is])...);
}
template<typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each(const vec<T, S>& vector1, const vec<T, S>& vector2, UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_impl(std::make_index_sequence<S>{}, vector1, vector2, fun);
}
template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector1, const vec<T, S>& vector2, const vec<T, S>& vector3,
                                   UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(vector1.components[0], vector2.components[0], vector3.components[0]));
    return vec<return_type, S>(fun(vector1.components[Is], vector2.components[Is], vector3.components[Is])...);
}
template<typename UnaryConvertFun, typename T, size_t S>
constexpr auto apply_for_each(const vec<T, S>& vector1, const vec<T, S>& vector2, const vec<T, S>& vector3, UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_impl(std::make_index_sequence<S>{}, vector1, vector2, vector3, fun);
}
}    // namespace detail

template<typename T, size_t S>
constexpr auto normalize(const vec<T, S>& a) noexcept
{
    return detail::apply_for_each(a, [inv_len = T(1) / length(a)](const auto& val) { return val * inv_len; });
}

template<typename T>
constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept
{
    return vec<T, 3>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

template<size_t S>
constexpr bool any_of(const vec<bool, S>& v)
{
    return std::any_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<size_t S>
constexpr bool all_of(const vec<bool, S>& v)
{
    return std::all_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<size_t S>
constexpr bool none_of(const vec<bool, S>& v)
{
    return std::none_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<typename T, size_t S>
constexpr auto min_element(const vec<T, S>& v)
{
    return std::min_element(v.begin(), v.end(), std::less<T>());
}
template<typename T, size_t S>
constexpr auto max_element(const vec<T, S>& v)
{
    return std::max_element(v.begin(), v.end(), std::less<T>());
}
template<typename T, size_t S>
constexpr auto minmax_element(const vec<T, S>& v)
{
    return std::minmax_element(v.begin(), v.end(), std::less<T>());
}
}    // namespace v1
}    // namespace gfx

namespace std {
template<typename T, size_t S>
constexpr auto real(const gfx::vec<T, S>& a) noexcept
{
    return a.real();
}
template<typename T, size_t S>
constexpr auto imag(const gfx::vec<T, S>& a) noexcept
{
    return a.imag();
}
template<typename T, size_t S>
constexpr auto abs(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::abs(val); });
}
template<typename T, size_t S>
constexpr auto arg(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::arg(val); });
}
template<typename T, size_t S>
constexpr auto norm(const gfx::vec<T, S>& a) noexcept
{
    return gfx::dot(a, a);
}
template<typename T, size_t S>
constexpr auto conj(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::conj(val); });
}
template<typename T, size_t S>
constexpr auto proj(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::proj(val); });
}
template<typename T, size_t S>
constexpr auto exp(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::exp(val); });
}
template<typename T, size_t S>
constexpr auto exp2(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::exp2(val); });
}
template<typename T, size_t S>
constexpr auto expm1(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::expm1(val); });
}
template<typename T, size_t S>
constexpr auto log(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log(val); });
}
template<typename T, size_t S>
constexpr auto log2(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log2(val); });
}
template<typename T, size_t S>
constexpr auto log10(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log10(val); });
}
template<typename T, size_t S>
constexpr auto log1p(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log1p(val); });
}
template<typename T, size_t S>
constexpr auto pow(const gfx::vec<T, S>& a, const T& exponent) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& val) { return std::pow(val, exponent); });
}
template<typename T, size_t S>
constexpr auto pow(const gfx::vec<T, S>& a, const gfx::vec<T, S>& exponent) noexcept
{
    return gfx::detail::apply_for_each(a, exponent, [&](const auto& val, const auto& ex) { return std::pow(val, ex); });
}
template<typename T, size_t S>
constexpr auto sqrt(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sqrt(val); });
}
template<typename T, size_t S>
constexpr auto cqrt(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cqrt(val); });
}
template<typename T, size_t S>
constexpr auto hypot(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::hypot(va, vb); });
}
template<typename T, size_t S>
constexpr auto hypot(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b, const gfx::vec<T, S>& c) noexcept
{
    return gfx::detail::apply_for_each(a, b, c, [](const auto& va, const auto& vb, const auto& vc) { return std::hypot(va, vb, vc); });
}
template<typename T, size_t S>
constexpr auto sin(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sin(val); });
}
template<typename T, size_t S>
constexpr auto cos(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cos(val); });
}
template<typename T, size_t S>
constexpr auto tan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::tan(val); });
}
template<typename T, size_t S>
constexpr auto asin(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::asin(val); });
}
template<typename T, size_t S>
constexpr auto acos(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::acos(val); });
}
template<typename T, size_t S>
constexpr auto atan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::atan(val); });
}
template<typename T, size_t S>
constexpr auto sinh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sinh(val); });
}
template<typename T, size_t S>
constexpr auto cosh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cosh(val); });
}
template<typename T, size_t S>
constexpr auto tanh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::tanh(val); });
}
template<typename T, size_t S>
constexpr auto asinh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::asinh(val); });
}
template<typename T, size_t S>
constexpr auto acosh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::acosh(val); });
}
template<typename T, size_t S>
constexpr auto atanh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::atanh(val); });
}
template<typename T, size_t S>
constexpr auto atan2(const gfx::vec<T, S>& a, const T& b) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& va) { return std::atan2(va, b); });
}
template<typename T, size_t S>
constexpr auto atan2(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::atan2(va, vb); });
}
template<typename T, size_t S>
constexpr auto ceil(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::ceil(val); });
}
template<typename T, size_t S>
constexpr auto floor(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::floor(val); });
}
template<typename T, size_t S>
constexpr auto round(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::round(val); });
}
template<typename T, size_t S>
constexpr auto isfinite(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isfinite(val); });
}
template<typename T, size_t S>
constexpr auto isinf(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isinf(val); });
}
template<typename T, size_t S>
constexpr auto isnan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isnan(val); });
}
template<typename T, size_t S>
constexpr auto isnormal(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isnormal(val); });
}
template<typename T, size_t S>
constexpr auto signbit(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::signbit(val); });
}
template<typename T, size_t S>
constexpr auto isgreater(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isgreater(va, vb); });
}
template<typename T, size_t S>
constexpr auto isgreaterequal(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isgreaterequal(va, vb); });
}
template<typename T, size_t S>
constexpr auto isless(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isless(va, vb); });
}
template<typename T, size_t S>
constexpr auto islessequal(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::islessequal(va, vb); });
}
template<typename T, size_t S>
constexpr auto islessgreater(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::islessgreater(va, vb); });
}
template<typename T, size_t S>
constexpr auto isunordered(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isunordered(va, vb); });
}
template<typename T, size_t S>
constexpr auto max(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::max(va, vb); });
}
template<typename T, size_t S>
constexpr auto min(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::min(va, vb); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const gfx::vec<T, S>& min, const gfx::vec<T, S>& max) noexcept
{
    return gfx::detail::apply_for_each(a, min, max,
                                       [](const auto& va, const auto& vmin, const auto& vmax) { return std::clamp(va, vmin, vmax); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const T& min, const gfx::vec<T, S>& max) noexcept
{
    return gfx::detail::apply_for_each(a, max, [&](const auto& va, const auto& vmax) { return std::clamp(va, min, vmax); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const gfx::vec<T, S>& min, const T& max) noexcept
{
    return gfx::detail::apply_for_each(a, min, [&](const auto& va, const auto& vmin) { return std::clamp(va, vmin, max); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const T& min, const T& max) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& va) { return std::clamp(va, min, max); });
}
}    // namespace std

namespace gfx {
inline namespace v1 {
using std::abs;
using std::acos;
using std::acosh;
using std::arg;
using std::asin;
using std::asinh;
using std::atan;
using std::atanh;
using std::ceil;
using std::clamp;
using std::conj;
using std::cos;
using std::cosh;
using std::cqrt;
using std::exp;
using std::exp2;
using std::expm1;
using std::floor;
using std::hypot;
using std::imag;
using std::isfinite;
using std::isgreater;
using std::isgreaterequal;
using std::isinf;
using std::isless;
using std::islessequal;
using std::islessgreater;
using std::isnan;
using std::isnormal;
using std::isunordered;
using std::log;
using std::log10;
using std::log1p;
using std::log2;
using std::max;
using std::min;
using std::norm;
using std::pow;
using std::proj;
using std::real;
using std::round;
using std::signbit;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;
}    // namespace v1
}    // namespace gfx
#include "vec.inl"