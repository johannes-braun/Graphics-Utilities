#pragma once

#include <mygl/gl.hpp>
#include <glm/ext.hpp>
#include <variant>
#include <memory>

namespace gl
{
    class shader;
    class pipeline;
    class graphics_pipeline;
    class compute_pipeline;

    template <typename T>
    class uniform
    {
        friend shader;
        friend pipeline;
        friend graphics_pipeline;
        friend compute_pipeline;
    public:
        uniform(uniform&&) = default;
        uniform(const uniform&) = default;
        operator T() const noexcept;
        uniform& operator=(const T& value) noexcept;

    protected:
        uniform(uint32_t location, gl_shader_program_t s) noexcept;
        void update() const noexcept;

        gl_shader_program_t _shader;
        std::shared_ptr<T> _value;
        uint32_t _location;
    };
    
    using all_uniform_types = std::variant<
        uniform<glm::int32>, uniform<glm::ivec2>, uniform<glm::ivec3>, uniform<glm::ivec4>,

        uniform<glm::uint32>, uniform<glm::uvec2>, uniform<glm::uvec3>, uniform<glm::uvec4>,

        uniform<glm::float32>, uniform<glm::vec2>, uniform<glm::vec3>, uniform<glm::vec4>,

        uniform<glm::mat2>, uniform<glm::mat2x3>, uniform<glm::mat2x4>,
        uniform<glm::mat3>, uniform<glm::mat3x2>, uniform<glm::mat3x4>,
        uniform<glm::mat4>, uniform<glm::mat4x2>, uniform<glm::mat4x3>,

        uniform<uint64_t>
    >;
}

#include "uniform.inl"