#pragma once

#include <array>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <variant>

namespace gfx
{
using index32 = uint32_t;
using index16 = uint16_t;

struct vertex3d
{
    constexpr vertex3d() = default;
    constexpr vertex3d(glm::vec3 position) noexcept;
    constexpr vertex3d(glm::vec3 position, glm::vec3 norm) noexcept;
    constexpr vertex3d(glm::vec3 position, glm::vec2 uv, glm::vec3 norm) noexcept;

    glm::vec3 position{0, 0, 0};
    uint32_t  metadata_position = 0;
    glm::vec3 normal{0, 1, 0};
    uint32_t  metadata_normal = 0;
    glm::vec2 uv{0, 0};
    uint64_t  metadata_uv = 0;
};

struct vertex2d
{
    constexpr vertex2d() = default;
    constexpr vertex2d(glm::vec2 position) noexcept;
    constexpr vertex2d(glm::vec2 position, glm::vec2 uv) noexcept;
    constexpr vertex2d(glm::vec2 position, glm::vec2 uv, glm::u8vec4 color) noexcept;

    alignas(16) glm::vec2 position{0, 0};
    alignas(8) glm::vec2 uv{0, 0};
    alignas(8) glm::u8vec4 color{glm::u8vec1(0), glm::u8vec1(0), glm::u8vec1(0), glm::u8vec1(255)};
};

struct transform
{
    constexpr transform()                       = default;
    constexpr transform(transform&& other)      = default;
    constexpr transform(const transform& other) = default;
    constexpr transform& operator=(transform&& other) = default;
    constexpr transform& operator=(const transform& other) = default;
    constexpr transform(const glm::vec3& position, const glm::vec3& scale = {1, 1, 1}, const glm::quat& rotation = {1, 0, 0, 0}) noexcept;

    constexpr bool operator==(const transform& other) const noexcept;
    constexpr bool operator!=(const transform& other) const noexcept;

    constexpr transform  operator*(const transform& other) const noexcept;
    constexpr transform& operator*=(const transform& other) noexcept;
    constexpr vertex3d   operator*(const vertex3d& other) const noexcept;
    
    constexpr glm::mat4 matrix() const noexcept;
    constexpr transform(const glm::mat4& mat) noexcept;
    constexpr operator glm::mat4() const noexcept;

    constexpr glm::vec3 up() const noexcept;
    constexpr glm::vec3 down() const noexcept;
    constexpr glm::vec3 left() const noexcept;
    constexpr glm::vec3 right() const noexcept;
    constexpr glm::vec3 forward() const noexcept;
    constexpr glm::vec3 backward() const noexcept;

    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::quat rotation{1, 0, 0, 0};
};

class projection
{
    struct perspective_info
    {
        float field_of_view = glm::radians(80.f);
        int   screen_width  = 100;
        int   screen_height = 100;
        float clip_near     = 0.1f;
        float clip_far      = 1000.f;
        bool  negative_y    = false;
        bool  inverse_z     = true;
    };

    struct orthographic_info
    {
        float left      = -1;
        float right     = 1;
        float bottom    = -1;
        float top       = 1;
        float clip_near = -1.f;
        float clip_far  = -1.f;
    };

    enum class type
    {
        perspective,
        orthographic
    };

public:
    constexpr projection(float fov, int width, int height, float znear, float zfar, bool neg_y = false, bool inv_z = true) noexcept;
    constexpr projection(float left, float right, float bottom, float top, float znear = -1.f, float zfar = -1.f) noexcept;

