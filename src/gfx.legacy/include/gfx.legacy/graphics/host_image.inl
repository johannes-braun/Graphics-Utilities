#pragma once

namespace gfx {
inline namespace v1 {
constexpr extent::extent(const uint32_t width, const uint32_t height, const uint32_t depth) : width(width), height(height), depth(depth) {}
constexpr extent::extent(const extent extent2d, const uint32_t depth) : width(extent2d.width), height(extent2d.height), depth(depth) {}
constexpr extent::extent(const extent extent1d, const uint32_t height, const uint32_t depth)
      : width(extent1d.width), height(height), depth(depth)
{}

constexpr size_t extent::count() const noexcept
{
    return width * height * depth;
}

constexpr extent extent::as_1d() const noexcept
{
    return {width, height, 1};
}
constexpr extent extent::as_2d() const noexcept
{
    return {width, height, 1};
}

constexpr uint32_t extent::linear(const glm::uvec3& p) const noexcept
{
    return width * height * p.z + width * p.y + p.x;
}

constexpr glm::uvec3 extent::subpixel(uint32_t index) const noexcept
{
    glm::uvec3 result{};
    result.z = index / (width * height);
    index %= (width * height);
    result.y = index / width;
    result.x = index % width;
    return result;
}

constexpr extent::operator const glm::uvec3&() const noexcept
{
    return vec;
}

constexpr glm::ivec3 extent::clamp(const glm::ivec3 pixel) const noexcept
{
    return {std::clamp(pixel.x, 0, int(width) - 1), std::clamp(pixel.y, 0, int(height) - 1), std::clamp(pixel.z, 0, int(depth) - 1)};
}

constexpr glm::ivec3 extent::wrap(const glm::ivec3 pixel) const noexcept
{
    return {(pixel.x + width) % width, (pixel.y + height) % height, (pixel.z + depth) % depth};
}

#pragma warning(push)
#pragma warning(disable : 4244)

template<typename OpFun>
host_image& host_image::operator_apply(host_image& store_target, const host_image& image, OpFun&& fun) const
{
    const std::function<void(const glm::uvec3&)> store_fun = [&]() -> std::function<void(const glm::uvec3&)> {
        if (is_unsigned(_format))
            return [&](const auto& pixel) { store_target.storeu(pixel, fun(loadu(pixel), image.loadu(image.extents().wrap(pixel)))); };
        else if (is_signed(_format))
            return [&](const auto& pixel) { store_target.storei(pixel, fun(loadi(pixel), image.loadi(image.extents().wrap(pixel)))); };
        else
            return [&](const auto& pixel) { store_target.store(pixel, fun(load(pixel), image.load(image.extents().wrap(pixel)))); };
    }();
#pragma omp parallel for schedule(static)
    for (auto p = 0; p < _extent.count(); ++p) {
        const auto pixel = _extent.subpixel(p);
        store_fun(pixel);
    }
    return store_target;
}

template<typename Scalar, typename OpFun, typename>
host_image& host_image::operator_apply(host_image& store_target, Scalar image, OpFun&& fun) const
{
    const std::function<void(const glm::uvec3&)> store_fun = [&]() -> std::function<void(const glm::uvec3&)> {
        if (is_unsigned(_format))
            return [&](const auto& pixel) { store_target.storeu(pixel, fun(loadu(pixel), glm::uvec4(image))); };
        else if (is_signed(_format))
            return [&](const auto& pixel) { store_target.storei(pixel, fun(loadi(pixel), glm::ivec4(image))); };
        else
            return [&](const auto& pixel) { store_target.store(pixel, fun(load(pixel), glm::vec4(image))); };
    }();
#pragma omp parallel for schedule(static)
    for (auto p = 0; p < _extent.count(); ++p) {
        const auto pixel = _extent.subpixel(p);
        store_fun(pixel);
    }
    return store_target;
}

inline const auto op_add = [](const auto& a, const auto& b) { return a + b; };
inline const auto op_sub = [](const auto& a, const auto& b) { return a - b; };
inline const auto op_mul = [](const auto& a, const auto& b) { return a * b; };
inline const auto op_div = [](const auto& a, const auto& b) { return a / b; };

template<typename T, typename>
host_image host_image::operator+(const T& value) const
{
    return operator_nonapply(value, op_add);
}
template<typename T, typename>
host_image host_image::operator-(const T& value) const
{
    return operator_nonapply(value, op_sub);
}
template<typename T, typename>
host_image host_image::operator*(const T& value) const
{
    return operator_nonapply(value, op_mul);
}
template<typename T, typename>
host_image host_image::operator/(const T& value) const
{
    return operator_nonapply(value, op_div);
}
template<typename T, typename>
host_image& host_image::operator+=(const T& value)
{
    return operator_apply(*this, value, op_add);
}
template<typename T, typename>
host_image& host_image::operator-=(const T& value)
{
    return operator_apply(*this, value, op_sub);
}
template<typename T, typename>
host_image& host_image::operator*=(const T& value)
{
    return operator_apply(*this, value, op_mul);
}
template<typename T, typename>
host_image& host_image::operator/=(const T& value)
{
    return operator_apply(*this, value, op_div);
}
#pragma warning(pop)
}    // namespace v1
}    // namespace gfx