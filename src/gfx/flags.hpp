#pragma once

#include <type_traits>

namespace gfx
{
    template<typename TBase, typename TEnum>
    class flags
    {
        static_assert(std::is_enum_v<TEnum> && !std::is_convertible_v<TEnum, int>, "TEnum must be an enum class.");
        static_assert(std::is_integral_v<TBase>, "TBase must be an integral type.");
        static_assert(std::is_same_v<std::underlying_type_t<TEnum>, TBase>, "TEnum must have TBase as it's underlying type.");
    public:
        using base_type = TBase;
        using enum_type = TEnum;
        constexpr static TBase all_set = ~TBase(0);

        flags() = default;
        flags(TEnum value);
        flags(const flags& other) = default;
        flags(flags&& other) noexcept = default;
        flags& operator=(const flags& other) = default;
        flags& operator=(flags&& other) noexcept = default;

        template<typename B, typename E>
        friend flags<B, E> operator|(E flag, flags<B, E> value);
        template<typename B, typename E>
        friend flags<B, E> operator&(E flag, flags<B, E> value);
        template<typename B, typename E>
        friend flags<B, E> operator^(E flag, flags<B, E> value);

        flags operator|(flags value) const;
        flags operator&(flags value) const;
        flags operator^(flags value) const;
        bool operator!() const;
        flags operator~() const;

        bool operator==(flags flags) const;
        bool operator!=(flags flags) const;
        flags& operator|=(flags flags);
        flags& operator&=(flags flags);
        flags& operator^=(flags flags);

        bool has(flags flags) const;

        explicit operator bool() const;
        explicit operator TEnum() const;
        explicit operator TBase() const;

    private:
        TBase _flags = 0;
    };
} // namespace jpu

#include "flags.inl"