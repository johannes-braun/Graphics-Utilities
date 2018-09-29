#pragma once

#include "vec.hpp"

namespace gfx {
inline namespace v1 {
template<typename T, size_t SizeMajor, size_t SizeMinor>
class mat : public vec<vec<T, SizeMinor>, SizeMajor>
{
public:
	using nested_vector_type = vec<vec<T, SizeMinor>, SizeMajor>;

	using nested_vector_type::vec;
    mat(const nested_vector_type& val) : nested_vector_type(val) {}

};

using mat4 = mat<float, 4, 4>;

template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Maj, Min> diag(T value)
{
    return gfx::detail::apply_for_each<T, Maj>([i = Min - 1, value]() mutable {
        vec<T, Min> rv(0);
        rv[i--] = value;
        return rv;
    });
}
template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Maj, Min> identity()
{
    return diag<T, Maj, Min>(1);
}

template<typename T, size_t SizeMajorA, size_t SizeMinorA, size_t SizeMajorB>
constexpr mat<T, SizeMajorB, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const mat<T, SizeMajorB, SizeMajorA>& b)
{
    mat<T, SizeMajorB, SizeMinorA> result;
    for (size_t col = 0; col < SizeMajorB; ++col)
    {
        for (size_t row = 0; row < SizeMinorA; ++row)
        {
            for (size_t i = 0; i < SizeMajorA; ++i) { result[col][row] += a[i][row] * b[col][i]; }
        }
    }
    return result;
}
template<typename T, size_t SizeMajorA, size_t SizeMinorA>
constexpr vec<T, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const vec<T, SizeMajorA>& b)
{
    vec<T, SizeMinorA> result;
    for (size_t row = 0; row < SizeMinorA; ++row)
        for (size_t i = 0; i < SizeMajorA; ++i) { result[row] += a[i][row] * b[i]; }
    return result;
}

template<typename T, size_t S>
constexpr void transpose(mat<T, S, S>& m)
{
	for (size_t col = 0; col < S; ++col)
		for (size_t row = col + 1; row < S; ++row)
			std::swap(m[col][row], m[row][col]);
}

template<typename T, size_t S>
constexpr mat<T, S, S> transposed(mat<T, S, S> m)
{
	transpose(m);
	return m;
}

template<size_t Maj, size_t Min>
constexpr bool any_of(const mat<bool, Maj, Min>& v)
{
    return std::any_of(v.begin(), v.end(), [](const auto& b) { return any_of(b); });
}
template<size_t Maj, size_t Min>
constexpr bool all_of(const mat<bool, Maj, Min>& v)
{
    return std::all_of(v.begin(), v.end(), [](const auto& b) { return all_of(b); });
}
template<size_t Maj, size_t Min>
constexpr bool none_of(const mat<bool, Maj, Min>& v)
{
    return std::none_of(v.begin(), v.end(), [](const auto& b) { return none_of(b); });
}
template<typename T, size_t Maj, size_t Min>
constexpr auto min_element(const mat<T, Maj, Min>& m)
{
	auto min = m.begin()->begin();
	for (auto it = m.begin(); it != m.end(); ++it)
		for (auto r = it->begin(); r != it->end(); ++r)
			if (*min > *r)
				min = r;
	return min;
}
template<typename T, size_t Maj, size_t Min>
constexpr auto max_element(const mat<T, Maj, Min>& m)
{
	auto max = m.begin()->begin();
	for (auto it = m.begin(); it != m.end(); ++it)
		for (auto r = it->begin(); r != it->end(); ++r)
			if (*max < *r)
				max = r;
	return max;
}
template<typename T, size_t Maj, size_t Min>
constexpr auto minmax_element(const mat<T, Maj, Min>& m)
{
	auto minmax = std::make_pair(m.begin()->begin(), m.begin()->begin());
	for (auto it = m.begin(); it != m.end(); ++it)
		for (auto r = it->begin(); r != it->end(); ++r)
		{
			if (*minmax.first > *r)
				minmax.first = r;
			if (*minmax.second < *r)
				minmax.second = r;
		}
            
	return minmax;
}
}    // namespace v1
}    // namespace gfx