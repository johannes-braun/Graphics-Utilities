#pragma once

#include "glad/glad.h"
#include <glm/ext.hpp>
#include <variant>

namespace gl
{
    class shader;
    class pipeline;
    class graphics_pipeline;
    class compute_pipeline;

    template<typename T>
    class uniform
    {
        friend shader;
        friend pipeline;
        friend graphics_pipeline;
        friend compute_pipeline;
    public:

        uniform(uniform&&) = default;
        uniform(const uniform&) = default;
        uniform(uint32_t location, uint32_t s) : _shader(s), _location(location) {}

        operator T() const { return _value; }
        uniform& operator=(const T& value)
        {
            _value = value;
            update();
            return *this;
        }

    protected:
        void update() const;

        uint32_t _shader;
        T _value;
        uint32_t _location;
    };

    using sampler1D = uint64_t;
    using isampler1D = uint64_t;
    using usampler1D = uint64_t;
    using sampler2D = uint64_t;
    using isampler2D = uint64_t;
    using usampler2D = uint64_t;
    using sampler3D = uint64_t;
    using isampler3D = uint64_t;
    using usampler3D = uint64_t;
    using samplerCube = uint64_t;
    using isamplerCube = uint64_t;
    using usamplerCube = uint64_t;
    using sampler2DRect = uint64_t;
    using isampler2DRect = uint64_t;
    using usampler2DRect = uint64_t;
    using sampler1DArray = uint64_t;
    using isampler1DArray = uint64_t;
    using usampler1DArray = uint64_t;
    using sampler2DArray = uint64_t;
    using isampler2DArray = uint64_t;
    using usampler2DArray = uint64_t;
    using samplerCubeArray = uint64_t;
    using isamplerCubeArray = uint64_t;
    using usamplerCubeArray = uint64_t;
    using samplerBuffer = uint64_t;
    using isamplerBuffer = uint64_t;
    using usamplerBuffer = uint64_t;
    using sampler2DMS = uint64_t;
    using isampler2DMS = uint64_t;
    using usampler2DMS = uint64_t;
    using sampler2DMSArray = uint64_t;
    using isampler2DMSArray = uint64_t;
    using usampler2DMSArray = uint64_t;

    using sampler1DShadow = uint64_t;
    using sampler2DShadow = uint64_t;
    using samplerCubeShadow = uint64_t;
    using sampler2DRectShadow = uint64_t;
    using sampler1DArrayShadow = uint64_t;
    using sampler2DArrayShadow = uint64_t;
    using samplerCubeArrayShadow = uint64_t;

    template<> inline void uniform<int>::update() const { glProgramUniform1iv(_shader, _location, 1, &_value); }
    template<> inline void uniform<glm::ivec2>::update() const { glProgramUniform2iv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::ivec3>::update() const { glProgramUniform3iv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::ivec4>::update() const { glProgramUniform4iv(_shader, _location, 1, glm::value_ptr(_value)); }

    template<> inline void uniform<glm::uint>::update() const { glProgramUniform1ui(_shader, _location, _value); }
    template<> inline void uniform<glm::uvec2>::update() const { glProgramUniform2uiv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::uvec3>::update() const { glProgramUniform3uiv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::uvec4>::update() const { glProgramUniform4uiv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<uint64_t>::update() const { glProgramUniform1ui64ARB(_shader, _location, _value); }
    template<> inline void uniform<intptr_t>::update() const { glProgramUniform1i64ARB(_shader, _location, _value); }

    template<> inline void uniform<float>::update() const { glProgramUniform1f(_shader, _location, _value); }
    template<> inline void uniform<glm::vec2>::update() const { glProgramUniform2fv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::vec3>::update() const { glProgramUniform3fv(_shader, _location, 1, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::vec4>::update() const { glProgramUniform4fv(_shader, _location, 1, glm::value_ptr(_value)); }

    template<> inline void uniform<glm::mat2>::update() const { glProgramUniformMatrix2fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat2x3>::update() const { glProgramUniformMatrix2x3fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat2x4>::update() const { glProgramUniformMatrix2x4fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat3>::update() const { glProgramUniformMatrix3fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat3x2>::update() const { glProgramUniformMatrix3x2fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat3x4>::update() const { glProgramUniformMatrix3x4fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat4>::update() const { glProgramUniformMatrix4fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat4x2>::update() const { glProgramUniformMatrix4x2fv(_shader, _location, 1, false, glm::value_ptr(_value)); }
    template<> inline void uniform<glm::mat4x3>::update() const { glProgramUniformMatrix4x3fv(_shader, _location, 1, false, glm::value_ptr(_value)); }

    using all_uniform_types = std::variant <
        uniform<int>,
        uniform<glm::ivec2>,
        uniform<glm::ivec3>,
        uniform<glm::ivec4>,

        uniform<glm::uint>,
        uniform<glm::uvec2>,
        uniform<glm::uvec3>,
        uniform<glm::uvec4>,

        uniform<uint64_t>,
        uniform<intptr_t>,

        uniform<float>,
        uniform<glm::vec2>,
        uniform<glm::vec3>,
        uniform<glm::vec4>,

        uniform<glm::mat2>,
        uniform<glm::mat2x3>,
        uniform<glm::mat2x4>,
        uniform<glm::mat3>,
        uniform<glm::mat3x2>,
        uniform<glm::mat3x4>,
        uniform<glm::mat4>,
        uniform<glm::mat4x2>,
        uniform<glm::mat4x3>
    > ;
}
