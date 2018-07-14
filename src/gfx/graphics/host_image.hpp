#pragma once
#include "formats.hpp"
#include "host_buffer.hpp"
#include <cinttypes>
#include <functional>
#include <gfx/file.hpp>
#include <glm/ext.hpp>

namespace gfx
{
union extent
{
    constexpr extent(uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
    constexpr extent(extent extent1d, uint32_t height, uint32_t depth);
    constexpr extent(extent extent2d, uint32_t depth);

    constexpr size_t count() const noexcept;

    constexpr extent     as_1d() const noexcept;
    constexpr extent     as_2d() const noexcept;
    constexpr uint32_t linear(const glm::uvec3& p) const noexcept;
    constexpr glm::uvec3 subpixel(uint32_t index) const noexcept;
    constexpr operator const glm::uvec3&() const noexcept;
    constexpr glm::ivec3 clamp(glm::ivec3 pixel) const noexcept;
    constexpr glm::ivec3 wrap(glm::ivec3 pixel) const noexcept;

    bool operator==(const extent& other) const { return vec == other.vec; }
    bool operator!=(const extent& other) const { return vec != other.vec; }

    struct
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };
    glm::uvec3 vec;
};

enum class data_format
{
    r    = 1,
    rg   = 2,
    rgb  = 3,
    rgba = 4
};


bool is_unsigned(format fmt);
bool is_signed(format fmt);
bool is_unorm_compatible(format fmt);

class host_image
{
    public:
    host_image(format fmt, const extent& size);
    host_image(format fmt, const image_file& file);
    host_image(format fmt, const std::filesystem::path& file);

    host_image(const host_image& o);
    host_image& operator=(const host_image& o);
    host_image(host_image&& o) = default;
    host_image& operator=(host_image&& o) = default;

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
    glm::vec4  load_bilinear(const glm::vec3& pixel) const;
    glm::uvec4 loadu(const glm::uvec3& pixel) const;
    glm::ivec4 loadi(const glm::uvec3& pixel) const;
    void       store(const glm::uvec3& pixel, const glm::vec4& p);
    void       storeu(const glm::uvec3& pixel, const glm::uvec4& p);
    void       storei(const glm::uvec3& pixel, const glm::ivec4& p);

    using available_types = std::variant<float, int32_t, uint32_t, glm::vec4, glm::ivec4, glm::uvec4, host_image>;
    template<typename T> using enable_if_available = decltype(std::get<std::decay_t<T>>(std::declval<available_types>()));

    template<typename T, typename = enable_if_available<T>>
    host_image operator+(const T& value) const;
    template<typename T, typename = enable_if_available<T>>
    host_image operator-(const T& value) const;
    template<typename T, typename = enable_if_available<T>>
    host_image operator*(const T& value) const;
    template<typename T, typename = enable_if_available<T>>
    host_image operator/(const T& value) const;
    template<typename T, typename = enable_if_available<T>>
    host_image& operator+=(const T& value);
    template<typename T, typename = enable_if_available<T>>
    host_image& operator-=(const T& value);
    template<typename T, typename = enable_if_available<T>>
    host_image& operator*=(const T& value);
    template<typename T, typename = enable_if_available<T>>
    host_image& operator/=(const T& value);

    bool operator==(const host_image& image) const;
    bool operator!=(const host_image& image) const;

private:
    template<typename OpFun>
    host_image& operator_apply(host_image& store_target, const host_image& image, OpFun&& fun) const;
    template<typename Scalar, typename OpFun, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Scalar>, host_image>>>
    host_image& operator_apply(host_image& store_target, Scalar image, OpFun&& fun) const;
    template<typename T, typename OpFun>
    host_image operator_nonapply(const T& val, OpFun&& fun) const
    {
        host_image img(pixel_format(), _extent);
        operator_apply(img, val, std::forward<OpFun&&>(fun));
        return img;
    }

    std::function<void(const glm::vec4&, int64_t i)> get_write_unorm_fun();

    format                                             _format;
    extent                                             _extent;
    size_t                                             _storage_element_size;
    host_buffer<std::byte>                             _storage;
    std::function<void(const glm::vec4& v, int64_t i)> _write_unorm;
};
}    // namespace gfx

#include "host_image.inl"