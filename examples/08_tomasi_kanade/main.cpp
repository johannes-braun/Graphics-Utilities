#include <gfx/gfx.hpp>
#include <iostream>
#include <unordered_set>

void gauss_blur(gfx::host_image& image, int kernel_radius, float sigma)
{
    gfx::host_image img2(image.pixel_format(), image.extents());

    std::vector<float> kernel(2 * kernel_radius + 1);
    kernel[kernel_radius + 1] = glm::gauss(0.f, 0.f, sigma);
#pragma omp parallel for schedule(static)
    for (int i = 1; i < kernel_radius; ++i)
    {
        kernel[kernel_radius + 1 - i] = glm::gauss(i * 1.f, 0.f, sigma);
        kernel[kernel_radius + 1 + i] = glm::gauss(i * 1.f, 0.f, sigma);
    }
#pragma omp parallel for schedule(static)
    for (int x = 0; x < image.extents().width; ++x)
        for (int y = 0; y < image.extents().height; ++y)
        {
            glm::vec4 color{0};
            for (int i = -int(kernel.size() >> 1); i <= int(kernel.size() >> 1); ++i)
                color += kernel[i + (kernel.size() >> 1)] * image.load({glm::clamp<int>(x + i, 0, image.extents().width - 1), y, 0});
            img2.store({x, y, 0}, color);
        }
#pragma omp parallel for schedule(static)
    for (int x = 0; x < img2.extents().width; ++x)
        for (int y = 0; y < img2.extents().height; ++y)
        {
            glm::vec4 color{0};
            for (int i = -int(kernel.size() >> 1); i <= int(kernel.size() >> 1); ++i)
                color += kernel[i + (kernel.size() >> 1)] * img2.load({x, glm::clamp<int>(y + i, 0, img2.extents().height - 1), 0});
            image.store({x, y, 0}, color);
        }
}

auto tomasi_kanade(const gfx::host_image& image, double threshold)
{
    std::unordered_set<glm::uvec2> points;
    gfx::host_image                deriv_image(image.pixel_format(), image.extents());
    constexpr int                  rd = 1;
    for (int x = rd; x < image.extents().width - rd; ++x)
        for (int y = rd; y < image.extents().height - rd; ++y)
        {
            glm::dvec4 lum = glm::dvec4{glm::vec3(.299, .587, .114) * 0.75f, 0.25f};

            const auto v_m_m = dot(glm::dvec4(image.load({x - rd, y - rd, 0})), lum);
            const auto v_0_m = dot(glm::dvec4(image.load({x, y - rd, 0})), lum);
            const auto v_p_m = dot(glm::dvec4(image.load({x + rd, y - rd, 0})), lum);

            const auto v_m_0 = dot(glm::dvec4(image.load({x - rd, y, 0})), lum);
            const auto v_0_0 = dot(glm::dvec4(image.load({x, y, 0})), lum);
            const auto v_p_0 = dot(glm::dvec4(image.load({x + rd, y, 0})), lum);

            const auto v_m_p = dot(glm::dvec4(image.load({x - rd, y + rd, 0})), lum);
            const auto v_0_p = dot(glm::dvec4(image.load({x, y + rd, 0})), lum);
            const auto v_p_p = dot(glm::dvec4(image.load({x + rd, y + rd, 0})), lum);

            const auto fpx = abs(v_p_0 - v_0_0);
            const auto fnx = abs(v_0_0 - v_m_0);
            const auto fpy = abs(v_0_p - v_0_0);
            const auto fny = abs(v_0_0 - v_0_m);

            const auto fpyx = abs(v_p_m - v_0_0);
            const auto fnyx = abs(v_0_0 - v_m_p);
            const auto fpxy = abs(v_p_p - v_0_0);
            const auto fnxy = abs(v_0_0 - v_m_m);

            const auto fxx = fpx * fpx + fnx * fnx;
            const auto fyy = fpy * fpy + fny * fny;
            const auto fxy = (fpxy * fpxy + fnxy * fnxy);
            const auto fyx = (fpyx * fpyx + fnyx * fnyx);

            deriv_image.store(glm::uvec3{x, y, 0}, glm::vec4{fxx, fxy, fyx, fyy});
        }
    return deriv_image;
}

int main()
{
    gfx::context_options opt;
    opt.use_window = false;
    const auto ctx = gfx::context::create(opt);
    ctx->make_current();

    if (const auto img_file = gfx::file::open_dialog("Open Image", gfx::file(), {"*.png", "*.jpg", "*.jpeg"}); img_file)
    {
        gfx::hlog << "Threshold:";
        float thresh = 0;
        while (std::cin >> thresh)
        {
            gfx::host_image img(gfx::rgba32f, *img_file);

            auto deriv_image = tomasi_kanade(img, thresh);
            gauss_blur(deriv_image, 8, 3.f);
            auto convy = deriv_image.converted(gfx::rgba8unorm);
            gfx::image_file::save_png("DERV.png", convy.extents().width, convy.extents().height, 4,
                                      reinterpret_cast<const uint8_t*>(convy.storage().data()));

            std::unordered_set<glm::uvec2> features;
            constexpr int                  rd = 1;
            for (int x = rd; x < deriv_image.extents().width - rd; ++x)
                for (int y = rd; y < deriv_image.extents().height - rd; ++y)
                {
                    const auto der  = deriv_image.load({x, y, 0});
                    const auto fxx  = der.x;
                    const auto fxy  = der.y;
                    const auto fyx  = der.z;
                    const auto fyy  = der.a;
                    const auto pow2 = [](auto x) { return x * x; };

                    const auto t = abs(0.5 * (fxx + fyy - glm::sqrt(pow2(fxx - fyy) + 4.0 * pow2(fxy * fyx))));
                    if (t > thresh) features.emplace(x, y);
                }

           /* std::vector<int>        counts;
            std::vector<glm::uvec2> collapsed;
            for (const auto& f : features)
            {
                if (auto it = std::find_if(collapsed.begin(), collapsed.end(),
                                           [&](const glm::uvec2& ref) { return glm::distance(glm::vec2(ref), glm::vec2(f)) < 5; });
                    it != collapsed.end())
                { *it = glm::uvec2(glm::mix(glm::vec2(*it), glm::vec2(f), 1.f / (++counts[std::distance(collapsed.begin(), it)])));
                } else
                {
                    collapsed.push_back(f);
                    counts.push_back(1);
                }
            }*/

            img *= 0.1f;
            for (const auto& fp : features)
            {
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
            gfx::image_file::save_png("Out-" + std::to_string(thresh) + ".png", conv.extents().width, conv.extents().height, 4,
                                      reinterpret_cast<const uint8_t*>(conv.storage().data()));
            gfx::hlog << "Threshold:";
        }
    }
}