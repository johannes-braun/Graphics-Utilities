#pragma once

#include "vec.hpp"

#include <iostream>

namespace gfx {
inline namespace v1 {
template<typename T, size_t SizeMajor, size_t SizeMinor>
class mat : public vec<vec<T, SizeMinor>, SizeMajor>
{
public:
    using nested_vector_type = vec<vec<T, SizeMinor>, SizeMajor>;

    using nested_vector_type::vec;
    constexpr mat(const nested_vector_type& val) : nested_vector_type(val) {}

    constexpr size_t rows() const noexcept { return SizeMinor; }
    constexpr size_t cols() const noexcept { return SizeMajor; }

    template<typename X, size_t Maj, size_t Min>
    constexpr mat<X, Maj, Min> sub_mat(size_t offset_maj, size_t offset_min)
    {
        mat<X, Maj, Min> m;
        for (auto maj = offset_maj; maj < offset_maj + Maj; ++maj)
            for (auto min = offset_min; min < offset_min + Min; ++min) m[maj - offset_maj][min - offset_min] = (*this)[maj][min];
        return m;
    }
};

using mat4 = mat<float, 4, 4>;

template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Maj, Min> diag(T value)
{
    return detail::apply_for_each<T, Maj>([i = Min - 1, value](size_t) mutable {
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

namespace detail {
template<typename UnaryConvertFun, size_t... Is, typename T, size_t Min>
constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, size_t maj, const vec<T, Min>& vector,
                                           UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t(), vector.components[0]));
    if constexpr (std::is_same_v<return_type, void>)
        (fun(maj, Is, vector.components[Is]), ...);
    else
        return vec<return_type, Min>(fun(maj, Is, vector.components[Is])...);
}
template<typename UnaryConvertFun, size_t... Is, typename T, size_t Maj, size_t Min>
constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, const mat<T, Maj, Min>& matrix, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t(), matrix.components[0][0]));
    if constexpr (std::is_same_v<return_type, void>)
        (apply_for_each_indexed_impl(std::make_index_sequence<Min>{}, Is, matrix.components[Is], std::forward<UnaryConvertFun&&>(fun)),
         ...);
    else
        return mat<return_type, Maj, Min>(apply_for_each_indexed_impl(std::make_index_sequence<Min>{}, Is, matrix.components[Is],
                                                                      std::forward<UnaryConvertFun&&>(fun))...);
}
template<typename UnaryConvertFun, typename T, size_t Maj, size_t Min>
constexpr auto apply_for_each_indexed(const mat<T, Maj, Min>& matrix, UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_indexed_impl(std::make_index_sequence<Maj>{}, matrix, std::forward<UnaryConvertFun&&>(fun));
}

template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t()));
    if constexpr (std::is_same_v<return_type, void>)
        (fun(maj, Is), ...);
    else
        return vec<return_type, Min>(fun(maj, Is)...);
}
template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t()));
    if constexpr (std::is_same_v<return_type, void>)
        (apply_for_each_indexed_impl<T, Min>(std::make_index_sequence<Min>{}, Is, std::forward<UnaryConvertFun&&>(fun)), ...);
    else
        return mat<return_type, Maj, Min>(
            apply_for_each_indexed_impl<T, Min>(std::make_index_sequence<Min>{}, Is, std::forward<UnaryConvertFun&&>(fun))...);
}
template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
constexpr auto apply_for_each_indexed(UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_indexed_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
}


template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
constexpr auto apply_for_each_indexed_half_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t()));
    if constexpr (std::is_same_v<return_type, void>)
        (fun(maj, maj + Is + 1), ...);
    else
        return vec<return_type, Min>(fun(maj, maj + Is + 1)...);
}
template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
constexpr auto apply_for_each_indexed_half_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
{
    using return_type = decltype(fun(size_t(), size_t()));
    if constexpr (std::is_same_v<return_type, void>)
        (apply_for_each_indexed_half_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Is, std::forward<UnaryConvertFun&&>(fun)), ...);
    else
        return mat<return_type, Maj, Min>(apply_for_each_indexed_half_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Is,
                                                                                   std::forward<UnaryConvertFun&&>(fun))...);
}
template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
constexpr auto apply_for_each_half_indexed(UnaryConvertFun&& fun) noexcept
{
    return apply_for_each_indexed_half_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
}
}    // namespace detail

template<typename T, size_t S>
constexpr T det(const mat<T, S, S>& m);

template<size_t Col, typename T, size_t S, size_t... Is>
constexpr vec<T, S - 1> construct_det_submat_vector(const mat<T, S, S>& m, std::index_sequence<Is...>)
{
    return vec<T, S - 1>(m[Col][Is + 1]...);
}

template<typename T, size_t S, size_t Col, size_t... Is>
constexpr mat<T, S - 1, S - 1> construct_det_submat(const mat<T, S, S>& m, std::index_sequence<Is...>)
{
    return mat<T, S - 1, S - 1>(construct_det_submat_vector < Is >= Col ? Is + 1 : Is > (m, std::make_index_sequence<S - 1>{})...);
}

