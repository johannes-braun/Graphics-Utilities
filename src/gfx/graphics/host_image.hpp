#pragma once
#include "formats.hpp"
#include "host_buffer.hpp"
#include <cinttypes>
#include <functional>
#include <gfx/file/file.hpp>
#include <glm/ext.hpp>

namespace gfx {
inline namespace v1 {
union extent
{
    constexpr extent(uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
    constexpr extent(extent extent1d, uint32_t height, uint32_t depth);
    constexpr extent(extent extent2d, uint32_t depth);

    constexpr size_t count() const noexcept;

    constexpr extent     as_1d() const noexcept;
    constexpr extent     as_2d() const noexcept;
    constexpr uint32_t   linear(const glm::uvec3& p) const noexcept;
    constexpr glm::uvec3 subpixel(uint32_t index) const noexcept;
    constexpr            operator const glm::uvec3&() const noexcept;
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

class image_filter
{
public:
    static std::tuple<image_filter, image_filter, image_filter> gauss_separable(int width, float sigma)
    {
        std::tuple<image_filter, image_filter, image_filter> t{image_filter(extent(width, 1, 1)), image_filter(extent(1, width, 1)),
                                                               image_filter(extent(1, 1, width))};
        const auto                                           val        = glm::gauss(0.f, 0.f, sigma);
        const auto                                           half_width = uint32_t(width >> 1);
        std::get<0>(t)[{half_width, 0, 0}]                              = val;
        std::get<1>(t)[{0, half_width, 0}]                              = val;
        std::get<2>(t)[{0, 0, half_width}]                              = val;
        for (int i = 1; i <= static_cast<int>(half_width); ++i) {
            const auto g                           = glm::gauss(1.f * i, 0.f, sigma);
            std::get<0>(t)[{half_width - i, 0, 0}] = g;
            std::get<1>(t)[{0, half_width - i, 0}] = g;
            std::get<2>(t)[{0, 0, half_width - i}] = g;
            std::get<0>(t)[{half_width + i, 0, 0}] = g;
            std::get<1>(t)[{0, half_width + i, 0}] = g;
            std::get<2>(t)[{0, 0, half_width + i}] = g;
        }
        return t;
    }
    image_filter(extent extents) : _extents(extents), _data(extents.count(), 0.f)
    {
        assert(extents.width % 2 == 1 && extents.height % 2 == 1 && extents.depth % 2 == 1);
    }

    float&       operator[](const glm::uvec3& p) { return _data[_extents.linear(_extents.clamp(p))]; }
    const float& operator[](const glm::uvec3& p) const { return _data[_extents.linear(_extents.clamp(p))]; }

    const auto& extents() const noexcept { return _extents; }
    const auto* data() const noexcept { return _data.data(); }
    auto*       data() noexcept { return _data.data(); }

private:
    extent             _extents;
    std::vector<float> _data;
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
    host_image& operator       =(const host_image& o);
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

    template<typename Fun, typename = decltype(std::declval<Fun>()(std::declval<glm::uvec3>()))>
    void each_pixel(Fun&& f) const
    {
        for (auto i = 0u; i < _extent.count(); ++i) f(_extent.subpixel(i));
    }

    glm::vec4  load(const glm::uvec3& pixel) const;
    glm::vec4  load_bilinear(const glm::vec3& pixel) const;
    glm::uvec4 loadu(const glm::uvec3& pixel) const;
    glm::ivec4 loadi(const glm::uvec3& pixel) const;
    void       store(const glm::uvec3& pixel, const glm::vec4& p);
    void       storeu(const glm::uvec3& pixel, const glm::uvec4& p);
    void       storei(const glm::uvec3& pixel, const glm::ivec4& p);

    using available_types = std::variant<float, int32_t, uint32_t, glm::vec4, glm::ivec4, glm::uvec4, host_image>;
    template<typename T>
    using enable_if_available = decltype(std::get<std::decay_t<T>>(std::declval<available_types>()));

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

    void convolute(const image_filter& f);
    void convolute(const image_filter& f, host_image& into) const;

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
}    // namespace v1
}    // namespace gfx

#include "host_image.inl"