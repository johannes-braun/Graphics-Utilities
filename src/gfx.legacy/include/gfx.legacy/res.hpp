#pragma once

#include "graphics/device_image.hpp"
#include <gfx.file/file.hpp>
#include <string_view>
#include <unordered_map>

namespace gfx {
inline namespace v1 {
template<typename T>
class res
{
public:
    virtual ~res()                               = default;
    virtual T load(const std::string_view& name) = 0;

    T& operator[](const std::string_view& name)
    {
        if (const auto it = objects.find(name); it != objects.end()) return it->second;
        return objects.emplace(name, std::move(load(name))).first->second;
    }

    res()           = default;
    res(const res&) = delete;
    res& operator=(const res&) = delete;
    res(res&&)                 = delete;
    res& operator=(res&&) = delete;

private:
    std::unordered_map<std::string_view, T> objects;
};

class image_res_ldr : public res<device_image>
{
public:
    device_image load(const std::string_view& name) override
    {
        device_image img(host_image(rgba8unorm, gfx::file(name)));
        img.generate_mipmaps();
        return img;
    }
};

class image_res_hdr : public res<device_image>
{
public:
    device_image load(const std::string_view& name) override
    {
        device_image img(host_image(rgba16f, gfx::file(name)));
        img.generate_mipmaps();
        return img;
    }
};

class image_res_cubemap_hdr : public res<device_image>
{
public:
    device_image load(const std::string_view& name)
    {
        const std::filesystem::path folder = name;
		std::string extension;

		const auto h = gfx::file(folder).path;

		if (exists(h / "posx.hdr"))
			extension = ".hdr";
		else if (exists(h / "posx.png"))
			extension = ".png";

        const auto                  info   = image_file::info(folder / ("posx"+extension));
        const auto                  mips   = static_cast<uint32_t>(floor(log2(std::max(info.width, info.height))) + 1);

        device_image base_cubemap(img_type::image2d, rgba16f, extent(info.width, info.height, 6), mips);
        base_cubemap.layer(0) << host_image(base_cubemap.pixel_format(), folder / ("posx"+extension));
        base_cubemap.layer(1) << host_image(base_cubemap.pixel_format(), folder / ("negx"+extension));
        base_cubemap.layer(2) << host_image(base_cubemap.pixel_format(), folder / ("posy"+extension));
        base_cubemap.layer(3) << host_image(base_cubemap.pixel_format(), folder / ("negy"+extension));
        base_cubemap.layer(4) << host_image(base_cubemap.pixel_format(), folder / ("posz"+extension));
        base_cubemap.layer(5) << host_image(base_cubemap.pixel_format(), folder / ("negz"+extension));
        base_cubemap.generate_mipmaps();

        return base_cubemap;
    }
};

class scene_res : public res<scene_file>
{
public:
    scene_file load(const std::string_view& name) override { return scene_file(name); }
};

class ressources
{
public:
    ressources() = default;

    ressources(const ressources&) = delete;
    ressources& operator=(const ressources&) = delete;
    ressources(ressources&&)                 = delete;
    ressources& operator=(ressources&&) = delete;

    image_res_ldr         images_ldr;
    image_res_hdr         images_hdr;
    image_res_cubemap_hdr cubemaps_hdr;
    scene_res             scenes;
};
}    // namespace v1
}    // namespace gfx