template<typename T, size_t S, size_t... Columns>
constexpr T compute_det(const mat<T, S, S>& m, std::index_sequence<Columns...>)
{
    return ((m[Columns][0] * -(int(Columns & 0x1) * 2 - 1) * det(construct_det_submat<T, S, Columns>(m, std::make_index_sequence<S - 1>())))
            + ...);
}

template<typename T, size_t S>
constexpr T det(const mat<T, S, S>& m)
{
    if constexpr (S == 0)
        return T{1};
    else if constexpr (S == 1)
        return m[0][0];
    else if constexpr (S == 2)
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    else if constexpr (S == 3)
    {
        const auto m_22_33 = m[1][1] * m[2][2];
        const auto m_21_32 = m[1][0] * m[2][1];
        const auto m_31_23 = m[2][0] * m[1][2];
        const auto m_31_22 = m[2][0] * m[1][1];
        const auto m_21_33 = m[1][0] * m[2][2];
        const auto m_32_23 = m[2][1] * m[1][2];

        return m[0][0] * (m_22_33 - m_32_23) + m[0][2] * (m_21_32 - m_31_22) + m[0][1] * (m_31_23 - m_21_33);
    }
    else if constexpr (S == 4)
    {
		const auto m_33_44_34_43 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		const auto m_32_44_42_34 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		const auto m_32_43_42_33 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		const auto m_31_44_41_43 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		const auto m_31_43_41_33 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		const auto m_31_42_41_32 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        return m[0][0] * (m[1][1] * m_33_44_34_43 - m[1][2] * m_32_44_42_34 + m[1][3] * m_32_43_42_33)
               - m[0][1] * (m[1][0] * m_33_44_34_43 - m[1][2] * m_31_44_41_43 + m[1][3] * m_31_43_41_33)
               + m[0][2] * (m[1][0] * m_32_44_42_34 - m[1][1] * m_31_44_41_43 + m[1][3] * m_31_42_41_32)
               - m[0][3] * (m[1][0] * m_32_43_42_33 - m[1][1] * m_31_43_41_33 + m[1][2] * m_31_42_41_32);
    }
    else
    {
        return compute_det(m, std::make_index_sequence<S>{});
    }
}

template<typename T, size_t SizeMajorA, size_t SizeMinorA, size_t SizeMajorB>
constexpr mat<T, SizeMajorB, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const mat<T, SizeMajorB, SizeMajorA>& b)
{
    mat<T, SizeMajorB, SizeMinorA> result = detail::apply_for_each_indexed<T, SizeMajorB, SizeMinorA>([&](size_t col, size_t row) {
        T val = T();
        for (size_t i = 0; i < SizeMajorA; ++i) { val += a[i][row] * b[col][i]; }
        return val;
    });
    return result;
}
template<typename T, size_t SizeMajorA, size_t SizeMinorA>
constexpr vec<T, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const vec<T, SizeMajorA>& b)
{
    const auto result = detail::apply_for_each<T, SizeMinorA>([&](size_t min) {
        T val = T();
        for (size_t i = 0; i < SizeMajorA; ++i) { val += a[i][min] * b[i]; }
        return val;
    });
    return reinterpret_cast<const vec<T, SizeMinorA>&>(result);
}

template<typename T, size_t A, size_t B>
constexpr auto outer_prod(const vec<T, A>& a, const vec<T, B>& b)
{
	const auto& a_mat = reinterpret_cast<const mat<T, 1, A>&>(a);
	const auto& b_mat = reinterpret_cast<const mat<T, B, 1>&>(b);

	return a_mat * b_mat;
}

template<typename T, size_t S>
constexpr void transpose(mat<T, S, S>& m)
{
    detail::apply_for_each_half_indexed<T, S, S>([&](size_t col, size_t row) { std::swap(m[col][row], m[row][col]); });
}

template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Min, Maj> transposed(const mat<T, Maj, Min>& in)
{
    const auto x = detail::apply_for_each_indexed<T, Min, Maj>([&](size_t col, size_t row) { return in[row][col]; });
    return x;
}
template<typename T, size_t S>
constexpr mat<T, S, S> transposed(const mat<T, S, S>& in)
{
    const auto x = detail::apply_for_each_indexed<T, S, S>([&](size_t col, size_t row) { return in[row][col]; });
    return x;
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
            if (*min > *r) min = r;
    return min;
}
template<typename T, size_t Maj, size_t Min>
constexpr auto max_element(const mat<T, Maj, Min>& m)
{
    auto max = m.begin()->begin();
    for (auto it = m.begin(); it != m.end(); ++it)
        for (auto r = it->begin(); r != it->end(); ++r)
            if (*max < *r) max = r;
    return max;
}
template<typename T, size_t Maj, size_t Min>
constexpr auto minmax_element(const mat<T, Maj, Min>& m)
{
    auto minmax = std::make_pair(m.begin()->begin(), m.begin()->begin());
    for (auto it = m.begin(); it != m.end(); ++it)
        for (auto r = it->begin(); r != it->end(); ++r)
        {
            if (*minmax.first > *r) minmax.first = r;
            if (*minmax.second < *r) minmax.second = r;
        }

    return minmax;
}
}    // namespace v1
}    // namespace gfx