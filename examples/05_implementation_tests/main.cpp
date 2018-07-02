#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>

int main()
{
    gfx::context_options opt;
    opt.debug        = true;
    opt.window_title = "odijs";

    // OpenGL 4.3 test
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    auto context     = gfx::context::create(opt);
    context->make_current();

    // Buffer
    gfx::host_buffer<float>   src{1.f, 23.f, 29.f};
    gfx::device_buffer<float> dst(gfx::buffer_usage::storage, src.size());
    dst << src;

    // Images
    const auto        cubemap_format = gfx::r11g11b10f;
    const auto        info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::device_image file_texture(2, cubemap_format, gfx::extent(info.width, info.height, 6), 1);
    file_texture.layer(0) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3));
    file_texture.layer(1) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3));
    file_texture.layer(2) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3));
    file_texture.layer(3) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3));
    file_texture.layer(4) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3));
    file_texture.layer(5) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3));

    // Samplers
    gfx::sampler sampler;
    sampler.set_anisotropy(true, 16.f);
    sampler.set_filter(gfx::filter_mode::min, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);

    mygl::buffer  mgl_buffer  = dst;
    mygl::texture mgl_texture = file_texture;
    mygl::sampler mgl_sampler = sampler;

    while(context->run())
    {
        glFinish();
        glFinish();
    }
}