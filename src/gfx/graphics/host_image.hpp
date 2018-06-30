#pragma once
#include "host_buffer.hpp"
#include <cinttypes>
#include <gfx/file.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <functional>

namespace gfx
{
enum img_format
{
    // unorm
    r8unorm,
    rg8unorm,
    rgb8unorm,
    rgba8unorm,
    r16unorm,
    rg16unorm,
    rgb16unorm,
    rgba16unorm,

    // snorm
    r8snorm,
    rg8snorm,
    rgb8snorm,
    rgba8snorm,
    r16snorm,
    rg16snorm,
    rgb16snorm,
    rgba16snorm,

    // uint
    r8u,
    rg8u,
    rgb8u,
    rgba8u,
    r16u,
    rg16u,
    rgb16u,
    rgba16u,
    r32u,
    rg32u,
    rgb32u,
    rgba32u,

    // int
    r8i,
    rg8i,
    rgb8i,
    rgba8i,
    r16i,
    rg16i,
    rgb16i,
    rgba16i,
    r32i,
    rg32i,
    rgb32i,
    rgba32i,

    // float
    r16f,
    rg16f,
    rgb16f,
    rgba16f,
    r32f,
    rg32f,
    rgb32f,
    rgba32f,

    // special formats
    rgb5a1unorm,
    rgb10a2unorm,
    rgb10a2snorm,
    r11g11b10f,
    rgb9e5,
    r5g6b5unorm,
};

struct extent
{
    constexpr extent(uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
    constexpr extent(extent extent1d, uint32_t height, uint32_t depth);
    constexpr extent(extent extent2d, uint32_t depth);

    constexpr        operator glm::uvec3() const noexcept;
    constexpr size_t count() const noexcept;

    constexpr extent as_1d() const noexcept;
    constexpr extent as_2d() const noexcept;

    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
};

enum class data_format
{
    r    = 1,
    rg   = 2,
    rgb  = 3,
    rgba = 4
};

class host_image
{
public:
    host_image(img_format format, const extent& size);
    host_image(img_format format, const image_file& file);

    void update(data_format format, const uint8_t* data);
    void update(data_format format, const uint16_t* data);
    void update(data_format format, const uint32_t* data);
    void update(data_format format, const int8_t* data);
    void update(data_format format, const int16_t* data);
    void update(data_format format, const int32_t* data);
    void update(data_format format, const float* data);
    void update(const image_file& file);

    const host_buffer<std::byte>& storage() const noexcept;

    const extent& extents() const noexcept;

    glm::vec4  load(const glm::uvec3& pixel) const;
    glm::uvec4 loadu(const glm::uvec3& pixel) const;
    glm::ivec4 loadi(const glm::uvec3& pixel) const;
    void       store(const glm::uvec3& pixel, const glm::vec4& p) ;
    void       storeu(const glm::uvec3& pixel, const glm::uvec4& p) ;
    void       storei(const glm::uvec3& pixel, const glm::ivec4& p) ;

private:
    std::function<void(const glm::vec4&, int64_t i)> get_write_unorm_fun();

    img_format                                         _format;
    extent                                             _extent;
    size_t                                             _storage_element_size;
    host_buffer<std::byte>                             _storage;
    std::function<void(const glm::vec4& v, int64_t i)> _write_unorm;
};
} // namespace gfx

#include "host_image.inl"