#pragma once
#include <variant>

namespace gl
{
    template<typename T>
    uniform<T> shader::uniform(const std::string& name)
    {
        if (_uniforms.count(name) == 0)
            _uniforms.emplace(name, all_uniform_types(gl::uniform<T>(glGetUniformLocation(_id, name.data()), _id)));
        return std::get<gl::uniform<T>>(_uniforms.at(name));
    }
}