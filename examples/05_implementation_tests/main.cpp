#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <iostream>

#include <vulkan/vulkan.hpp>

int main()
{
    gfx::context_options opt;
    opt.graphics_api = gfx::gapi::vulkan;
    opt.debug        = true;
    opt.use_window   = true;

    auto context = gfx::context::create(opt);
    context->make_current();

    //// Buffer
    gfx::hbuffer<float> x{1.f, 23.f, 29.f};
    gfx::hbuffer<float> src = std::move(x);
    gfx::buffer<float>  dst(gfx::buffer_usage::uniform, {2.f, 1.f, 0.5f, 10.f, 1.f, 9.f});
    dst << src;

    gfx::buf_copy(src, dst, dst.capacity());

    for (auto f : src) gfx::ilog << f;

    // Images
    const auto cubemap_format = gfx::rgba8unorm;
    const auto info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::image file_texture(gfx::img_type::image2d, cubemap_format, gfx::extent(info.width, info.height, 6), 11);
    file_texture.layer(0) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3));
    file_texture.layer(1) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3));
    file_texture.layer(2) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3));
    file_texture.layer(3) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3));
    file_texture.layer(4) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3));
    file_texture.layer(5) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3));
    file_texture.generate_mipmaps();

    gfx::image another_image(gfx::himage(gfx::rgba8unorm, gfx::image_file("Lena.png", gfx::bits::b8, 4)));

    auto cubemap = file_texture.view(gfx::imgv_type::image_cube);
    auto texture = another_image.view(gfx::imgv_type::image2d);

    // Samplers
    gfx::sampler sampler;
    sampler.set_anisotropy(true, 16.f);
    sampler.set_filter(gfx::filter_mode::min, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);
    // TODO: for building/creating a sampler it must be casted once.
    VkSampler finished = gfx::handle_cast<VkSampler>(sampler);



    //TODO:
    /*

    - span<binding_set> as pipeline parameters
    - 

     */

    gfx::binding_layout layout1;
    layout1
        .push(gfx::binding_type::uniform_buffer)    // VK: { set: 0, binding: 0 }; GL: { uniform buffer binding: 0 }
        .push(gfx::binding_type::sampled_image)     // VK: { set: 0, binding: 1 }; GL: { texture binding: 0 }
        .push(gfx::binding_type::sampled_image)     // VK: { set: 0, binding: 2 }; GL: { texture binding: 1 }
        .push(gfx::binding_type::storage_image);    // VK: { set: 0, binding: 3 }; GL: { image binding: 0 }
    gfx::binding_layout layout2;
    layout2
        .push(gfx::binding_type::uniform_buffer)     // VK: { set: 1, binding: 0 }; GL: { uniform buffer binding: 1 }
        .push(gfx::binding_type::uniform_buffer)     // VK: { set: 1, binding: 1 }; GL: { uniform buffer binding: 2 }
        .push(gfx::binding_type::storage_buffer);    // VK: { set: 1, binding: 0 }; GL: { uniform buffer binding: 0

    gfx::binding_set set1(layout1);
    set1.bind(0, dst);
    set1.bind(1, cubemap, sampler);
    set1.bind(2, texture, sampler);
    set1.bind(3, texture);

	gfx::binding_set set2(layout2);
    set2.bind(0, dst);
    set2.bind(1, src);
    set2.bind(2, src);

	const auto input = std::make_shared<gfx::vertex_input>();

    while (context->run()) {
    }


    std::cin.ignore();
}