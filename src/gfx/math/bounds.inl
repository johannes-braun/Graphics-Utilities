#pragma once
#include <cassert>

namespace gfx
{
    namespace
    {
        template<typename T, glm::qualifier Q> constexpr glm::vec<1, T, Q> vec_min(const glm::vec<1, T, Q>& one, const glm::vec<1, T, Q>& other)
        {
            return std::min(one.x, other.x);
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<2, T, Q> vec_min(const glm::vec<2, T, Q>& one, const glm::vec<2, T, Q>& other)
        {
            return { std::min(one.x, other.x), std::min(one.y, other.y) };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<3, T, Q> vec_min(const glm::vec<3, T, Q>& one, const glm::vec<3, T, Q>& other)
        {
            return { std::min(one.x, other.x), std::min(one.y, other.y), std::min(one.z, other.z) };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<4, T, Q> vec_min(const glm::vec<4, T, Q>& one, const glm::vec<4, T, Q>& other)
        {
            return { std::min(one.x, other.x), std::min(one.y, other.y), std::min(one.z, other.z), std::min(one.w, other.w) };
        }

        template<typename T, glm::qualifier Q> constexpr glm::vec<1, T, Q> vec_max(const glm::vec<1, T, Q>& one, const glm::vec<1, T, Q>& other)
        {
            return std::max(one.x, other.x);
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<2, T, Q> vec_max(const glm::vec<2, T, Q>& one, const glm::vec<2, T, Q>& other)
        {
            return { std::max(one.x, other.x), std::max(one.y, other.y) };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<3, T, Q> vec_max(const glm::vec<3, T, Q>& one, const glm::vec<3, T, Q>& other)
        {
            return { std::max(one.x, other.x), std::max(one.y, other.y), std::max(one.z, other.z) };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<4, T, Q> vec_max(const glm::vec<4, T, Q>& one, const glm::vec<4, T, Q>& other)
        {
            return { std::max(one.x, other.x), std::max(one.y, other.y), std::max(one.z, other.z), std::max(one.w, other.w) };
        }

        template<typename T, glm::qualifier Q> constexpr glm::vec<1, T, Q> vec_div_scalar(const glm::vec<1, T, Q>& one, const float& other)
        {
            return one.x / other;
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<2, T, Q> vec_div_scalar(const glm::vec<2, T, Q>& one, const float& other)
        {
            return { one.x / other, one.y / other };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<3, T, Q> vec_div_scalar(const glm::vec<3, T, Q>& one, const float& other)
        {
            return { one.x / other, one.y / other, one.z / other };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<4, T, Q> vec_div_scalar(const glm::vec<4, T, Q>& one, const float& other)
        {
            return { one.x / other, one.y / other, one.z / other, one.w / other };
        }

        template<typename T, glm::qualifier Q> constexpr glm::vec<1, T, Q> vec_sub(const glm::vec<1, T, Q>& one, const glm::vec<1, T, Q>& other)
        {
            return one.x - other.x;
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<2, T, Q> vec_sub(const glm::vec<2, T, Q>& one, const glm::vec<2, T, Q>& other)
        {
            return { one.x - other.x, one.y - other.y };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<3, T, Q> vec_sub(const glm::vec<3, T, Q>& one, const glm::vec<3, T, Q>& other)
        {
            return { one.x - other.x, one.y - other.y, one.z - other.z };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<4, T, Q> vec_sub(const glm::vec<4, T, Q>& one, const glm::vec<4, T, Q>& other)
        {
            return { one.x - other.x, one.y - other.y, one.z - other.z, one.w - other.w };
        }

        template<typename T, glm::qualifier Q> constexpr glm::vec<1, T, Q> vec_add(const glm::vec<1, T, Q>& one, const glm::vec<1, T, Q>& other)
        {
            return one.x + other.x;
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<2, T, Q> vec_add(const glm::vec<2, T, Q>& one, const glm::vec<2, T, Q>& other)
        {
            return { one.x + other.x, one.y + other.y };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<3, T, Q> vec_add(const glm::vec<3, T, Q>& one, const glm::vec<3, T, Q>& other)
        {
            return { one.x + other.x, one.y + other.y, one.z + other.z };
        }
        template<typename T, glm::qualifier Q> constexpr glm::vec<4, T, Q> vec_add(const glm::vec<4, T, Q>& one, const glm::vec<4, T, Q>& other)
        {
            return { one.x + other.x, one.y + other.y, one.z + other.z, one.w + other.w };
        }

        template<typename T, glm::qualifier Q> constexpr bool vec_eq(const glm::vec<1, T, Q>& one, const glm::vec<1, T, Q>& other)
        {
            return one.x == other.x;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_eq(const glm::vec<2, T, Q>& one, const glm::vec<2, T, Q>& other)
        {
            return one.x == other.x && one.y == other.y;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_eq(const glm::vec<3, T, Q>& one, const glm::vec<3, T, Q>& other)
        {
            return one.x == other.x && one.y == other.y && one.z == other.z;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_eq(const glm::vec<4, T, Q>& one, const glm::vec<4, T, Q>& other)
        {
            return one.x == other.x && one.y == other.y && one.z == other.z && one.w == other.w;
        }

        template<size_t L, typename T, glm::qualifier Q> constexpr glm::vec<L, T, Q> vec_clamp(const glm::vec<L, T, Q>& p, const glm::vec<L, T, Q>& min, const glm::vec<L, T, Q>& max)
        {
            return vec_min<T ,Q>(vec_max<T, Q>(p, min), max);
        }

        template<typename T, glm::qualifier Q> constexpr T vec_surface(const glm::vec<1, T, Q>& min, const glm::vec<1, T, Q>& max)
        {
            return T(0);
        }
        template<typename T, glm::qualifier Q> constexpr T vec_surface(const glm::vec<2, T, Q>& min, const glm::vec<2, T, Q>& max)
        {
            return (max.x - min.x) * (max.y - min.y);
        }
        template<typename T, glm::qualifier Q> constexpr T vec_surface(const glm::vec<3, T, Q>& min, const glm::vec<3, T, Q>& max)
        {
            const auto s = vec_sub(max, min);
            return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z);
        }
        template<typename T, glm::qualifier Q> constexpr T vec_surface(const glm::vec<4, T, Q>& min, const glm::vec<4, T, Q>& max)
        {
            return vec_surface(glm::vec<3, T, Q>(min), glm::vec<3, T, Q>(max));
        }

        
        template<typename T, glm::qualifier Q> constexpr bool vec_any_lteq(const glm::vec<1, T, Q>& a, const glm::vec<1, T, Q>& b)
        {
            return a.x <= b.x;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_any_lteq(const glm::vec<2, T, Q>& a, const glm::vec<2, T, Q>& b)
        {
            return a.x <= b.x || a.y <= b.y;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_any_lteq(const glm::vec<3, T, Q>& a, const glm::vec<3, T, Q>& b)
        {
            return a.x <= b.x || a.y <= b.y || a.z <= b.z;
        }
        template<typename T, glm::qualifier Q> constexpr bool vec_any_lteq(const glm::vec<4, T, Q>& a, const glm::vec<4, T, Q>& b)
        {
            return a.x <= b.x || a.y <= b.y || a.z <= b.z || a.w <= b.w;
        }
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>::bounds(const value_type& min, const value_type& max) noexcept
        : min(min), max(max)
    {
        
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr const typename bounds<T, Dim, Align>::value_type& bounds<T, Dim, Align>::operator[](const size_t i) const noexcept
    {
        assert(i >= 0 && i <= 1 && "Index out of bounds.");
        return i==0 ? min : max;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr typename bounds<T, Dim, Align>::value_type& bounds<T, Dim, Align>::operator[](const size_t i) noexcept
    {
        assert(i >= 0 && i <= 1 && "Index out of bounds.");
        return i==0 ? min : max;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::operator+=(const bounds& other) noexcept
    {
        return enclose(other);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align> bounds<T, Dim, Align>::operator+(const bounds& other) const noexcept
    {
        auto b = *this;
        return b.enclose(other);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::operator+=(const value_type& other) noexcept
    {
        return enclose(other);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align> bounds<T, Dim, Align>::operator+(const value_type& other) const noexcept
    {
        auto b = *this;
        return b.enclose(other);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::enclose(const bounds& other) noexcept
    {
        enclose(other.max);
        enclose(other.min);
        return *this;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::enclose(const value_type& other) noexcept
    {
        min = vec_min(min, other);
        max = vec_max(max, other);
        return *this;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr typename bounds<T, Dim, Align>::value_type bounds<T, Dim, Align>::size() const noexcept
    {
        return vec_sub(max, min);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr typename bounds<T, Dim, Align>::value_type bounds<T, Dim, Align>::center() const noexcept
    {
        return vec_div_scalar(vec_add(max, min), 2.f);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::clip(const bounds& other) noexcept
    {
        min = vec_clamp(min, other.min, other.max);
        max = vec_clamp(max, other.min, other.max);
        return *this;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bounds<T, Dim, Align>& bounds<T, Dim, Align>::inset(const value_type& at_min, const value_type& at_max) noexcept
    {
        min = vec_add(min, at_min);
        max = vec_sub(max, at_max);
        return *this;
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bool bounds<T, Dim, Align>::contains(const bounds& other) const noexcept
    {
        return contains(other.max) && contains(other.min);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bool bounds<T, Dim, Align>::contains(const value_type& p) const noexcept
    {
        return vec_eq(vec_clamp(p, min, max), p);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bool bounds<T, Dim, Align>::operator==(const bounds& other) const noexcept
    {
        return vec_eq(min, other.min) && vec_eq(max, other.max);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bool bounds<T, Dim, Align>::operator!=(const bounds& other) const noexcept
    {
        return !operator==(other);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr typename bounds<T, Dim, Align>::component_type bounds<T, Dim, Align>::surface() const noexcept
    {
        return vec_surface(min, max);
    }

    template<typename T, size_t Dim, size_t Align>
    constexpr bool bounds<T, Dim, Align>::empty(component_type epsilon) const noexcept
    {
        return vec_any_lteq(size(), value_type(epsilon));
    }
}
