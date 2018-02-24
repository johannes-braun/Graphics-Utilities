#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define NANOSVGRAST_IMPLEMENTATION
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#include <nanosvgrast.h>

namespace res
{
    image load_image(const filesystem::path& path, const image_type type, image_components components)
    {
        image img;
        img.data = type == image_type::signed_float ?
            stbi_data(static_cast<void*>(stbi_loadf(path.string().c_str(), &img.width, &img.height, &img.components, static_cast<int>(components)))) :
            stbi_data(static_cast<void*>(stbi_load(path.string().c_str(), &img.width, &img.height, &img.components, static_cast<int>(components))));
        img.components = std::max(img.components, static_cast<int>(components));
        img.type = type;
        return img;
    }

    image load_svg_rasterized(const filesystem::path& path, const float scale)
    {
        struct rasterizer_deleter
        {
            void operator()(NSVGrasterizer* r) const
            {
                nsvgDeleteRasterizer(r);
            }
        };

        static std::unique_ptr<NSVGrasterizer, rasterizer_deleter> rasterizer{ nsvgCreateRasterizer() };

        NSVGimage* parsed = nsvgParseFromFile(path.string().c_str(), "px", 96);
        
        image image;
        image.data = stbi_data(malloc(scale*scale*parsed->width*parsed->height * 4));
        image.components = 4;
        image.type = image_type::unsigned_byte;
        image.width = scale*parsed->width;
        image.height = scale*parsed->height;
        nsvgRasterize(rasterizer.get(), parsed, 0, 0, scale, static_cast<uint8_t*>(image.data.get()), scale*parsed->width, scale*parsed->height, scale*parsed->width * 4);
        nsvgDelete(parsed);
        return image;
    }
}
