#pragma once
#include "host_buffer.hpp"
#include <cinttypes>
#include <gfx/file.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <functional>
#include "formats.hpp"

namespace gfx
{
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
    host_image(format fmt, const extent& size);
    host_image(format fmt, const image_file& file);

    void update(data_format fmt, const uint8_t* data);
    void update(data_format fmt, const uint16_t* data);
    void update(data_format fmt, const uint32_t* data);
    void update(data_format fmt, const int8_t* data);
    void update(data_format fmt, const int16_t* data);
    void update(data_format fmt, const int32_t* data);
    void update(data_format fmt, const float* data);
    void update(const image_file& file);

    host_buffer<std::byte>& storage() noexcept;
    const host_buffer<std::byte>& storage() const noexcept;

    const extent& extents() const noexcept;
    uint32_t      max_levels() const noexcept;

    glm::vec4  load(const glm::uvec3& pixel) const;
    glm::uvec4 loadu(const glm::uvec3& pixel) const;
    glm::ivec4 loadi(const glm::uvec3& pixel) const;
    void       store(const glm::uvec3& pixel, const glm::vec4& p) ;
    void       storeu(const glm::uvec3& pixel, const glm::uvec4& p) ;
    void       storei(const glm::uvec3& pixel, const glm::ivec4& p) ;

private:
    std::function<void(const glm::vec4&, int64_t i)> get_write_unorm_fun();

    format                                         _format;
    extent                                             _extent;
    size_t                                             _storage_element_size;
    host_buffer<std::byte>                             _storage;
    std::function<void(const glm::vec4& v, int64_t i)> _write_unorm;
};
} // namespace gfx

#include "host_image.inl"