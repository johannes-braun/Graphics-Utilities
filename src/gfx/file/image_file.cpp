#include <gfx/file.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#define NANOSVG_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "nanosvg.h"
#include "nanosvgrast.h"
#include <algorithm>

namespace gfx
{
    void image_file::save_png(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data)
    {
        stbi_write_png(path.string().c_str(), int(width), int(height), channels, data, 0);
    }

    void image_file::save_bmp(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data)
    {
        stbi_write_bmp(path.string().c_str(), int(width), int(height), channels, data);
    }

    void image_file::save_tga(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data)
    {
        stbi_write_tga(path.string().c_str(), int(width), int(height), channels, data);
    }

    void image_file::save_jpg(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data, int quality)
    {
        stbi_write_jpg(path.string().c_str(), int(width), int(height), channels, data, quality);
    }

    void image_file::save_hdr(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const float* data)
    {
        stbi_write_hdr(path.string().c_str(), int(width), int(height), channels, data);
    }

    image_info image_file::info(const files::path& path) noexcept
    {
        int w, h, c;
        stbi_info(gfx::file(path).path.string().c_str(), &w, &h, &c);
        image_info info;
        info.width      = uint32_t(w);
        info.height     = uint32_t(h);
        info.channels   = uint16_t(c);
        return info;
    }

    image_file::image_file(const files::path& path, bits channel_bits, uint16_t channels)
        : file(path), channel_bits(channel_bits)
    {
        assert(std::clamp<uint16_t>(channels, 1, 4) == channels && "Invalid channel count.");
        struct deleter
        {
            void operator()(void* d) { free(d); }
        };

        stbi_hdr_to_ldr_gamma(2.2f);
        stbi_hdr_to_ldr_scale(1.0f);

        int w, h;
        switch (channel_bits)
        {
        case bits::b8:
        {
            std::unique_ptr<void, deleter> data (stbi_load(file::path.string().c_str(), &w, &h, nullptr, channels));
            uint8_t* ptr = static_cast<uint8_t*>(data.get());
            this->channels = channels;
            this->data = std::vector<uint8_t>(ptr, ptr + w*h*channels);
            _raw = std::get<std::vector<uint8_t>>(this->data).data();
        }
        break;
        case bits::b16:
        {
            std::unique_ptr<void, deleter> data (stbi_load_16(file::path.string().c_str(), &w, &h, nullptr, channels));
            uint16_t* ptr = static_cast<uint16_t*>(data.get());
            this->channels = channels;
            this->data = std::vector<uint16_t>(ptr, ptr + w*h*channels);
            _raw = std::get<std::vector<uint16_t>>(this->data).data();
        }
        break;
        case bits::b32:
        {
            std::unique_ptr<void, deleter> data (stbi_loadf(file::path.string().c_str(), &w, &h, nullptr, channels));
            float* ptr = static_cast<float*>(data.get());
            this->channels = channels;
            this->data = std::vector<float>(ptr, ptr + w*h*channels);
            _raw = std::get<std::vector<float>>(this->data).data();
        }
        break;
        }
        width = uint32_t(w);
        height = uint32_t(h);
    }

    image_file::image_file(image_file&& other) noexcept
    {
        width = other.width;
        height = other.height;
        channels = other.channels;
        channel_bits = other.channel_bits;
        data = std::move(other.data);

        switch (channel_bits)
        {
        case bits::b8:
            _raw = std::get<std::vector<uint8_t>>(this->data).data();
        break;
        case bits::b16:
            _raw = std::get<std::vector<uint16_t>>(this->data).data();
        break;
        case bits::b32:
            _raw = std::get<std::vector<float>>(this->data).data();
        break;
        }
    }

    image_file& image_file::operator=(image_file&& other) noexcept
    {
        width = other.width;
        height = other.height;
        channels = other.channels;
        channel_bits = other.channel_bits;
        data = std::move(other.data);

        switch (channel_bits)
        {
        case bits::b8:
            _raw = std::get<std::vector<uint8_t>>(this->data).data();
            break;
        case bits::b16:
            _raw = std::get<std::vector<uint16_t>>(this->data).data();
            break;
        case bits::b32:
            _raw = std::get<std::vector<float>>(this->data).data();
            break;
        }
        return *this;
    }

    image_file::image_file(const files::path& svg, float raster_scale)
        : file(svg)
    {
        assert(svg.extension() == ".svg");

        struct rasterizer_deleter { void operator()(NSVGrasterizer* r) const {
                nsvgDeleteRasterizer(r);
            }
        };

        static std::unique_ptr<NSVGrasterizer, rasterizer_deleter> rasterizer{ nsvgCreateRasterizer() };

        NSVGimage* parsed = nsvgParseFromFile(file::path.string().c_str(), "px", 96);

        auto&& vec = std::get<std::vector<uint8_t>>(data = std::vector<uint8_t>(size_t(raster_scale*raster_scale*parsed->width*parsed->height * 4)));
        channels = 4;
        channel_bits = bits::b8;
        width = static_cast<int>(raster_scale*parsed->width);
        height = static_cast<int>(raster_scale*parsed->height);
        nsvgRasterize(rasterizer.get(), parsed, 0, 0, raster_scale, static_cast<uint8_t*>(vec.data()),
            static_cast<int>(raster_scale*parsed->width), static_cast<int>(raster_scale*parsed->height), static_cast<int>(raster_scale*parsed->width) * 4);
        nsvgDelete(parsed);

        _raw = vec.data();
    }

    int image_file::pixel_count() const noexcept
    {
        return width * height;
    }

    void* image_file::bytes() const noexcept
    {
        return _raw;
    }
}