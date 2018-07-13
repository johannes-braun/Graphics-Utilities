#pragma once
#include "formats.hpp"
#include "host_buffer.hpp"
#include <cinttypes>
#include <functional>
#include <gfx/file.hpp>
#include <glm/ext.hpp>
#include <vector>

namespace gfx
{
struct extent
{
    constexpr extent(uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
    constexpr extent(extent extent1d, uint32_t height, uint32_t depth);
    constexpr extent(extent extent2d, uint32_t depth);

    constexpr        operator glm::uvec3() const noexcept;
    constexpr size_t count() const noexcept;

    constexpr extent   as_1d() const noexcept;
    constexpr extent   as_2d() const noexcept;
    constexpr uint32_t linear(const glm::uvec3& p) const noexcept { return width * height * p.z + width * p.y + p.x; }

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
    host_image(format fmt, const std::filesystem::path& file);

    host_image converted(format fmt) const;
    void       flip_vertically();

    void update(data_format fmt, const uint8_t* data);
    void update(data_format fmt, const uint16_t* data);
    void update(data_format fmt, const uint32_t* data);
    void update(data_format fmt, const int8_t* data);
    void update(data_format fmt, const int16_t* data);
    void update(data_format fmt, const int32_t* data);
    void update(data_format fmt, const float* data);
    void update(const image_file& file);

    host_buffer<std::byte>&       storage() noexcept;
    const host_buffer<std::byte>& storage() const noexcept;

    const extent& extents() const noexcept;
    uint32_t      max_levels() const noexcept;
    format        pixel_format() const noexcept;

    glm::vec4  load(const glm::uvec3& pixel) const;
    glm::uvec4 loadu(const glm::uvec3& pixel) const;
    glm::ivec4 loadi(const glm::uvec3& pixel) const;
    void       store(const glm::uvec3& pixel, const glm::vec4& p);
    void       storeu(const glm::uvec3& pixel, const glm::uvec4& p);
    void       storei(const glm::uvec3& pixel, const glm::ivec4& p);

	auto operator*(float f)
	{
        gfx::host_image img(pixel_format(), _extent);
#pragma omp parallel for schedule(static)
        for (auto x = 0; x < _extent.width; ++x)
            for (auto y = 0u; y < _extent.height; ++y)
                for (auto z = 0u; z < _extent.depth; ++z) img.store({uint32_t(x), y, z}, f * load({uint32_t(x), y, z}));
        return img;
	}

    auto& operator*=(float f)
    {
#pragma omp parallel for schedule(static)
        for (auto x = 0; x < _extent.width; ++x)
            for (auto y = 0u; y < _extent.height; ++y)
                for (auto z = 0u; z < _extent.depth; ++z) store({uint32_t(x), y, z}, f * load({uint32_t(x), y, z}));
        return *this;
    }

    auto operator+(const gfx::host_image& image)
    {
        gfx::host_image img(pixel_format(), _extent);
        const auto wrap = [&image](glm::uvec3 px) {
            return px % glm::uvec3(image.extents().width, image.extents().height, image.extents().depth);
        };

#pragma omp parallel for schedule(static)
        for (auto x = 0; x < _extent.width; ++x)
            for (auto y = 0u; y < _extent.height; ++y)
                for (auto z = 0u; z < _extent.depth; ++z)
                    img.store({uint32_t(x), y, z}, load({uint32_t(x), y, z}) + image.load(wrap({uint32_t(x), y, z})));
        return img;
    }

    auto& operator+=(const gfx::host_image& image)
    {
        const auto wrap = [&image](glm::uvec3 px) {
            return px % glm::uvec3(image.extents().width, image.extents().height, image.extents().depth);
        };

#pragma omp parallel for schedule(static)
        for (auto x = 0; x < _extent.width; ++x)
            for (auto y = 0u; y < _extent.height; ++y)
                for (auto z = 0u; z < _extent.depth; ++z)
                    store({uint32_t(x), y, z}, load({uint32_t(x), y, z}) + image.load(wrap({uint32_t(x), y, z})));
        return *this;
    }

    private:
    std::function<void(const glm::vec4&, int64_t i)> get_write_unorm_fun();

    format                                             _format;
    extent                                             _extent;
    size_t                                             _storage_element_size;
    host_buffer<std::byte>                             _storage;
    std::function<void(const glm::vec4& v, int64_t i)> _write_unorm;
};
}    // namespace gfx

#include "host_image.inl"