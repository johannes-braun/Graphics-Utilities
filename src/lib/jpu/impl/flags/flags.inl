#pragma once

namespace jpu
{
    namespace flags_operators
    {
        template<typename E, typename = std::underlying_type_t<E>>
        flags<std::underlying_type_t<E>, E> operator|(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }

        template<typename E, typename = std::underlying_type_t<E>>
        flags<std::underlying_type_t<E>, E> operator&(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }

        template<typename E, typename = std::underlying_type_t<E>>
        flags<std::underlying_type_t<E>, E> operator^(E flag, E value)
        {
            return jpu::flags<std::underlying_type_t<E>, E>(flag) | value;
        }
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::flags(TEnum value)
        : _flags(static_cast<TBase>(value))
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
        return flags<TBase, TEnum>(static_cast<TEnum>(_flags | value._flags));
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator&(flags<TBase, TEnum> value) const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(_flags & value._flags));
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator^(flags<TBase, TEnum> value) const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(_flags ^ value._flags));
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator!() const
    {
        return static_cast<bool>(_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum> flags<TBase, TEnum>::operator~() const
    {
        return flags<TBase, TEnum>(static_cast<TEnum>(~_flags));
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator==(flags flags) const
    {
        return _flags == flags._flags;
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::operator!=(flags flags) const
    {
        return _flags != flags._flags;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator|=(flags flags)
    {
        _flags |= flags._flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator&=(flags flags)
    {
        _flags &= flags._flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>& flags<TBase, TEnum>::operator^=(flags flags)
    {
        _flags ^= flags._flags;
        return *this;
    }

    template <typename TBase, typename TEnum>
    bool flags<TBase, TEnum>::has(flags flags) const
    {
        return (_flags & flags._flags) != 0;
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator bool() const
    {
        return static_cast<bool>(_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator TEnum() const
    {
        return static_cast<TEnum>(_flags);
    }

    template <typename TBase, typename TEnum>
    flags<TBase, TEnum>::operator TBase() const
    {
        return _flags;
    }
}
