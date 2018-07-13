#include <gfx/gfx.hpp>

void gauss_blur(gfx::host_image& image, int kernel_radius, float sigma)
{
    gfx::host_image img2(image.pixel_format(), image.extents());

    std::vector<float> kernel(2 * kernel_radius + 1);
    kernel[kernel_radius + 1] = glm::gauss(0.f, 0.f, sigma);
    //#pragma omp parallel for schedule(static)
    for (int i = 1; i < kernel_radius; ++i) {
        kernel[kernel_radius + 1 - i] = glm::gauss(i * 1.f, 0.f, sigma);
        kernel[kernel_radius + 1 + i] = glm::gauss(i * 1.f, 0.f, sigma);
    }
#pragma omp parallel for schedule(static)
    for (int x = 0; x < image.extents().width; ++x)
        for (int y = 0; y < image.extents().height; ++y) {
            glm::vec4 color{0};
            for (int i = -int(kernel.size() >> 1); i <= int(kernel.size() >> 1); ++i)
                color += kernel[i + (kernel.size() >> 1)] * image.load({glm::clamp<int>(x + i, 0, image.extents().width - 1), y, 0});
            img2.store({x, y, 0}, color);
        }
#pragma omp parallel for schedule(static)
    for (int x = 0; x < img2.extents().width; ++x)
        for (int y = 0; y < img2.extents().height; ++y) {
            glm::vec4 color{0};
            for (int i = -int(kernel.size() >> 1); i <= int(kernel.size() >> 1); ++i)
                color += kernel[i + (kernel.size() >> 1)] * img2.load({x, glm::clamp<int>(y + i, 0, img2.extents().height - 1), 0});
            image.store({x, y, 0}, color);
        }
}

std::unordered_set<glm::uvec2> tomasi_kanade(const gfx::host_image& image, double threshold)
{
    std::unordered_set<glm::uvec2> points;
    constexpr int                  rd = 1;
    for (int x = rd; x < image.extents().width - rd; ++x)
        for (int y = rd; y < image.extents().height - rd; ++y) {
            glm::dvec4 lum = glm::dvec4{0.299f, 0.587f, 0.114f, 0.f};

			//const glm::dvec4 d_m1_m1 = image.load({x - rd, y - rd, 0});
   //         const glm::dvec4 d_0_m1 = image.load({x, y - rd, 0});
   //         const glm::dvec4 d_p1_m1 = image.load({x + rd, y - rd, 0});

   //         const glm::dvec4 d_m1_0 = image.load({x - rd, y, 0});
   //         const glm::dvec4 d_0_0 = image.load({x, y, 0});
   //         const glm::dvec4 d_p1_0 = image.load({x + rd, y, 0});

   //         const glm::dvec4 d_m1_p1 = image.load({x - rd, y + rd, 0});
   //         const glm::dvec4 d_0_p1  = image.load({x, y + rd, 0});
   //         const glm::dvec4 d_p1_p1 = image.load({x + rd, y + rd, 0});


			//const auto df_xx = pow2(d_m1_0 - d_0_0) + pow2(d_0_0 - d_p1_0);
   //         const auto df_yy = pow2(d_0_m1 - d_0_0) + pow2(d_0_0 - d_0_p1);
   //         const auto df_xy = pow2(d_m1_m1 - d_0_0) + pow2(d_0_0 - d_p1_p1);
   //         const auto df_yx = pow2(d_p1_m1 - d_0_0) + pow2(d_0_0 - d_m1_p1);


            const auto vn00 = glm::dvec4(image.load({x - rd, y - rd, 0}));
            const auto vn01 = glm::dvec4(image.load({x - rd, y, 0}));
            const auto vn10 = glm::dvec4(image.load({x, y - rd, 0}));
            const auto vn11 = glm::dvec4(image.load({x, y, 0}));

            const auto v00 = glm::dvec4(image.load({x, y, 0}));
            const auto v01 = glm::dvec4(image.load({x, y + rd, 0}));
            const auto v10 = glm::dvec4(image.load({x + rd, y, 0}));
            const auto v11 = glm::dvec4(image.load({x + rd, y + rd, 0}));

            const auto pow2 = [](auto x) { return x * x; };

            const auto fx = v01 - v00;
            const auto fy = v10 - v00;

            const auto fnx = vn01 - vn00;
            const auto fny = vn10 - vn00;

            const auto fxx = fx * fx + fnx * fnx;
            const auto fyy = fy * fy + fny * fny;
            const auto fxy = fx * fy + fnx * fny;
            const auto fyx = fy * fx + fnx * fny;

            const auto t     = abs(0.5 * (fxx + fyy - glm::sqrt(pow2(fxx - fyy) + 4.0 * fxy * fyx)));
            const auto value = t.r + t.g + t.b > threshold;

            if (value) points.emplace(x, y);
        }
    return points;
}

int main()
{
    gfx::context_options opt;
    opt.use_window = false;
    const auto ctx = gfx::context::create(opt);
    ctx->make_current();

    gfx::host_image img(gfx::rgba32f, "shape.jpg");
    gauss_blur(img, 6, 0.1f);

    auto convx = img.converted(gfx::rgba8unorm);
    gfx::image_file::save_png("Out3.png", convx.extents().width, convx.extents().height, 4,
                              reinterpret_cast<const uint8_t*>(convx.storage().data()));

    auto features = tomasi_kanade(img, 20.2f);

    img *= 0.01f;
    for (const auto& fp : features) {
        for (int x = -4; x <= 4; ++x)
            img.store(glm::clamp<3, float, glm::highp>({glm::vec2(fp) + glm::vec2{x, 0}, 0}, {0, 0, 0},
                                                       {img.extents().width - 1, img.extents().height - 1, 0}),
                      {1, 0, 0, 1});

        for (int y = -4; y <= 4; ++y)
            img.store(glm::clamp<3, float, glm::highp>({glm::vec2(fp) + glm::vec2{0, y}, 0}, {0, 0, 0},
                                                       {img.extents().width - 1, img.extents().height - 1, 0}),
                      {1, 0, 0, 1});
    }
    auto conv = img.converted(gfx::rgba8unorm);
    gfx::image_file::save_png("Out.png", conv.extents().width, conv.extents().height, 4,
                              reinterpret_cast<const uint8_t*>(conv.storage().data()));
}