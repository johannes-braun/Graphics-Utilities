#pragma once

namespace gl
{
    template <typename T>
    uniform<T>::uniform(const uint32_t location, const uint32_t s) : _shader(s), _location(location)
    {}

    template <typename T>
    uniform<T>::operator T() const { return _value; }

    template <typename T>
    uniform<T>& uniform<T>::operator=(const T& value)
    {
        _value = value;
        update();
        return *this;
    }
}