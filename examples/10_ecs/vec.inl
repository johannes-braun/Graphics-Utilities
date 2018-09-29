#pragma once

namespace gfx {
inline namespace v1 {
template<typename T, size_t S>
template<std::size_t... Is>
constexpr vec<T, S>::vec(std::index_sequence<Is...>, T&& value) noexcept : detail::vec_components<T, S>{(static_cast<void>(Is), value)...}
{}

template<typename T, size_t S>
template<typename X, size_t D, std::size_t... Is>
constexpr vec<T, S>::vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept
      : detail::vec_components<T, S>{static_cast<T>(other[Is])}
{}

template<typename T, size_t S>
template<typename X, std::size_t... Is>
constexpr vec<T, S>::vec(std::index_sequence<Is...>, const X* other) noexcept : detail::vec_components<T, S>{static_cast<T>(other[Is])...}
{}

template<typename T, size_t S>
constexpr vec<T, S>::vec() noexcept : vec(0)
{}

template<typename T, size_t S>
template<typename X, size_t D, typename>
constexpr vec<T, S>::vec(const vec<X, D>& other) noexcept : vec(std::make_index_sequence<std::min(S, D)>(), other)
{}

template<typename T, size_t S>
template<typename X, typename>
constexpr vec<T, S>::vec(const X* ptr) : vec(std::make_index_sequence<S>(), ptr)
{}

template<typename T, size_t S>
template<typename X, typename>
constexpr vec<T, S>::vec(X* ptr) : vec(std::make_index_sequence<S>(), ptr)
{}

template<typename T, size_t S>
constexpr vec<T, S>::vec(T&& value) noexcept : vec(std::make_index_sequence<S>(), std::forward<T&&>(value))
{}

template<typename T, size_t S>
template<typename... Ts, typename>
constexpr vec<T, S>::vec(Ts&&... ts) noexcept : detail::vec_components<T, S>{static_cast<value_type>(ts)...}
{}

template<typename T, size_t S>
constexpr typename vec<T, S>::reference vec<T, S>::at(size_type index)
{
    return detail::vec_components<T, S>::components.at(index);
}

template<typename T, size_t S>
constexpr typename vec<T, S>::const_reference vec<T, S>::at(size_type index) const
{
    return detail::vec_components<T, S>::components.at(index);
}

template<typename T, size_t S>
constexpr typename vec<T, S>::reference vec<T, S>::operator[](size_type index)
{
    return detail::vec_components<T, S>::components[index];
}

template<typename T, size_t S>
constexpr typename vec<T, S>::const_reference vec<T, S>::operator[](size_type index) const
{
    return detail::vec_components<T, S>::components[index];
}

template<typename T, size_t S>
constexpr vec<T, S>::operator const glm::vec<S, T>&() const noexcept
{
    return reinterpret_cast<const glm::vec<S, T>&>(*this);
}

template<typename T, size_t S>
constexpr vec<T, S>::operator glm::vec<S, T>&() noexcept
{
    return reinterpret_cast<glm::vec<S, T>&>(*this);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_add_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec((s1[Is] + s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_sub_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec((s1[Is] - s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_mul_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec((s1[Is] * s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_div_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec((s1[Is] / s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_mod_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec((s1[Is] % s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_fmod_unroll(std::index_sequence<Is...>, const_pointer s1, const_pointer s2) noexcept
{
    return vec(std::fmod(s1[Is], s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_add_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] += s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_sub_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] -= s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_mul_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] *= s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_div_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] /= s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_mod_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] %= s2[Is])...);
}

template<typename T, size_t S>
template<std::size_t... Is>
constexpr auto vec<T, S>::op_fmod_unroll_assign(std::index_sequence<Is...>, pointer d, const_pointer s2) noexcept
{
    return ((d[Is] = std::fmod(d[Is], s2[Is]))...);
}

template<typename T, size_t S>
template<typename X, size_t D, std::size_t... Is>
constexpr vec<T, S>::vec(std::index_sequence<Is...>, const glm_interop_t&, const glm::vec<D, X>& other) noexcept
      : detail::vec_components<T, S>{static_cast<T>(other[Is])...}
{}

template<typename T, size_t S>
constexpr vec<T, S>::vec(const glm::vec<S, T>& v) noexcept : vec(std::make_index_sequence<S>{}, glm_interop, v)
{}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator+=(const vec& other) noexcept
{
#define op +
    if constexpr (S == 1)
        this->x op = other.x;
    else if constexpr (S == 2)
        this->x op = other.x, this->y op = other.y;
    else if constexpr (S == 3)
        this->x op = other.x, this->y op = other.y, this->z op = other.z;
    else if constexpr (S == 4)
        this->x op = other.x, this->y op = other.y, this->z op = other.z, this->w op = other.w;
    else
        op_add_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator-=(const vec& other) noexcept
{
#define op -
    if constexpr (S == 1)
        this->x op = other.x;
    else if constexpr (S == 2)
        this->x op = other.x, this->y op = other.y;
    else if constexpr (S == 3)
        this->x op = other.x, this->y op = other.y, this->z op = other.z;
    else if constexpr (S == 4)
        this->x op = other.x, this->y op = other.y, this->z op = other.z, this->w op = other.w;
    else
        op_sub_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator*=(const vec& other) noexcept
{
#define op *
    if constexpr (S == 1)
        this->x op = other.x;
    else if constexpr (S == 2)
        this->x op = other.x, this->y op = other.y;
    else if constexpr (S == 3)
        this->x op = other.x, this->y op = other.y, this->z op = other.z;
    else if constexpr (S == 4)
        this->x op = other.x, this->y op = other.y, this->z op = other.z, this->w op = other.w;
    else
        op_mul_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator/=(const vec& other) noexcept
{
#define op /
    if constexpr (S == 1)
        this->x op = other.x;
    else if constexpr (S == 2)
        this->x op = other.x, this->y op = other.y;
    else if constexpr (S == 3)
        this->x op = other.x, this->y op = other.y, this->z op = other.z;
    else if constexpr (S == 4)
        this->x op = other.x, this->y op = other.y, this->z op = other.z, this->w op = other.w;
    else
        op_div_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator%=(const vec& other) const noexcept
{
    if constexpr (std::is_floating_point_v<std::decay_t<T>>)
    {
        if constexpr (S == 1)
            this->x = std::fmod(this->x, other.x);
        else if constexpr (S == 2)
            this->x = std::fmod(this->x, other.x), this->y = std::fmod(this->y, other.y);
        else if constexpr (S == 3)
            this->x = std::fmod(this->x, other.x), this->y = std::fmod(this->y, other.y), this->z = std::fmod(this->z, other.z);
        else if constexpr (S == 4)
            this->x = std::fmod(this->x, other.x), this->y = std::fmod(this->y, other.y), this->z = std::fmod(this->z, other.z),
            this->w = std::fmod(this->w, other.w);
        else
            op_fmod_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
    }
    else
    {
#define op %
        if constexpr (S == 1)
            this->x op = other.x;
        else if constexpr (S == 2)
            this->x op = other.x, this->y op = other.y;
        else if constexpr (S == 3)
            this->x op = other.x, this->y op = other.y, this->z op = other.z;
        else if constexpr (S == 4)
            this->x op = other.x, this->y op = other.y, this->z op = other.z, this->w op = other.w;
        else
            op_mod_unroll_assign(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    }
    return *this;
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator+(const vec& other) const noexcept
{
#define op +
    if constexpr (S == 1)
        return vec(this->x op other.x);
    else if constexpr (S == 2)
        return vec(this->x op other.x, this->y op other.y);
    else if constexpr (S == 3)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z);
    else if constexpr (S == 4)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z, this->w op other.w);
    else
        return op_add_unroll(std::make_index_sequence<S>(), this->components, other.components);
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator-(const vec& other) const noexcept
{
#define op -
    if constexpr (S == 1)
        return vec(this->x op other.x);
    else if constexpr (S == 2)
        return vec(this->x op other.x, this->y op other.y);
    else if constexpr (S == 3)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z);
    else if constexpr (S == 4)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z, this->w op other.w);
    else
        return op_sub_unroll(std::make_index_sequence<S>(), this->components, other.components);
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator*(const vec& other) const noexcept
{
#define op *
    if constexpr (S == 1)
        return vec(this->x op other.x);
    else if constexpr (S == 2)
        return vec(this->x op other.x, this->y op other.y);
    else if constexpr (S == 3)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z);
    else if constexpr (S == 4)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z, this->w op other.w);
    else
        return op_mul_unroll(std::make_index_sequence<S>(), this->components, other.components);
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator/(const vec& other) const noexcept
{
#define op /
    if constexpr (S == 1)
        return vec(this->x op other.x);
    else if constexpr (S == 2)
        return vec(this->x op other.x, this->y op other.y);
    else if constexpr (S == 3)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z);
    else if constexpr (S == 4)
        return vec(this->x op other.x, this->y op other.y, this->z op other.z, this->w op other.w);
    else
        return op_div_unroll(std::make_index_sequence<S>(), this->components, other.components);
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator%(const vec& other) const noexcept
{
    if constexpr (std::is_floating_point_v<std::decay_t<T>>)
    {
        if constexpr (S == 1)
            return vec(std::fmod(this->x, other.x));
        else if constexpr (S == 2)
            return vec(std::fmod(this->x, other.x), std::fmod(this->y, other.y));
        else if constexpr (S == 3)
            return vec(std::fmod(this->x, other.x), std::fmod(this->y, other.y), std::fmod(this->z, other.z));
        else if constexpr (S == 4)
            return vec(std::fmod(this->x, other.x), std::fmod(this->y, other.y), std::fmod(this->z, other.z), std::fmod(this->w, other.w));
        else
            return op_fmod_unroll(std::make_index_sequence<S>(), this->components, other.components);
    }
    else
    {
#define op %
        if constexpr (S == 1)
            return vec(this->x op other.x);
        else if constexpr (S == 2)
            return vec(this->x op other.x, this->y op other.y);
        else if constexpr (S == 3)
            return vec(this->x op other.x, this->y op other.y, this->z op other.z);
        else if constexpr (S == 4)
            return vec(this->x op other.x, this->y op other.y, this->z op other.z, this->w op other.w);
        else
            return op_mod_unroll(std::make_index_sequence<S>(), this->components, other.components);
#undef op
    }
}

template<typename T, size_t S>
constexpr typename vec<T, S>::pointer vec<T, S>::data() noexcept
{
    return this->components;
}

template<typename T, size_t S>
constexpr typename vec<T, S>::const_pointer vec<T, S>::data() const noexcept
{
    return this->components;
}

template<typename T, size_t S>
constexpr typename vec<T, S>::size_type vec<T, S>::size() const noexcept
{
    return S;
}

template<typename T, size_t S>
void vec<T, S>::fill(const T& value)
{
    std::fill_n(this->components, S, value);
}
}    // namespace v1
}    // namespace gfx