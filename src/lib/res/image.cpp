#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace res
{

    const auto components = [](GLenum format) {
        switch (format)
        {
        case GL_RGBA: return STBI_rgb_alpha;
        case GL_RGB: return STBI_rgb;
        case GL_RG: return STBI_grey_alpha;
        case GL_RED: return STBI_grey;
        default: throw std::invalid_argument("Unsupported format!");
        }
    };

    jpu::ref_ptr<gl::texture> load_texture(const filesystem::path& path, const GLenum internal_format, const GLenum format, const GLenum type)
    {
        if (!filesystem::exists(path))
            throw std::invalid_argument("File not found: " + path.string());
        
        int w, h;
        const auto data = [&]() {
            switch (type)
            {
            case GL_FLOAT: return stbi_data(stbi_loadf(path.string().c_str(), &w, &h, nullptr, components(format)));
            case GL_UNSIGNED_BYTE: return stbi_data(stbi_load(path.string().c_str(), &w, &h, nullptr, components(format)));
            default: throw std::invalid_argument("Unsupported type!");
            }
        }();

        auto tex = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        tex->storage_2d(w, h, internal_format);
        tex->assign_2d(format, type, data.get());
        tex->generate_mipmaps();
        return tex;
    }

    int save_texture(gl::texture* texture, const filesystem::path& path, const GLenum format, const int level, const int jpg_quality)
    {
        int internal_format;
        glGetTextureLevelParameteriv(*texture, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
        int w, h;
        glGetTextureLevelParameteriv(*texture, 0, GL_TEXTURE_WIDTH, &w);
        glGetTextureLevelParameteriv(*texture, 0, GL_TEXTURE_HEIGHT, &h);
        const auto comp = components(format);
        const auto type = path.extension() == ".hdr" ? GL_FLOAT : GL_UNSIGNED_BYTE;
        const uint32_t type_size = type==GL_FLOAT ? sizeof(float) : sizeof(uint8_t);
        const uint32_t data_size = type_size * comp * w * h;

        std::unique_ptr<void, decltype(&free)> texture_data(malloc(data_size), &free);
        glGetTextureImage(*texture, level, format, type, data_size, texture_data.get());
        
        stbi__vertical_flip(texture_data.get(), w, h, type_size * comp);

        const auto extension = path.extension().string();
        if(extension == ".png") return stbi_write_png(path.string().c_str(), w, h, comp, static_cast<const uint8_t*>(texture_data.get()), 0);
        if(extension == ".bmp") return stbi_write_bmp(path.string().c_str(), w, h, comp, static_cast<const uint8_t*>(texture_data.get()));
        if(extension == ".tga") return stbi_write_tga(path.string().c_str(), w, h, comp, static_cast<const uint8_t*>(texture_data.get()));
        if(extension == ".hdr") return stbi_write_hdr(path.string().c_str(), w, h, comp, static_cast<const float*>(texture_data.get()));
        if(extension == ".jpg" || extension == ".jpeg") return stbi_write_jpg(path.string().c_str(), w, h, comp, static_cast<const uint8_t*>(texture_data.get()), jpg_quality);
        throw std::invalid_argument("File extension not supported: " + extension);
    }
}
