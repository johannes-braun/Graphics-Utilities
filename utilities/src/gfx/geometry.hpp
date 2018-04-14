#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/io.hpp>

namespace gfx
{
    using index32 = uint32_t;
    struct vertex
    {
        constexpr vertex() = default;
        constexpr vertex(glm::vec3 position) noexcept;
        constexpr vertex(glm::vec3 position, glm::vec3 norm) noexcept;
        constexpr vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 norm) noexcept;

        glm::vec3   position{ 0, 0, 0 };
        uint32_t    metadata_position   = 0;
        glm::vec3   normal{ 0, 1, 0 };
        uint32_t    metadata_normal     = 0;
        glm::vec2   uv{ 0, 0 };
        uint64_t    metadata_uv         = 0;
    };

    struct transform
    {
        constexpr transform() = default;
        constexpr transform(transform&& other) = default;
        constexpr transform(const transform& other) = default;
        constexpr transform& operator=(transform&& other) = default;
        constexpr transform& operator=(const transform& other) = default;
        constexpr transform(const glm::vec3& position, const glm::vec3& scale ={ 1,1,1 }, const glm::quat& rotation ={ 1, 0, 0, 0 }) noexcept;

        constexpr bool operator==(const transform& other) const noexcept;
        constexpr bool operator!=(const transform& other) const noexcept;

        constexpr transform operator*(const transform& other) const noexcept;
        constexpr transform& operator*=(const transform& other) noexcept;
        constexpr vertex operator*(const vertex& other) const noexcept;

        constexpr transform(const glm::mat4& mat) noexcept;
        constexpr operator glm::mat4() const noexcept;

        constexpr glm::vec3 up() const noexcept;
        constexpr glm::vec3 down() const noexcept;
        constexpr glm::vec3 left() const noexcept;
        constexpr glm::vec3 right() const noexcept;
        constexpr glm::vec3 forward() const noexcept;
        constexpr glm::vec3 backward() const noexcept;

        glm::vec3 position{ 0, 0, 0 };
        glm::vec3 scale{ 1, 1, 1 };
        glm::quat rotation{ 1, 0, 0, 0 };
    };

    template<typename T, size_t Dim, size_t Align>
    struct bounds
    {
        constexpr static size_t         dimensions  = Dim;
        constexpr static size_t         alignment   = Align;
        constexpr static glm::qualifier precision   = glm::highp;
        using component_type    = T;
        using value_type        = glm::vec<dimensions, component_type, precision>;

        constexpr bounds() = default;
        constexpr bounds(const value_type& min, const value_type& max) noexcept;

        constexpr const value_type& operator[](size_t i) const noexcept;
        constexpr value_type& operator[](size_t i) noexcept;

        constexpr bounds& operator+=(const bounds& other) noexcept;
        constexpr bounds& operator+=(const value_type& other) noexcept;
        constexpr bounds& enclose(const bounds& other) noexcept;
        constexpr bounds& enclose(const value_type& other) noexcept;
        constexpr bounds& clip(const bounds& other) noexcept;
        constexpr bounds& inset(const value_type& at_min, const value_type& at_max) noexcept;

        constexpr value_type size() const noexcept;
        constexpr bool contains(const bounds& other) const noexcept;
        constexpr bool contains(const value_type& p) const noexcept;
        constexpr bool operator==(const bounds& other) const noexcept;
        constexpr bool operator!=(const bounds& other) const noexcept;
        constexpr component_type surface() const noexcept;
        constexpr bool empty(component_type epsilon = std::numeric_limits<component_type>::epsilon()) const noexcept;

        alignas(alignment)value_type min = { std::numeric_limits<component_type>::max(), std::numeric_limits<component_type>::max(), std::numeric_limits<component_type>::max() };
        alignas(alignment)value_type max = { std::numeric_limits<component_type>::lowest(), std::numeric_limits<component_type>::lowest(), std::numeric_limits<component_type>::lowest() };
    };

    using line1f    = bounds<float, 1, 4>;
    using rect2f    = bounds<float, 2, 8>;
    using bounds3f  = bounds<float, 3, 16>;
    using bounds4f  = bounds<float, 4, 16>;
}

#include "geometry/vertex.inl"
#include "geometry/transform.inl"
#include "geometry/bounds.inl"

namespace gfx::cube_preset
{
    constexpr std::array<vertex, 24> vertices{
        //Back
        vertex({ -1, 1, -1 },{ 0, 1 },{ 0, 0, -1 }), vertex({ 1, 1, -1 },{ 1, 1 },{ 0, 0, -1 }), vertex({ 1, -1, -1 },{ 1, 0 },{ 0, 0, -1 }), vertex({ -1, -1, -1 },{ 0, 0 },{ 0, 0, -1 }),
        //Front
        vertex({ -1, 1, 1 },{ 0, 1 },{ 0, 0, 1 }), vertex({ 1, 1, 1 },{ 1, 1 },{ 0, 0, 1 }), vertex({ 1, -1, 1 },{ 1, 0 },{ 0, 0, 1 }), vertex({ -1, -1, 1 },{ 0, 0 },{ 0, 0, 1 }),
        //Bottom
        vertex({ -1, -1, -1 },{ 0, 0 },{ 0, -1, 0 }), vertex({ 1, -1, -1 },{ 0, 1 },{ 0, -1, 0 }), vertex({ 1, -1, 1 },{ 1, 1 },{ 0, -1, 0 }), vertex({ -1, -1, 1 },{ 0, 1 },{ 0, -1, 0 }),
        //Top
        vertex({ -1, 1, -1 },{ 0, 0 },{ 0, 1, 0 }), vertex({ 1, 1, -1 },{ 1, 0 },{ 0, 1, 0 }), vertex({ 1, 1, 1 },{ 1, 1 },{ 0, 1, 0 }), vertex({ -1, 1, 1 },{ 0, 1 },{ 0, 1, 0 }),
        //Right
        vertex({ 1, -1, -1 },{ 0, 0 },{ 1, 0, 0 }), vertex({ 1, 1, -1 },{ 1, 0 },{ 1, 0, 0 }), vertex({ 1, 1, 1 },{ 1, 1 },{ 1, 0, 0 }), vertex({ 1, -1, 1 },{ 0, 1 },{ 1, 0, 0 }),
        //Left
        vertex({ -1, -1, -1 },{ 0, 0 },{ -1, 0, 0 }), vertex({ -1, 1, -1 },{ 1, 0 },{ -1, 0, 0 }), vertex({ -1, 1, 1 },{ 1, 1 },{ -1, 0, 0 }), vertex({ -1, -1, 1 },{ 0, 1 },{ -1, 0, 0 }),
    };

    constexpr std::array<index32, 36> indices{
        0, 1, 2,        0, 2, 3,
        6, 5, 4,        7, 6, 4,
        8, 9, 10,       8, 10, 11,
        14, 13, 12,     15, 14, 12,
        16, 17, 18,     16, 18, 19,
        22, 21, 20,     23, 22, 20,
    };

    constexpr bounds3f bounds({ -1, -1, -1 }, { 1, 1, 1 });
}