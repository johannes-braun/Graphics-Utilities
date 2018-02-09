#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace res
{
    image load_image(const filesystem::path& path, image_type type, image_components components)
    {
        image img;
        img.data = type == image_type::signed_float ?
            stbi_data(static_cast<void*>(stbi_loadf(path.string().c_str(), &img.width, &img.height, &img.components, static_cast<int>(components)))) :
            stbi_data(static_cast<void*>(stbi_load(path.string().c_str(), &img.width, &img.height, &img.components, static_cast<int>(components))));
        img.components = std::max(img.components, static_cast<int>(components));
        img.type = type;
        return img;
    }
}
