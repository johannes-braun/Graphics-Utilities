#pragma once

namespace gl
{
    template <typename T>
    uniform<T>::uniform(const uint32_t location, const gl_shader_program_t s) noexcept 
        : _shader(s), _value(new T()), _location(location)
    {}

    template <typename T>
    uniform<T>::operator T() const noexcept { return *_value; }

    template <typename T>
    uniform<T>& uniform<T>::operator=(const T& value) noexcept
    {
        *_value = value;
        update();
        return *this;
    }
}