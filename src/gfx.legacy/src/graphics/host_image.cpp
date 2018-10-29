#include <graphics/host_image.hpp>

namespace gfx {
inline namespace v1 {
host_image::host_image(const format fmt, const extent& size)
      : _format(fmt)
      , _extent(size)
      , _storage_element_size(format_element_size(fmt))
      , _storage(_storage_element_size * _extent.count())
      , _write_unorm(get_write_unorm_fun())
{}

host_image::host_image(const format format, const image_file& file) : host_image(format, extent{file.width, file.height})
{
    update(file);
}

host_image::host_image(const format fmt, const std::filesystem::path& file)
      : host_image(fmt, image_file(file, bits::b32, image_file::info(file).channels))
{}

host_image::host_image(const host_image& o) : host_image(o.pixel_format(), o.extents())
{
    memcpy(storage().data(), o.storage().data(), o.storage().size());
}

host_image& host_image::operator=(const host_image& o)
{
    _format               = o._format;
    _extent               = o._extent;
    _storage_element_size = format_element_size(_format);
    _storage.resize(_storage_element_size * _extent.count());
    _write_unorm = get_write_unorm_fun();
    memcpy(storage().data(), o.storage().data(), o.storage().size());
    return *this;
}

host_image host_image::converted(const format fmt) const
{
    host_image n(fmt, _extent);
#pragma omp parallel for schedule(static)
    for (int64_t p = 0; p < static_cast<int64_t>(_extent.count()); ++p) {
        const int64_t z = p / (_extent.width * _extent.height);
        const int64_t t = p % (_extent.width * _extent.height);
        const int64_t x = t % _extent.width;
        const int64_t y = t / _extent.width;
        n.store({x, y, z}, load({x, y, z}));
    }
    return n;
}

void host_image::flip_vertically()
{
    const auto fmt_size = _storage_element_size;
    for (int d = 0; d < static_cast<int>(_extent.depth); ++d) {
        for (int x = 0; x < static_cast<int>(_extent.width); ++x) {
            for (int hy = 0; hy < static_cast<int>(_extent.height / 2); ++hy) {
                const int     other   = _extent.height - 1 - hy;
                const int64_t offset1 = fmt_size * _extent.linear({x, hy, d});
                const int64_t offset2 = fmt_size * _extent.linear({x, other, d});

                std::byte buf[32]{};
                memcpy(buf, _storage.data() + offset1, fmt_size);
                memcpy(_storage.data() + offset1, _storage.data() + offset2, fmt_size);
                memcpy(_storage.data() + offset2, buf, fmt_size);
            }
        }
    }
}

void host_image::update(const image_file& file)
{
    switch (file.channel_bits)
    {
    case bits::b8: update(data_format(file.channels), static_cast<const uint8_t*>(file.bytes())); break;
    case bits::b16: update(data_format(file.channels), static_cast<const uint16_t*>(file.bytes())); break;
    case bits::b32: update(data_format(file.channels), static_cast<const float*>(file.bytes())); break;
    }
}

const extent& host_image::extents() const noexcept
{
    return _extent;
}

uint32_t host_image::max_levels() const noexcept
{
    return 1u + static_cast<uint32_t>(floor(log2(std::max(_extent.width, std::max(_extent.height, _extent.depth)))));
}

format host_image::pixel_format() const noexcept
{
    return _format;
}

glm::vec4 host_image::load_bilinear(const glm::vec3& pixel) const
{
    const auto       frac = fract(pixel);
    const glm::ivec3 p00  = _extent.clamp(floor(pixel));
    const glm::ivec3 p11  = _extent.clamp(ceil(pixel));

    const auto c000 = load(p00);
    const auto c001 = load({p00.x, p00.y, p11.z});
    const auto c010 = load({p00.x, p11.y, p00.z});
    const auto c011 = load({p00.x, p11.y, p11.z});
    const auto c100 = load({p11.x, p00.y, p00.z});
    const auto c101 = load({p11.x, p00.y, p11.z});
    const auto c110 = load({p11.x, p11.y, p00.z});
    const auto c111 = load(p11);

    const auto m00z = mix(c000, c001, frac.z);
    const auto m01z = mix(c010, c011, frac.z);
    const auto m10z = mix(c100, c101, frac.z);
    const auto m11z = mix(c110, c111, frac.z);

    const auto m0yz = mix(m00z, m01z, frac.y);
    const auto m1yz = mix(m10z, m11z, frac.y);

    return mix(m0yz, m1yz, frac.x);
}

glm::vec4 host_image::load(glm::uvec3 pixel) const
{
	pixel = _extent.clamp(pixel);
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;

    switch (_format)
    {
    case r8unorm: return glm::vec4(*reinterpret_cast<const uint8_t*>(&_storage[index]), 0, 0, 255) / 255.f;
    case r8snorm: return glm::vec4(*reinterpret_cast<const int8_t*>(&_storage[index]), 0, 0, 255) / 255.f;
    case rg8unorm: return glm::vec4(*reinterpret_cast<const glm::u8vec2*>(&_storage[index]), 0, 255) / 255.f;
    case rg8snorm: return glm::vec4(*reinterpret_cast<const glm::i8vec2*>(&_storage[index]), 0, 255) / 255.f;
    case rgb8unorm: return glm::vec4(*reinterpret_cast<const glm::u8vec3*>(&_storage[index]), 255) / 255.f;
    case rgb8snorm: return glm::vec4(*reinterpret_cast<const glm::i8vec3*>(&_storage[index]), 255) / 255.f;
    case rgba8unorm: return glm::vec4(*reinterpret_cast<const glm::u8vec4*>(&_storage[index])) / 255.f;
    case rgba8snorm: return glm::vec4(*reinterpret_cast<const glm::i8vec4*>(&_storage[index])) / 255.f;
	case bgra8unorm: return [&]() {
		glm::vec4 rgba = glm::vec4(*reinterpret_cast<const glm::i8vec4*>(&_storage[index])) / 255.f;
		return glm::vec4(rgba.b, rgba.g, rgba.r, rgba.a);
	}();
	case bgr8unorm: return [&]() {
		glm::vec4 rgba = glm::vec4(*reinterpret_cast<const glm::i8vec3*>(&_storage[index]), 255.f) / 255.f;
		return glm::vec4(rgba.b, rgba.g, rgba.r, rgba.a);
	}();
    case r16unorm:
        return glm::vec4(*reinterpret_cast<const uint16_t*>(&_storage[index]), 0, 0, std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case r16snorm:
        return glm::vec4(*reinterpret_cast<const int16_t*>(&_storage[index]), 0, 0, std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case rg16unorm:
        return glm::vec4(*reinterpret_cast<const glm::u16vec2*>(&_storage[index]), 0, std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case rg16snorm:
        return glm::vec4(*reinterpret_cast<const glm::i16vec2*>(&_storage[index]), 0, std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case rgb16unorm:
        return glm::vec4(*reinterpret_cast<const glm::u16vec3*>(&_storage[index]), std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case rgb16snorm:
        return glm::vec4(*reinterpret_cast<const glm::i16vec3*>(&_storage[index]), std::numeric_limits<uint16_t>::max())
               / float(std::numeric_limits<uint16_t>::max());
    case rgba16unorm:
        return glm::vec4(*reinterpret_cast<const glm::u16vec4*>(&_storage[index])) / float(std::numeric_limits<uint16_t>::max());
    case rgba16snorm:
        return glm::vec4(*reinterpret_cast<const glm::i16vec4*>(&_storage[index])) / float(std::numeric_limits<uint16_t>::max());

    case r5g6b5unorm: return glm::vec4(glm::unpackUnorm1x5_1x6_1x5(*reinterpret_cast<const uint16_t*>(&_storage[index])), 1);
    case rgb5a1unorm: return glm::vec4(glm::unpackUnorm3x5_1x1(*reinterpret_cast<const uint16_t*>(&_storage[index])));

    case rgb10a2snorm: return glm::vec4(glm::unpackSnorm3x10_1x2(*reinterpret_cast<const uint32_t*>(&_storage[index])));
    case rgb10a2unorm: return glm::vec4(glm::unpackUnorm3x10_1x2(*reinterpret_cast<const uint32_t*>(&_storage[index])));

    case r11g11b10f: return glm::vec4(glm::unpackF2x11_1x10(*reinterpret_cast<const uint32_t*>(&_storage[index])), 1.f);
    case rgb9e5: return glm::vec4(glm::unpackF3x9_E1x5(*reinterpret_cast<const uint32_t*>(&_storage[index])), 1.f);

    case r16f: return glm::vec4(glm::unpackHalf1x16(*reinterpret_cast<const uint16_t*>(&_storage[index])), 0, 0, 1.f);
    case rg16f: return glm::vec4(glm::unpackHalf2x16(*reinterpret_cast<const uint32_t*>(&_storage[index])), 0, 1.f);
    case rgb16f: return glm::vec4(glm::unpackHalf<3, glm::defaultp>(*reinterpret_cast<const glm::u16vec3*>(&_storage[index])), 1.f);
    case rgba16f: return glm::vec4(glm::unpackHalf4x16(*reinterpret_cast<const uint64_t*>(&_storage[index])));

    case r32f: return glm::vec4(*reinterpret_cast<const float*>(&_storage[index]), 0, 0, 1);
    case rg32f: return glm::vec4(*reinterpret_cast<const glm::vec2*>(&_storage[index]), 0, 1);
    case rgb32f: return glm::vec4(*reinterpret_cast<const glm::vec3*>(&_storage[index]), 1);
    case rgba32f: return *reinterpret_cast<const glm::vec4*>(&_storage[index]);

    default: throw std::invalid_argument("Cannot call load4f on a non-normalized or non-floating-point image.");
    }
}

glm::uvec4 host_image::loadu(const glm::uvec3& pixel) const
{
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;
    switch (_format)
    {
    case r8u: return glm::uvec4(*reinterpret_cast<const uint8_t*>(&_storage[index]), 0, 0, 0);
    case rg8u: return glm::uvec4(*reinterpret_cast<const glm::u8vec2*>(&_storage[index]), 0, 0);
    case rgb8u: return glm::uvec4(*reinterpret_cast<const glm::u8vec3*>(&_storage[index]), 0);
    case rgba8u: return glm::uvec4(*reinterpret_cast<const glm::u8vec4*>(&_storage[index]));

    case r16u: return glm::uvec4(*reinterpret_cast<const uint16_t*>(&_storage[index]), 0, 0, 0);
    case rg16u: return glm::uvec4(*reinterpret_cast<const glm::u16vec2*>(&_storage[index]), 0, 0);
    case rgb16u: return glm::uvec4(*reinterpret_cast<const glm::u16vec3*>(&_storage[index]), 0);
    case rgba16u: return glm::uvec4(*reinterpret_cast<const glm::u16vec4*>(&_storage[index]));

    case r32u: return glm::uvec4(*reinterpret_cast<const uint32_t*>(&_storage[index]), 0, 0, 0);
    case rg32u: return glm::uvec4(*reinterpret_cast<const glm::u32vec2*>(&_storage[index]), 0, 0);
    case rgb32u: return glm::uvec4(*reinterpret_cast<const glm::u32vec3*>(&_storage[index]), 0);
    case rgba32u: return glm::uvec4(*reinterpret_cast<const glm::u32vec4*>(&_storage[index]));

    default: throw std::invalid_argument("Cannot call load4u on an image which has no uint type.");
    }
}

glm::ivec4 host_image::loadi(const glm::uvec3& pixel) const
{
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;
    switch (_format)
    {
    case r8i: return glm::ivec4(*reinterpret_cast<const int8_t*>(&_storage[index]), 0, 0, 0);
    case rg8i: return glm::ivec4(*reinterpret_cast<const glm::i8vec2*>(&_storage[index]), 0, 0);
    case rgb8i: return glm::ivec4(*reinterpret_cast<const glm::i8vec3*>(&_storage[index]), 0);
    case rgba8i: return glm::ivec4(*reinterpret_cast<const glm::i8vec4*>(&_storage[index]));

    case r16i: return glm::ivec4(*reinterpret_cast<const int16_t*>(&_storage[index]), 0, 0, 0);
    case rg16i: return glm::ivec4(*reinterpret_cast<const glm::i16vec2*>(&_storage[index]), 0, 0);
    case rgb16i: return glm::ivec4(*reinterpret_cast<const glm::i16vec3*>(&_storage[index]), 0);
    case rgba16i: return glm::ivec4(*reinterpret_cast<const glm::i16vec4*>(&_storage[index]));

    case r32i: return glm::ivec4(*reinterpret_cast<const int32_t*>(&_storage[index]), 0, 0, 0);
    case rg32i: return glm::ivec4(*reinterpret_cast<const glm::i32vec2*>(&_storage[index]), 0, 0);
    case rgb32i: return glm::ivec4(*reinterpret_cast<const glm::i32vec3*>(&_storage[index]), 0);
    case rgba32i: return glm::ivec4(*reinterpret_cast<const glm::i32vec4*>(&_storage[index]));

    default: throw std::invalid_argument("Cannot call load4u on an image which has no uint type.");
    }
}

template<typename T, glm::length_t L>
auto cnorm(const glm::vec4& p)
{
    using vec_type      = glm::vec<L, T, glm::highp>;
    constexpr float min = std::is_signed_v<T> ? -1.f : 0.f;
    if constexpr (L == 1)
        return T(glm::clamp(p.x, min, 1.f) * float(std::numeric_limits<T>::max()));
    else
        return vec_type(glm::clamp(p, min, 1.f) * float(std::numeric_limits<T>::max()));
}

void host_image::store(const glm::uvec3& pixel, const glm::vec4& p)
{
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;
    switch (_format)
    {
    case r8unorm: *reinterpret_cast<uint8_t*>(&_storage[index]) = cnorm<uint8_t, 1>(p); break;
    case r8snorm: *reinterpret_cast<int8_t*>(&_storage[index]) = cnorm<int8_t, 1>(p); break;
    case rg8unorm: *reinterpret_cast<glm::u8vec2*>(&_storage[index]) = cnorm<uint8_t, 2>(p); break;
    case rg8snorm: *reinterpret_cast<glm::i8vec2*>(&_storage[index]) = cnorm<int8_t, 2>(p); break;
    case rgb8unorm: *reinterpret_cast<glm::u8vec3*>(&_storage[index]) = cnorm<uint8_t, 3>(p); break;
    case rgb8snorm: *reinterpret_cast<glm::u8vec3*>(&_storage[index]) = cnorm<int8_t, 3>(p); break;
    case rgba8unorm: *reinterpret_cast<glm::u8vec4*>(&_storage[index]) = cnorm<uint8_t, 4>(p); break;
    case rgba8snorm: *reinterpret_cast<glm::i8vec4*>(&_storage[index]) = cnorm<int8_t, 4>(p); break;
	
	case bgr8unorm: [&] {
		const auto cnormed = cnorm<int8_t, 3>(p);
		*reinterpret_cast<glm::i8vec3*>(&_storage[index]) ={ cnormed.b, cnormed.g, cnormed.r };
	}();
	case bgra8unorm: [&] {
		const auto cnormed = cnorm<int8_t, 4>(p);
		*reinterpret_cast<glm::i8vec4*>(&_storage[index]) ={ cnormed.b, cnormed.g, cnormed.r, cnormed.a };
	}();

    case r16unorm: *reinterpret_cast<uint16_t*>(&_storage[index]) = cnorm<uint16_t, 1>(p); break;
    case r16snorm: *reinterpret_cast<int16_t*>(&_storage[index]) = cnorm<int16_t, 1>(p); break;
    case rg16unorm: *reinterpret_cast<glm::u16vec2*>(&_storage[index]) = cnorm<uint16_t, 2>(p); break;
    case rg16snorm: *reinterpret_cast<glm::i16vec2*>(&_storage[index]) = cnorm<int16_t, 2>(p); break;
    case rgb16unorm: *reinterpret_cast<glm::u16vec3*>(&_storage[index]) = cnorm<uint16_t, 3>(p); break;
    case rgb16snorm: *reinterpret_cast<glm::u16vec3*>(&_storage[index]) = cnorm<int16_t, 3>(p); break;
    case rgba16unorm: *reinterpret_cast<glm::u16vec4*>(&_storage[index]) = cnorm<uint16_t, 4>(p); break;
    case rgba16snorm: *reinterpret_cast<glm::i16vec4*>(&_storage[index]) = cnorm<int16_t, 4>(p); break;

    case r5g6b5unorm: *reinterpret_cast<uint16_t*>(&_storage[index]) = packUnorm1x5_1x6_1x5(p); break;
    case rgb5a1unorm: *reinterpret_cast<uint16_t*>(&_storage[index]) = packUnorm3x5_1x1(p); break;

    case rgb10a2snorm: *reinterpret_cast<uint32_t*>(&_storage[index]) = packSnorm3x10_1x2(p); break;
    case rgb10a2unorm: *reinterpret_cast<uint32_t*>(&_storage[index]) = packUnorm3x10_1x2(p); break;

    case r11g11b10f: *reinterpret_cast<uint32_t*>(&_storage[index]) = packF2x11_1x10(p); break;
    case rgb9e5: *reinterpret_cast<uint32_t*>(&_storage[index]) = packF3x9_E1x5(p); break;

    case r16f: *reinterpret_cast<uint16_t*>(&_storage[index]) = glm::packHalf1x16(p.x); break;
    case rg16f: *reinterpret_cast<uint32_t*>(&_storage[index]) = packHalf2x16(p); break;
    case rgb16f: *reinterpret_cast<glm::u16vec3*>(&_storage[index]) = glm::packHalf<3, glm::defaultp>(p); break;
    case rgba16f: *reinterpret_cast<uint64_t*>(&_storage[index]) = packHalf4x16(p); break;

    case r32f: *reinterpret_cast<float*>(&_storage[index]) = p.x; break;
    case rg32f: *reinterpret_cast<glm::vec2*>(&_storage[index]) = p; break;
    case rgb32f: *reinterpret_cast<glm::vec3*>(&_storage[index]) = p; break;
    case rgba32f: *reinterpret_cast<glm::vec4*>(&_storage[index]) = p; break;

    default: throw std::invalid_argument("Cannot call storef on a non-normalized or non-floating-point image.");
    }
}

void host_image::storeu(const glm::uvec3& pixel, const glm::uvec4& p)
{
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;
    switch (_format)
    {
    case r8u: *reinterpret_cast<glm::u8vec1*>(&_storage[index]) = p; break;
    case rg8u: *reinterpret_cast<glm::u8vec2*>(&_storage[index]) = p; break;
    case rgb8u: *reinterpret_cast<glm::u8vec3*>(&_storage[index]) = p; break;
    case rgba8u: *reinterpret_cast<glm::u8vec4*>(&_storage[index]) = p; break;

    case r16u: *reinterpret_cast<glm::u16vec1*>(&_storage[index]) = p; break;
    case rg16u: *reinterpret_cast<glm::u16vec2*>(&_storage[index]) = p; break;
    case rgb16u: *reinterpret_cast<glm::u16vec3*>(&_storage[index]) = p; break;
    case rgba16u: *reinterpret_cast<glm::u16vec4*>(&_storage[index]) = p; break;

    case r32u: *reinterpret_cast<glm::u32vec1*>(&_storage[index]) = p; break;
    case rg32u: *reinterpret_cast<glm::u32vec2*>(&_storage[index]) = p; break;
    case rgb32u: *reinterpret_cast<glm::u32vec3*>(&_storage[index]) = p; break;
    case rgba32u: *reinterpret_cast<glm::u32vec4*>(&_storage[index]) = p; break;

    default: throw std::invalid_argument("Cannot call storeu on an image which has no uint type.");
    }
}

void host_image::storei(const glm::uvec3& pixel, const glm::ivec4& p)
{
    const size_t index = (_extent.width * _extent.height * pixel.z + _extent.width * pixel.y + pixel.x) * _storage_element_size;
    switch (_format)
    {
    case r8i: *reinterpret_cast<glm::i8vec1*>(&_storage[index]) = p; break;
    case rg8i: *reinterpret_cast<glm::i8vec2*>(&_storage[index]) = p; break;
    case rgb8i: *reinterpret_cast<glm::i8vec3*>(&_storage[index]) = p; break;
    case rgba8i: *reinterpret_cast<glm::i8vec4*>(&_storage[index]) = p; break;

    case r16i: *reinterpret_cast<glm::i16vec1*>(&_storage[index]) = p; break;
    case rg16i: *reinterpret_cast<glm::i16vec2*>(&_storage[index]) = p; break;
    case rgb16i: *reinterpret_cast<glm::i16vec3*>(&_storage[index]) = p; break;
    case rgba16i: *reinterpret_cast<glm::i16vec4*>(&_storage[index]) = p; break;

    case r32i: *reinterpret_cast<glm::i32vec1*>(&_storage[index]) = p; break;
    case rg32i: *reinterpret_cast<glm::i32vec2*>(&_storage[index]) = p; break;
    case rgb32i: *reinterpret_cast<glm::i32vec3*>(&_storage[index]) = p; break;
    case rgba32i: *reinterpret_cast<glm::i32vec4*>(&_storage[index]) = p; break;

    default: throw std::invalid_argument("Cannot call storeu on an image which has no uint type.");
    }
}

bool is_unorm_compatible(const format fmt)
{
    switch (fmt)
    {
    case r8unorm:
    case r8snorm:
    case rg8unorm:
    case rg8snorm:
    case rgb8unorm:
    case rgb8snorm:
    case rgba8unorm:
    case rgba8snorm:
    case r5g6b5unorm:
    case r16unorm:
    case r16snorm:
    case rg16unorm:
    case rg16snorm:
    case r16f:
    case rgb16unorm:
    case rgb16snorm:
    case rgb5a1unorm:
    case rgb9e5:
    case rg16f:
    case rgb10a2unorm:
    case rgb10a2snorm:
    case r11g11b10f:
    case rgb16f:
    case rgba16unorm:
    case rgba16snorm:
    case rgba16f:
    case r32f:
    case rg32f:
    case rgb32f:
    case rgba32f: return true;
    default: break;
    }
    return false;
}

bool is_unsigned(const format fmt)
{
    switch (fmt)
    {
    case r8u:
    case rg8u:
    case rgb8u:
    case rgba8u:
    case r16u:
    case rg16u:
    case rgb16u:
    case rgba16u:
    case r32u:
    case rg32u:
    case rgb32u:
    case rgba32u: return true;
    default: break;
    }
    return false;
}
bool is_signed(const format fmt)
{
    switch (fmt)
    {
    case r8i:
    case rg8i:
    case rgb8i:
    case rgba8i:
    case r16i:
    case rg16i:
    case rgb16i:
    case rgba16i:
    case r32i:
    case rg32i:
    case rgb32i:
    case rgba32i: return true;
    default: break;
    }
    return false;
}

void host_image::convolute(const image_filter& f)
{
    gfx::host_image tmp(pixel_format(), extents());
    convolute(f, tmp);
    memcpy(storage().data(), tmp.storage().data(), storage().size());
}

void host_image::convolute(const image_filter& f, host_image& into) const
{
    assert(is_unorm_compatible(_format));

    const auto half_size = f.extents().vec / 2u;

#pragma omp parallel for
    for (int p = 0; p < extents().count(); ++p) {
        const glm::uvec3 pixel = extents().subpixel(p);

        glm::vec4 color{0};
        for (auto i = 0u; i < f.extents().count(); ++i) {
            const auto filter_pixel = f.extents().subpixel(i);
            const auto cl           = extents().clamp(pixel + filter_pixel - half_size);

            color += load(cl) * f[filter_pixel];
        }
        into.store(pixel, color);
    }
}

bool host_image::operator==(const host_image& image) const
{
    return _format == image._format && _extent == image._extent && storage().size() == image.storage().size()
           && memcmp(storage().data(), image.storage().data(), storage().size()) == 0;
}

bool host_image::operator!=(const host_image& image) const
{
    return !(*this == image);
}

std::function<void(const glm::vec4& v, int64_t i)> host_image::get_write_unorm_fun()
{
    const auto pack_fun = [this](auto fun) {
        return [ this, f = fun ](const glm::vec4& v, const int64_t i)
        {
            auto x = f(v);
            memcpy(&(_storage.data()[_storage_element_size * i]), &x, sizeof(x));
        };
    };
    const auto pack_funf = [this](auto fun) {
        return [ this, f = fun ](const glm::vec4& v, const int64_t i)
        {
            auto x = f(v.x);
            memcpy(&(_storage.data()[_storage_element_size * i]), &x, sizeof(x));
        };
    };

    switch (_format)
    {
    case r8unorm: return pack_funf(glm::packUnorm1x8);
    case r8snorm: return pack_funf(glm::packSnorm1x8);
    case rg8unorm: return pack_fun(glm::packUnorm2x8);
    case rg8snorm: return pack_fun(glm::packSnorm2x8);
    case rgb8unorm: return pack_fun(glm::packUnorm<uint8_t, 3, float, glm::highp>);
    case rgb8snorm: return pack_fun(glm::packSnorm<uint8_t, 3, float, glm::highp>);
    case rgba8unorm: return pack_fun(glm::packUnorm4x8);
    case rgba8snorm: return pack_fun(glm::packSnorm4x8);
    case r5g6b5unorm: return pack_fun(glm::packUnorm1x5_1x6_1x5);
    case r16unorm: return pack_funf(glm::packUnorm1x16);
    case r16snorm: return pack_funf(glm::packSnorm1x16);
    case rg16unorm: return pack_fun(glm::packUnorm2x16);
    case rg16snorm: return pack_fun(glm::packSnorm2x16);
    case r16f: return pack_funf(glm::packHalf1x16);
    case rgb16unorm: return pack_fun(glm::packUnorm<uint16_t, 3, float, glm::highp>);
    case rgb16snorm: return pack_fun(glm::packSnorm<uint16_t, 3, float, glm::highp>);
    case rgb5a1unorm: return pack_fun(glm::packUnorm3x5_1x1);
    case rgb9e5: return pack_fun(glm::packF3x9_E1x5);
    case rg16f: return pack_fun(glm::packHalf2x16);
    case rgb10a2unorm: return pack_fun(glm::packUnorm3x10_1x2);
    case rgb10a2snorm: return pack_fun(glm::packSnorm3x10_1x2);
    case r11g11b10f: return pack_fun(glm::packF2x11_1x10);
    case rgb16f: return pack_fun(glm::packHalf<3, glm::highp>);
    case rgba16unorm: return pack_fun(glm::packUnorm4x16);
    case rgba16snorm: return pack_fun(glm::packSnorm4x16);
    case rgba16f: return pack_fun(glm::packHalf4x16);
    case r32f: return pack_fun([](const auto& v) { return glm::vec1(v); });
    case rg32f: return pack_fun([](const auto& v) { return glm::vec2(v); });
    case rgb32f: return pack_fun([](const auto& v) { return glm::vec3(v); });
    case rgba32f: return pack_fun([](const auto& v) { return glm::vec4(v); });
    default: break;
    }
    return pack_fun([](const auto& v) { return glm::vec4(v); });
}

void host_image::update(data_format format, const uint8_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const auto   data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r8u:
    case r8i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg8u:
    case rg8i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb8u:
    case rgb8i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba8u:
    case rgba8i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba8unorm ? std::numeric_limits<data_type>::max()
                                                      : (_format == rgba8snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    case r32u:
    case r32i:
    case rg32u:
    case rg32i:
    case rgb32u:
    case rgb32i:
    case rgba32u:
    case rgba32i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const uint16_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r16u:
    case r16i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg16u:
    case rg16i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb16u:
    case rgb16i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba16u:
    case rgba16i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u8v4[c] = static_cast<uint8_t>(data[data_components * i + c]);

            memcpy(&(_storage.data()[_storage_element_size * i]), &u8v4,
                   std::min(_storage_element_size, sizeof(uint8_t) * data_components));
        }
    }
        return;
    case r32u:
    case r32i:
    case rg32u:
    case rg32i:
    case rgb32u:
    case rgb32i:
    case rgba32u:
    case rgba32i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const uint32_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r32u:
    case r32i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg32u:
    case rg32i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb32u:
    case rgb32i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba32u:
    case rgba32i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u8v4,
                   std::min(_storage_element_size, sizeof(uint8_t) * data_components));
        }
    }
        return;
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int8_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r8u:
    case r8i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg8u:
    case rg8i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb8u:
    case rgb8i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba8u:
    case rgba8i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba8unorm ? std::numeric_limits<data_type>::max()
                                                      : (_format == rgba8snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    case r32u:
    case r32i:
    case rg32u:
    case rg32i:
    case rgb32u:
    case rgb32i:
    case rgba32u:
    case rgba32i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int16_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r16u:
    case r16i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg16u:
    case rg16i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb16u:
    case rgb16i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba16u:
    case rgba16i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u8v4[c] = static_cast<uint8_t>(data[data_components * i + c]);

            memcpy(&(_storage.data()[_storage_element_size * i]), &u8v4,
                   std::min(_storage_element_size, sizeof(uint8_t) * data_components));
        }
    }
        return;
    case r32u:
    case r32i:
    case rg32u:
    case rg32i:
    case rgb32u:
    case rgb32i:
    case rgba32u:
    case rgba32i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int32_t* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);
    const size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch (_format)
    {
    case r32u:
    case r32i:
        switch (format)
        {
        case data_format::r: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg32u:
    case rg32i:
        switch (format)
        {
        case data_format::rg: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb32u:
    case rgb32i:
        switch (format)
        {
        case data_format::rgb: memcpy(_storage.data(), data, _storage.size()); break;
        default:
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba32u:
    case rgba32i:
        switch (format)
        {
        case data_format::rgba: memcpy(_storage.data(), data, _storage.size()); break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    default: break;
    }

    //... or widen to u16 or u32
    switch (_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u8v4,
                   std::min(_storage_element_size, sizeof(uint8_t) * data_components));
        }
    }
        return;
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]), &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const float* data)
{
    using data_type              = std::decay_t<decltype(data[0])>;
    const size_t data_components = static_cast<size_t>(format);

    switch (_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u8v4[c] = static_cast<uint8_t>(data[data_components * i + c]);

            memcpy(&(_storage.data()[_storage_element_size * i]), &u8v4,
                   std::min(_storage_element_size, sizeof(uint8_t) * data_components));
        }
    }
        return;
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u16v4[c] = static_cast<uint16_t>(data[data_components * i + c]);

            memcpy(&(_storage.data()[_storage_element_size * i]), &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    case r32u:
    case r32i:
    case rg32u:
    case rg32i:
    case rgb32u:
    case rgb32i:
    case rgba32u:
    case rgba32i: {
#pragma omp parallel for schedule(static)
        for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for (int c = 0; c < data_components; ++c) u32v4[c] = static_cast<uint32_t>(data[data_components * i + c]);

            memcpy(&(_storage.data()[_storage_element_size * i]), &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    default: break;
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < static_cast<int64_t>(_extent.count()); ++i) {
        glm::vec4 unpacked{0, 0, 0, 1};
        for (int c = 0; c < data_components; ++c) {
            unpacked[c] = data[data_components * i + c];
        }

        _write_unorm(unpacked, i);
    }
}

const host_buffer<std::byte>& host_image::storage() const noexcept
{
    return _storage;
}
host_buffer<std::byte>& host_image::storage() noexcept
{
    return _storage;
}
}    // namespace v1
}    // namespace gfx