#include "../uniform.hpp"

namespace gl
{
    template <>
    void uniform<int>::update() const noexcept
    {
        glProgramUniform1iv(_shader, _location, 1, &*_value);
    }

    template <>
    void uniform<glm::ivec2>::update() const noexcept
    {
        glProgramUniform2iv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::ivec3>::update() const noexcept
    {
        glProgramUniform3iv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::ivec4>::update() const noexcept
    {
        glProgramUniform4iv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::uint>::update() const noexcept
    {
        glProgramUniform1ui(_shader, _location, *_value);
    }

    template <>
    void uniform<glm::uvec2>::update() const noexcept
    {
        glProgramUniform2uiv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::uvec3>::update() const noexcept
    {
        glProgramUniform3uiv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::uvec4>::update() const noexcept
    {
        glProgramUniform4uiv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<uint64_t>::update() const noexcept
    {
        glProgramUniform1ui64ARB(_shader, _location, *_value);
    }

    template <>
    void uniform<float>::update() const noexcept
    {
        glProgramUniform1f(_shader, _location, *_value);
    }

    template <>
    void uniform<glm::vec2>::update() const noexcept
    {
        glProgramUniform2fv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::vec3>::update() const noexcept
    {
        glProgramUniform3fv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::vec4>::update() const noexcept
    {
        glProgramUniform4fv(_shader, _location, 1, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat2>::update() const noexcept
    {
        glProgramUniformMatrix2fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat2x3>::update() const noexcept
    {
        glProgramUniformMatrix2x3fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat2x4>::update() const noexcept
    {
        glProgramUniformMatrix2x4fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat3>::update() const noexcept
    {
        glProgramUniformMatrix3fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat3x2>::update() const noexcept
    {
        glProgramUniformMatrix3x2fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat3x4>::update() const noexcept
    {
        glProgramUniformMatrix3x4fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat4>::update() const noexcept
    {
        glProgramUniformMatrix4fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat4x2>::update() const noexcept
    {
        glProgramUniformMatrix4x2fv(_shader, _location, 1, false, value_ptr(*_value));
    }

    template <>
    void uniform<glm::mat4x3>::update() const noexcept
    {
        glProgramUniformMatrix4x3fv(_shader, _location, 1, false, value_ptr(*_value));
    }
}