    constexpr const orthographic_info& orthographic() const;
    constexpr orthographic_info&       orthographic();
    constexpr const perspective_info&  perspective() const;
    constexpr perspective_info&        perspective();
    glm::mat4                          matrix() const noexcept;
                                       operator glm::mat4() const noexcept;

private:
    type                                              _type;
    std::variant<perspective_info, orthographic_info> _info;
};

template <typename T, size_t Dim, size_t Align> struct bounds
{
    constexpr static size_t         dimensions = Dim;
    constexpr static size_t         alignment  = Align;
    constexpr static glm::qualifier precision  = glm::highp;
    using component_type                       = T;
    using value_type                           = glm::vec<dimensions, component_type, precision>;

    constexpr bounds() = default;
    constexpr bounds(const value_type& min, const value_type& max) noexcept;

    constexpr const value_type& operator[](size_t i) const noexcept;
    constexpr value_type&       operator[](size_t i) noexcept;

    constexpr bounds& operator+=(const bounds& other) noexcept;
    constexpr bounds& operator+=(const value_type& other) noexcept;
    constexpr bounds& enclose(const bounds& other) noexcept;
    constexpr bounds& enclose(const value_type& other) noexcept;
    constexpr bounds& clip(const bounds& other) noexcept;
    constexpr bounds& inset(const value_type& at_min, const value_type& at_max) noexcept;

    constexpr value_type     size() const noexcept;
    constexpr bool           contains(const bounds& other) const noexcept;
    constexpr bool           contains(const value_type& p) const noexcept;
    constexpr bool           operator==(const bounds& other) const noexcept;
    constexpr bool           operator!=(const bounds& other) const noexcept;
    constexpr component_type surface() const noexcept;
    constexpr bool           empty(component_type epsilon = std::numeric_limits<component_type>::epsilon()) const noexcept;

    alignas(alignment) value_type min = {std::numeric_limits<component_type>::max(),
                                         std::numeric_limits<component_type>::max(),
                                         std::numeric_limits<component_type>::max()};
    alignas(alignment) value_type max = {std::numeric_limits<component_type>::lowest(),
                                         std::numeric_limits<component_type>::lowest(),
                                         std::numeric_limits<component_type>::lowest()};
};

using line1f   = bounds<float, 1, 4>;
using rect2f   = bounds<float, 2, 8>;
using bounds3f = bounds<float, 3, 16>;
using bounds4f = bounds<float, 4, 16>;
}

#include "geometry/bounds.inl"
#include "geometry/projection.inl"
#include "geometry/transform.inl"
#include "geometry/vertex.inl"

namespace gfx::cube_preset
{
constexpr std::array<vertex3d, 24> vertices{
        // Back
        vertex3d({-1, 1, -1}, {0, 1}, {0, 0, -1}),
        vertex3d({1, 1, -1}, {1, 1}, {0, 0, -1}),
        vertex3d({1, -1, -1}, {1, 0}, {0, 0, -1}),
        vertex3d({-1, -1, -1}, {0, 0}, {0, 0, -1}),
        // Front
        vertex3d({-1, 1, 1}, {0, 1}, {0, 0, 1}),
        vertex3d({1, 1, 1}, {1, 1}, {0, 0, 1}),
        vertex3d({1, -1, 1}, {1, 0}, {0, 0, 1}),
        vertex3d({-1, -1, 1}, {0, 0}, {0, 0, 1}),
        // Bottom
        vertex3d({-1, -1, -1}, {0, 0}, {0, -1, 0}),
        vertex3d({1, -1, -1}, {0, 1}, {0, -1, 0}),
        vertex3d({1, -1, 1}, {1, 1}, {0, -1, 0}),
        vertex3d({-1, -1, 1}, {0, 1}, {0, -1, 0}),
        // Top
        vertex3d({-1, 1, -1}, {0, 0}, {0, 1, 0}),
        vertex3d({1, 1, -1}, {1, 0}, {0, 1, 0}),
        vertex3d({1, 1, 1}, {1, 1}, {0, 1, 0}),
        vertex3d({-1, 1, 1}, {0, 1}, {0, 1, 0}),
        // Right
        vertex3d({1, -1, -1}, {0, 0}, {1, 0, 0}),
        vertex3d({1, 1, -1}, {1, 0}, {1, 0, 0}),
        vertex3d({1, 1, 1}, {1, 1}, {1, 0, 0}),
        vertex3d({1, -1, 1}, {0, 1}, {1, 0, 0}),
        // Left
        vertex3d({-1, -1, -1}, {0, 0}, {-1, 0, 0}),
        vertex3d({-1, 1, -1}, {1, 0}, {-1, 0, 0}),
        vertex3d({-1, 1, 1}, {1, 1}, {-1, 0, 0}),
        vertex3d({-1, -1, 1}, {0, 1}, {-1, 0, 0}),
};

constexpr std::array<index32, 36> indices{
        0, 1, 2, 0, 2, 3, 6, 5, 4, 7, 6, 4, 8, 9, 10, 8, 10, 11, 14, 13, 12, 15, 14, 12, 16, 17, 18, 16, 18, 19, 22, 21, 20, 23, 22, 20,
};

constexpr bounds3f bounds({-1, -1, -1}, {1, 1, 1});
}