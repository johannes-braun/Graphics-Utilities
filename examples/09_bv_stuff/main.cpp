#include <gfx/gfx.hpp>

enum class distortion_domain
{
    norm_coordinates,
    pixel_coordinates
};

gfx::himage barrel_pillow_distort(const gfx::himage& src, const double factor_pow2, const double factor_pow4,
                                      const distortion_domain domain = distortion_domain::norm_coordinates)
{
    const double k1 = factor_pow2;
    const double k2 = factor_pow4;

    const auto map = [&](const double radius) {
        const auto r2 = radius * radius;
        return 1 + k1 * r2 + k2 * r2 * r2;
    };

    gfx::himage result(src.pixel_format(), src.extents());

    const glm::dvec3 center = static_cast<glm::dvec3>(src.extents().vec) * 0.5;
    #pragma omp parallel for
    for (auto i = 0ll; i < static_cast<long long>(src.extents().count()); ++i) {
        const glm::dvec3 pixel         = src.extents().subpixel(i);
        const double     max_component = domain == distortion_domain::norm_coordinates ? glm::compMax(src.extents().vec) : 1.0;
        const auto       radius        = distance(pixel / max_component, center / max_component);
        const auto       diff_src      = (pixel - center) / map(radius);
        const auto       bl_color      = src.load_bilinear(diff_src + center);
        result.store(pixel, bl_color);
    }
    return result;
}

int main()
{
    gfx::context_options opt;
    opt.use_window     = false;
    const auto context = gfx::context::create(opt);
    context->make_current();

    if (const auto file = gfx::file::open_dialog("Open Image", gfx::file(), {"*.png", "*.jpg", "*.jpeg"}); file) {
        gfx::himage source(gfx::rgba32f, *file);
        gfx::himage destination = barrel_pillow_distort(source, -1, -0.1);
        gfx::himage rgba8       = destination.converted(gfx::rgba8unorm);
        gfx::image_file::save_png("out.png", rgba8.extents().width, rgba8.extents().height, 4,
                                  reinterpret_cast<uint8_t*>(rgba8.storage().data()));
    }
}