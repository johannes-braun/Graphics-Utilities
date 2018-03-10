#pragma once

#include <filesystem>
#include <jpu/memory.hpp>
#include <stb_image.h>

namespace res
{
    namespace filesystem = std::experimental::filesystem;

    struct stbi_free_deleter { void operator()(void* d) const { free(d); } };
    using stbi_data = std::unique_ptr<void, stbi_free_deleter>;

    enum class image_type
    {
        u8 = 0,
        f32
    };

    enum class image_components
    {
        automatic = 0,
        grey,
        grey_alpha,
        rgb,
        rgb_alpha
    };

    struct image
    {
        int num_pixels() const { return width * height; }
        int width{ 0 };
        int height{ 0 };
        int components{ 0 };
        image_type type{ image_type::u8 };
        stbi_data data;
    };

    image load_image(const filesystem::path& path, image_type type, image_components components = image_components::automatic);

    image load_svg_rasterized(const filesystem::path& path, float scale);
}