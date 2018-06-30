#include "host_image.hpp"

namespace gfx
{
size_t format_element_size(img_format format)
{
    switch(format)
    {
    case r8unorm:
    case r8snorm:
    case r8u:
    case r8i:
        return 1;
    case rg8unorm:
    case rg8snorm:
    case rg8u:
    case rg8i:
    case r16unorm:
    case r16snorm:
    case r16u:
    case r16i:
    case r16f:
    case r5g6b5unorm:
        return 2;
    case rgb8unorm:
    case rgb8snorm:
    case rgb8u:
    case rgb8i:
        return 3;
    case rgba8unorm:
    case rgba8snorm:
    case rgba8u:
    case rgba8i:
    case rg16unorm:
    case rg16snorm:
    case rg16u:
    case rg16i:
    case rg16f:
    case r32u:
    case r32i:
    case r32f:
    case rgb5a1unorm:
    case rgb10a2snorm:
    case rgb10a2unorm:
    case r11g11b10f:
    case rgb9e5:
        return 4;
    case rgb16unorm:
    case rgb16snorm:
    case rgb16u:
    case rgb16i:
    case rgb16f:
        return 6;
    case rgba16unorm:
    case rgba16snorm:
    case rgba16u:
    case rgba16i:
    case rgba16f:
    case rg32u:
    case rg32i:
    case rg32f:
        return 8;
    case rgb32u:
    case rgb32i:
    case rgb32f:
        return 12;
    case rgba32u:
    case rgba32i:
    case rgba32f:
        return 16;
    }
    return 1;
}


host_image::host_image(img_format format, const extent& size)
        : _format(format)
        , _extent(size)
        , _storage_element_size(format_element_size(format))
        , _storage(_storage_element_size * _extent.count())
        , _write_unorm(get_write_unorm_fun())
{
}

host_image::host_image(img_format format, const image_file& file)
        : host_image(format, {file.width, file.height})
{
    update(file);
}

void host_image::update(const image_file& file)
{
    switch(file.channel_bits)
    {
    case bits::b8:
        update(data_format(file.channels), static_cast<const uint8_t*>(file.bytes()));
        break;
    case bits::b16:
        update(data_format(file.channels), static_cast<const uint16_t*>(file.bytes()));
        break;
    case bits::b32:
        update(data_format(file.channels), static_cast<const float*>(file.bytes()));
        break;
    }
}

const extent& host_image::extents() const noexcept { return _extent; }

glm::vec4 to_vec4(data_format fmt, uint8_t* unorm_data)
{
    switch(fmt)
    {
    case data_format::r:
        return {unorm_data[0] / 255.f, 0, 0, 1};
    case data_format::rg:
        return {unorm_data[0] / 255.f, unorm_data[1] / 255.f, 0, 1};
    case data_format::rgb:
        return {unorm_data[0] / 255.f, unorm_data[1] / 255.f, unorm_data[2] / 255.f, 1};
    case data_format::rgba:
        return {unorm_data[0] / 255.f, unorm_data[1] / 255.f, unorm_data[2] / 255.f, unorm_data[3] / 255.f};
    default:
        return {0, 0, 0, 1};
    }
}

std::function<void(const glm::vec4& v, int64_t i)> host_image::get_write_unorm_fun()
{
    const auto pack_fun = [this](auto fun) {
        return [this, f = fun](const glm::vec4& v, int64_t i) {
            auto x = f(v);
            memcpy(&(_storage.data()[_storage_element_size * i]), &x, sizeof(x));
        };
    };
    const auto pack_funf = [this](auto fun) {
        return [this, f = fun](const glm::vec4& v, int64_t i) {
            auto x = f(v.x);
            memcpy(&(_storage.data()[_storage_element_size * i]), &x, sizeof(x));
        };
    };

    switch(_format)
    {
    case r8unorm:
        return pack_funf(glm::packUnorm1x8);
    case r8snorm:
        return pack_funf(glm::packSnorm1x8);
    case rg8unorm:
        return pack_fun(glm::packUnorm2x8);
    case rg8snorm:
        return pack_fun(glm::packSnorm2x8);
    case rgb8unorm:
        return pack_fun(glm::packUnorm<uint8_t, 3, float, glm::highp>);
    case rgb8snorm:
        return pack_fun(glm::packSnorm<uint8_t, 3, float, glm::highp>);
    case rgba8unorm:
        return pack_fun(glm::packUnorm4x8);
    case rgba8snorm:
        return pack_fun(glm::packSnorm4x8);
    case r5g6b5unorm:
        return pack_fun(glm::packUnorm1x5_1x6_1x5);
    case r16unorm:
        return pack_funf(glm::packUnorm1x16);
    case r16snorm:
        return pack_funf(glm::packSnorm1x16);
    case rg16unorm:
        return pack_fun(glm::packUnorm2x16);
    case rg16snorm:
        return pack_fun(glm::packSnorm2x16);
    case r16f:
        return pack_funf(glm::packHalf1x16);
    case rgb16unorm:
        return pack_fun(glm::packUnorm<uint16_t, 3, float, glm::highp>);
    case rgb16snorm:
        return pack_fun(glm::packSnorm<uint16_t, 3, float, glm::highp>);
    case rgb5a1unorm:
        return pack_fun(glm::packUnorm3x5_1x1);
    case rgb9e5:
        return pack_fun(glm::packF3x9_E1x5);
    case rg16f:
        return pack_fun(glm::packHalf2x16);
    case rgb10a2unorm:
        return pack_fun(glm::packUnorm3x10_1x2);
    case rgb10a2snorm:
        return pack_fun(glm::packSnorm3x10_1x2);
    case r11g11b10f:
        return pack_fun(glm::packF2x11_1x10);
    case rgb16f:
        return pack_fun(glm::packHalf<3, glm::highp>);
    case rgba16unorm:
        return pack_fun(glm::packUnorm4x16);
    case rgba16snorm:
        return pack_fun(glm::packSnorm4x16);
    case rgba16f:
        return pack_fun(glm::packHalf4x16);
    case r32f:
        return pack_fun([](const auto& v) { return glm::vec1(v); });
    case rg32f:
        return pack_fun([](const auto& v) { return glm::vec2(v); });
    case rgb32f:
        return pack_fun([](const auto& v) { return glm::vec3(v); });
    case rgba32f:
        return pack_fun([](const auto& v) { return glm::vec4(v); });
    }
    return pack_fun([](const auto& v) { return glm::vec4(v); });
}

void host_image::update(data_format format, const uint8_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r8u:
    case r8i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg8u:
    case rg8i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb8u:
    case rgb8i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba8u:
    case rgba8i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba8unorm ? std::numeric_limits<data_type>::max()
                                                      : (_format == rgba8snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u16v4,
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
    case rgba32i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const uint16_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r16u:
    case r16i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg16u:
    case rg16i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb16u:
    case rgb16i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba16u:
    case rgba16i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u8v4,
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
    case rgba32i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const uint32_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r32u:
    case r32i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg32u:
    case rg32i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb32u:
    case rgb32i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba32u:
    case rgba32i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u8v4,
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
    case rgba16i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int8_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r8u:
    case r8i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg8u:
    case rg8i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb8u:
    case rgb8i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba8u:
    case rgba8i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba8unorm ? std::numeric_limits<data_type>::max()
                                                      : (_format == rgba8snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r16u:
    case r16i:
    case rg16u:
    case rg16i:
    case rgb16u:
    case rgb16i:
    case rgba16u:
    case rgba16i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u16v4,
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
    case rgba32i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int16_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r16u:
    case r16i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg16u:
    case rg16i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb16u:
    case rgb16i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba16u:
    case rgba16i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u8v4,
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
    case rgba32i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const int32_t* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r32u:
    case r32i:
        switch(format)
        {
        case data_format::r:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rg32u:
    case rg32i:
        switch(format)
        {
        case data_format::rg:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgb32u:
    case rgb32i:
        switch(format)
        {
        case data_format::rgb:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
            break;
        }
        return;

    case rgba32u:
    case rgba32i:
        switch(format)
        {
        case data_format::rgba:
            memcpy(_storage.data(), data, _storage.size());
            break;
        default:
        {
            data_type max_val = _format == rgba16unorm ? std::numeric_limits<data_type>::max()
                                                       : (_format == rgba16snorm ? std::numeric_limits<data_type>::max() : 0);
#pragma omp parallel for schedule(static)
            for(int64_t i = 0; i < _extent.count(); ++i)
            {
                memcpy(&(_storage.data()[_storage_element_size * i]), &(data[data_components * i]), data_size);
                memcpy(&(_storage.data()[_storage_element_size * i + 3 * sizeof(max_val)]), &max_val, sizeof(max_val));
            }
        }
        break;
        }
        return;
    }

    //... or widen to u16 or u32
    switch(_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u8v4,
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
    case rgba16i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u16v4,
                   std::min(_storage_element_size, sizeof(uint16_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c] / static_cast<float>(std::numeric_limits<data_type>::max());
        }

        _write_unorm(unpacked, i);
    }
}
void host_image::update(data_format format, const float* data)
{
    using data_type        = std::decay_t<decltype(data[0])>;
    size_t data_components = static_cast<size_t>(format);
    size_t data_size       = std::min(_storage_element_size, sizeof(data_type) * data_components);

    switch(_format)
    {
    case r8u:
    case r8i:
    case rg8u:
    case rg8i:
    case rgb8u:
    case rgb8i:
    case rgba8u:
    case rgba8i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u8vec4 u8v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u8v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u8v4,
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
    case rgba16i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u16vec4 u16v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u16v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u16v4,
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
    case rgba32i:
    {
#pragma omp parallel for schedule(static)
        for(int64_t i = 0; i < _extent.count(); ++i)
        {
            glm::u32vec4 u32v4{0, 0, 0, 0};
            for(int c = 0; c < data_components; ++c)
                u32v4[c] = data[data_components * i + c];

            memcpy(&(_storage.data()[_storage_element_size * i]),
                   &u32v4,
                   std::min(_storage_element_size, sizeof(uint32_t) * data_components));
        }
    }
        return;
    }

#pragma omp parallel for schedule(static)
    for(int64_t i = 0; i < _extent.count(); ++i)
    {
        glm::vec4 unpacked{0, 0, 0, 1};
        for(int c = 0; c < data_components; ++c)
        {
            unpacked[c] = data[data_components * i + c];
        }

        _write_unorm(unpacked, i);
    }
}

const host_buffer<std::byte>& host_image::storage() const noexcept { return _storage; }
} // namespace gfx