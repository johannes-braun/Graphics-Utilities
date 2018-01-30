#pragma once

namespace jpu
{
    namespace flags_operators
    {
        template<typename E, typename = std::underlying_type_t<E>>
        jpu::flags<std::underlying_type_t<E>, E> operator|(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }

        template<typename E, typename = std::underlying_type_t<E>>
        jpu::flags<std::underlying_type_t<E>, E> operator&(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }

        template<typename E, typename = std::underlying_type_t<E>>
        jpu::flags<std::underlying_type_t<E>, E> operator^(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::flags(TEnum value)
        : m_flags(static_cast<TBase>(value))
    {}

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> operator|(TEnum flag, flags<TBase, TEnum> flags)
    {
        return flags | flag;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> operator&(TEnum flag, flags<TBase, TEnum> flags)
    {
        return flags & flag;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> operator^(TEnum flag, flags<TBase, TEnum> flags)
    {
        return flags ^ flag;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator|(flags<TBase, TEnum> value) const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(m_flags | value.m_flags));
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator&(flags<TBase, TEnum> value) const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(m_flags & value.m_flags));
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator^(flags<TBase, TEnum> value) const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(m_flags ^ value.m_flags));
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator!() const
    {
        return static_cast<bool>(m_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator~() const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(~m_flags));
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator==(flags flags) const
    {
        return m_flags == flags.m_flags;
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator!=(flags flags) const
    {
        return m_flags != flags.m_flags;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator|=(flags flags)
    {
        m_flags |= flags.m_flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator&=(flags flags)
    {
        m_flags &= flags.m_flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator^=(flags flags)
    {
        m_flags ^= flags.m_flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::has(flags flags) const
    {
        return (m_flags & flags.m_flags) != 0;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator bool() const
    {
        return static_cast<bool>(m_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator TEnum() const
    {
        return static_cast<TEnum>(m_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator TBase() const
    {
        return m_flags;
    }
}
