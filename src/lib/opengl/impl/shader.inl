#pragma once

namespace gl
{
    template<typename T>
    uniform<T> shader::get_uniform(std::string_view name)
    {
        if (_uniforms.count(name) == 0)
        {
            std::pair<std::string_view, all_uniform_types> p(name, all_uniform_types(gl::uniform<T>(glGetUniformLocation(_id, name.data()), _id)));
            _uniforms.insert(p);
        }

        return std::get<uniform<T>>(_uniforms.at(name));
    }
}