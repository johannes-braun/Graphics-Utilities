#include "uniform.hpp"

namespace gl
{
    template <>
    void uniform<int>::update() const
    {
        glProgramUniform1iv(_shader, _location, 1, &_value);
    }

    template <>
    void uniform<glm::ivec2>::update() const
    {
        glProgramUniform2iv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::ivec3>::update() const
    {
        glProgramUniform3iv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::ivec4>::update() const
    {
        glProgramUniform4iv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::uint>::update() const
    {
        glProgramUniform1ui(_shader, _location, _value);
    }

    template <>
    void uniform<glm::uvec2>::update() const
    {
        glProgramUniform2uiv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::uvec3>::update() const
    {
        glProgramUniform3uiv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::uvec4>::update() const
    {
        glProgramUniform4uiv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<uint64_t>::update() const
    {
        glProgramUniform1ui64ARB(_shader, _location, _value);
    }

    template <>
    void uniform<float>::update() const
    {
        glProgramUniform1f(_shader, _location, _value);
    }

    template <>
    void uniform<glm::vec2>::update() const
    {
        glProgramUniform2fv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::vec3>::update() const
    {
        glProgramUniform3fv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::vec4>::update() const
    {
        glProgramUniform4fv(_shader, _location, 1, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat2>::update() const
    {
        glProgramUniformMatrix2fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat2x3>::update() const
    {
        glProgramUniformMatrix2x3fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat2x4>::update() const
    {
        glProgramUniformMatrix2x4fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat3>::update() const
    {
        glProgramUniformMatrix3fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat3x2>::update() const
    {
        glProgramUniformMatrix3x2fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat3x4>::update() const
    {
        glProgramUniformMatrix3x4fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat4>::update() const
    {
        glProgramUniformMatrix4fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat4x2>::update() const
    {
        glProgramUniformMatrix4x2fv(_shader, _location, 1, false, value_ptr(_value));
    }

    template <>
    void uniform<glm::mat4x3>::update() const
    {
        glProgramUniformMatrix4x3fv(_shader, _location, 1, false, value_ptr(_value));
    }
}