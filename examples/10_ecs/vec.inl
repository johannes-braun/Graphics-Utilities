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
constexpr vec<T, S>::vec() noexcept : vec(T{})
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
    return detail::vec_components<T, S>::components[index];
}

template<typename T, size_t S>
constexpr typename vec<T, S>::const_reference vec<T, S>::at(size_type index) const
{
    return detail::vec_components<T, S>::components[index];
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
template<typename X, size_t D, std::size_t... Is>
constexpr vec<T, S>::vec(std::index_sequence<Is...>, const glm_interop_t&, const glm::vec<D, X>& other) noexcept
      : detail::vec_components<T, S>{static_cast<T>(other[Is])...}
{}

template<typename T, size_t S>
constexpr vec<T, S>::vec(const glm::vec<S, T>& v) noexcept : vec(std::make_index_sequence<S>{}, glm_interop, v)
{}
/*
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
		detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		val op = other;
	});
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
		detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		val op = other;
	});
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
		detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		val op = other;
	});
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
		detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		val op = other;
	});
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
			detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
		{
			val op = other;
		});
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
			detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
		{
			val op = other;
		});
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
		return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		return val op other;
	});
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
		return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		return val op other;
	});
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
		return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		return val op other;
	});
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
		return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
	{
		return val op other;
	});
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
		    return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
		    {
			    return std::fmod(val, other);
		    });
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
			return detail::apply_for_each(*this, other, [](const auto& val, const auto& other)
		    {
			    return val op other;
		    });
#undef op
    }
}



template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator+=(const T& other) noexcept
{
#define op +
	if constexpr (S == 1)
		this->x op = other;
	else if constexpr (S == 2)
		this->x op = other, this->y op = other;
	else if constexpr (S == 3)
		this->x op = other, this->y op = other, this->z op = other;
	else if constexpr (S == 4)
		this->x op = other, this->y op = other, this->z op = other, this->w op = other;
	else
		detail::apply_for_each(*this, [&](auto& val)
	{
		val op = other;
	});
#undef op
	return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator-=(const T& other) noexcept
{
#define op -
	if constexpr (S == 1)
		this->x op = other;
	else if constexpr (S == 2)
		this->x op = other, this->y op = other;
	else if constexpr (S == 3)
		this->x op = other, this->y op = other, this->z op = other;
	else if constexpr (S == 4)
		this->x op = other, this->y op = other, this->z op = other, this->w op = other;
	else
		detail::apply_for_each(*this, [&](auto& val)
	{
		val op = other;
	});
#undef op
	return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator*=(const T& other) noexcept
{
#define op *
	if constexpr (S == 1)
		this->x op = other;
	else if constexpr (S == 2)
		this->x op = other, this->y op = other;
	else if constexpr (S == 3)
		this->x op = other, this->y op = other, this->z op = other;
	else if constexpr (S == 4)
		this->x op = other, this->y op = other, this->z op = other, this->w op = other;
	else
		detail::apply_for_each(*this, [&](auto& val)
	{
		val op = other;
	});
#undef op
	return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator/=(const T& other) noexcept
{
#define op /
	if constexpr (S == 1)
		this->x op = other;
	else if constexpr (S == 2)
		this->x op = other, this->y op = other;
	else if constexpr (S == 3)
		this->x op = other, this->y op = other, this->z op = other;
	else if constexpr (S == 4)
		this->x op = other, this->y op = other, this->z op = other, this->w op = other;
	else
		detail::apply_for_each(*this, [&](auto& val)
	{
		val op = other;
	});
#undef op
	return *this;
}

template<typename T, size_t S>
constexpr vec<T, S>& vec<T, S>::operator%=(const T& other) const noexcept
{
	if constexpr (std::is_floating_point_v<std::decay_t<T>>)
	{
		if constexpr (S == 1)
			this->x = std::fmod(this->x, other);
		else if constexpr (S == 2)
			this->x = std::fmod(this->x, other), this->y = std::fmod(this->y, other);
		else if constexpr (S == 3)
			this->x = std::fmod(this->x, other), this->y = std::fmod(this->y, other), this->z = std::fmod(this->z, other);
		else if constexpr (S == 4)
			this->x = std::fmod(this->x, other), this->y = std::fmod(this->y, other), this->z = std::fmod(this->z, other),
			this->w = std::fmod(this->w, other);
		else
			detail::apply_for_each(*this, [&](auto& val)
		{
			val = std::fmod(val, other);
		});
	}
	else
	{
#define op %
		if constexpr (S == 1)
			this->x op = other;
		else if constexpr (S == 2)
			this->x op = other, this->y op = other;
		else if constexpr (S == 3)
			this->x op = other, this->y op = other, this->z op = other;
		else if constexpr (S == 4)
			this->x op = other, this->y op = other, this->z op = other, this->w op = other;
		else
			detail::apply_for_each(*this, [&](auto& val)
		{
			val op = other;
		});
#undef op
	}
	return *this;
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator+(const T& other) const noexcept
{
#define op +
	if constexpr (S == 1)
		return vec(this->x op other);
	else if constexpr (S == 2)
		return vec(this->x op other, this->y op other);
	else if constexpr (S == 3)
		return vec(this->x op other, this->y op other, this->z op other);
	else if constexpr (S == 4)
		return vec(this->x op other, this->y op other, this->z op other, this->w op other);
	else
		return detail::apply_for_each(*this, [&](const auto& val)
	{
		return val op other;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator-(const T& other) const noexcept
{
#define op -
	if constexpr (S == 1)
		return vec(this->x op other);
	else if constexpr (S == 2)
		return vec(this->x op other, this->y op other);
	else if constexpr (S == 3)
		return vec(this->x op other, this->y op other, this->z op other);
	else if constexpr (S == 4)
		return vec(this->x op other, this->y op other, this->z op other, this->w op other);
	else
		return detail::apply_for_each(*this, [&](const auto& val)
	{
		return val op other;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator*(const T& other) const noexcept
{
#define op *
	if constexpr (S == 1)
		return vec(this->x op other);
	else if constexpr (S == 2)
		return vec(this->x op other, this->y op other);
	else if constexpr (S == 3)
		return vec(this->x op other, this->y op other, this->z op other);
	else if constexpr (S == 4)
		return vec(this->x op other, this->y op other, this->z op other, this->w op other);
	else
		return detail::apply_for_each(*this, [&](const auto& val)
	{
		return val op other;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator/(const T& other) const noexcept
{
#define op /
	if constexpr (S == 1)
		return vec(this->x op other);
	else if constexpr (S == 2)
		return vec(this->x op other, this->y op other);
	else if constexpr (S == 3)
		return vec(this->x op other, this->y op other, this->z op other);
	else if constexpr (S == 4)
		return vec(this->x op other, this->y op other, this->z op other, this->w op other);
	else
		return detail::apply_for_each(*this, [&](const auto& val)
	{
		return val op other;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> vec<T, S>::operator%(const T& other) const noexcept
{
	if constexpr (std::is_floating_point_v<std::decay_t<T>>)
	{
		if constexpr (S == 1)
			return vec(std::fmod(this->x, other));
		else if constexpr (S == 2)
			return vec(std::fmod(this->x, other), std::fmod(this->y, other));
		else if constexpr (S == 3)
			return vec(std::fmod(this->x, other), std::fmod(this->y, other), std::fmod(this->z, other));
		else if constexpr (S == 4)
			return vec(std::fmod(this->x, other), std::fmod(this->y, other), std::fmod(this->z, other), std::fmod(this->w, other));
		else
			return detail::apply_for_each(*this, [&](const auto& val)
		{
			return std::fmod(val, other);
		});
	}
	else
	{
#define op %
		if constexpr (S == 1)
			return vec(this->x op other);
		else if constexpr (S == 2)
			return vec(this->x op other, this->y op other);
		else if constexpr (S == 3)
			return vec(this->x op other, this->y op other, this->z op other);
		else if constexpr (S == 4)
			return vec(this->x op other, this->y op other, this->z op other, this->w op other);
		else
			return detail::apply_for_each(*this, [&](const auto& val)
		    {
			    return val op other;
		    });
#undef op
	}
}



template<typename T, size_t S>
constexpr vec<T, S> operator+(const T& other, const vec<T, S>& v) noexcept
{
#define op +
	if constexpr (S == 1)
		return vec(other op this->x);
	else if constexpr (S == 2)
		return vec(other op v.x, other op v.y);
	else if constexpr (S == 3)
		return vec(other op v.x, other op v.y, other op v.z);
	else if constexpr (S == 4)
		return vec(other op v.x, other op v.y, other op v.z, other op v.w);
	else
		return detail::apply_for_each(v, [&](const auto& val)
	{
		return val op other;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> operator-(const T& other, const vec<T, S>& v) noexcept
{
#define op -
	if constexpr (S == 1)
		return vec(other op v.x);
	else if constexpr (S == 2)
		return vec(other op v.x, other op v.y);
	else if constexpr (S == 3)
		return vec(other op v.x, other op v.y, other op v.z);
	else if constexpr (S == 4)
		return vec(other op v.x, other op v.y, other op v.z, other op v.w);
	else
		return detail::apply_for_each(v, [&](const auto& val)
	{
		return other op val;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> operator*(const T& other, const vec<T, S>& v) noexcept
{
#define op *
	if constexpr (S == 1)
		return vec<T, S>(other op v.x);
	else if constexpr (S == 2)
		return vec<T, S>(other op v.x, other op v.y);
	else if constexpr (S == 3)
		return vec<T, S>(other op v.x, other op v.y, other op v.z);
	else if constexpr (S == 4)
		return vec<T, S>(other op v.x, other op v.y, other op v.z, other op v.w);
	else
		return detail::apply_for_each(v, [&](const auto& val)
	{
		return other op val;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> operator/(const T& other, const vec<T, S>& v) noexcept
{
#define op /
	if constexpr (S == 1)
		return vec<T, S>(other op v.x);
	else if constexpr (S == 2)
		return vec<T, S>(other op v.x, other op v.y);
	else if constexpr (S == 3)
		return vec<T, S>(other op v.x, other op v.y, other op v.z);
	else if constexpr (S == 4)
		return vec<T, S>(other op v.x, other op v.y, other op v.z, other op v.w);
	else
		return detail::apply_for_each(v, [&](const auto& val)
	{
		return other op val;
	});
#undef op
}

template<typename T, size_t S>
constexpr vec<T, S> operator%(const T& other, const vec<T, S>& v) noexcept
{
	if constexpr (std::is_floating_point_v<std::decay_t<T>>)
	{
		if constexpr (S == 1)
			return vec<T, S>(std::fmod(v.x, other));
		else if constexpr (S == 2)
			return vec<T, S>(std::fmod(v.x, other), std::fmod(v.y, other));
		else if constexpr (S == 3)
			return vec<T, S>(std::fmod(v.x, other), std::fmod(v.y, other), std::fmod(v.z, other));
		else if constexpr (S == 4)
			return vec<T, S>(std::fmod(v.x, other), std::fmod(v.y, other), std::fmod(v.z, other), std::fmod(v.w, other));
		else
			return detail::apply_for_each(v, [&](const auto& val)
		{
			return std::fmod(other, val);
		});
	}
	else
	{
#define op %
		if constexpr (S == 1)
			return vec<T, S>(other op v.x);
		else if constexpr (S == 2)
			return vec<T, S>(other op v.x, other op v.y);
		else if constexpr (S == 3)
			return vec<T, S>(other op v.x, other op v.y, other op v.z);
		else if constexpr (S == 4)
			return vec<T, S>(other op v.x, other op v.y, other op v.z, other op v.w);
		else
			return detail::apply_for_each(v, [&](const auto& val)
		{
			return other op val;
		});
#undef op
	}
}
*/
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
constexpr void vec<T, S>::fill(const T& value)
{
    std::fill_n(this->components, S, value);
}
}    // namespace v1
}    // namespace gfx