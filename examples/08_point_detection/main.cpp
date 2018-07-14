#include <gfx/gfx.hpp>
#include <iostream>
#include <regex>
#include <unordered_set>

void gauss_blur(gfx::host_image& image, int kernel_radius, float sigma)
{
    gfx::host_image img2(image.pixel_format(), image.extents());

    std::vector<float> kernel(2 * kernel_radius + 1);
    kernel[kernel_radius + 1] = glm::gauss(0.f, 0.f, sigma);
#pragma omp parallel for schedule(static)
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

auto gradient_image(const gfx::host_image& image, int window_size = 5)
{
    const float        sigma = 1.f;
    std::vector<float> kernel(window_size);
    kernel[window_size >> 1] = glm::gauss(0.f, 0.f, sigma);
#pragma omp parallel for schedule(static)
    for (int i = 1; i <= window_size >> 1; ++i) {
        kernel[(window_size >> 1) - i] = glm::gauss(i * 1.f, 0.f, sigma);
        kernel[(window_size >> 1) + i] = glm::gauss(i * 1.f, 0.f, sigma);
    }

    std::vector<std::vector<float>> kernel2d;
    for (int i = 0; i < window_size; ++i) {
        kernel2d.push_back(kernel);
        std::transform(kernel2d.back().begin(), kernel2d.back().end(), kernel2d.back().begin(), [&](auto x) { return x * kernel[i]; });
    }

    gfx::host_image df_img(image.pixel_format(), image.extents());

    // glm::dvec4 lum = glm::dvec4{glm::vec3(.299, .587, .114) * 0.75f, 0.25f};
    glm::dvec4 lum = glm::dvec4{1, 1, 1, 0.0f};

#pragma omp parallel for
    for (int p = 0; p < image.extents().count(); ++p) {
        const glm::uvec3 pixel = image.extents().subpixel(p);
        const int        x     = pixel.x;
        const int        y     = pixel.y;

        double          sobelx{};
        double          sobely{};
        const glm::mat3 s_op{1, 2, 1, 0, 0, 0, -1, -2, -1};
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                const auto cl = image.extents().clamp({x + i - 1, y + j - 1, 0});
                sobelx += dot(glm::dvec4(image.load(cl)), lum) * s_op[i][j];
                sobely += dot(glm::dvec4(image.load(cl)), lum) * s_op[j][i];
            }
        df_img.store({x, y, 0}, {sobelx, sobely, 0, 1});
    }

    gfx::host_image deriv_image(image.pixel_format(), image.extents());
#pragma omp parallel for
    for (int p = 0; p < image.extents().count(); ++p) {
        const glm::uvec3 pixel = image.extents().subpixel(p);
        const int        x     = pixel.x;
        const int        y     = pixel.y;

        const int win_size      = window_size;
        const int win_size_half = win_size >> 1;

        double dfdxx = 0;
        double dfdxy = 0;
        double dfdyx = 0;
        double dfdyy = 0;

        for (int dwx = 0; dwx < win_size; ++dwx)
            for (int dwy = 0; dwy < win_size; ++dwy) {
                const auto pow2 = [](auto x) { return x * x; };

                const auto clamped_pixel = df_img.extents().clamp({x + dwx - win_size_half, y + dwy - win_size_half, 0});
                const auto gradient      = df_img.load(clamped_pixel);
                const auto dfdx          = gradient.x;
                const auto dfdy          = gradient.y;

                dfdxx += kernel2d[dwx][dwy] * pow2(dfdx);
                dfdxy += kernel2d[dwx][dwy] * (dfdx * dfdy);
                dfdyx += kernel2d[dwx][dwy] * (dfdx * dfdy);
                dfdyy += kernel2d[dwx][dwy] * pow2(dfdy);
            }

        deriv_image.store(glm::uvec3{x, y, 0}, glm::vec4{dfdxx, dfdxy, dfdyx, dfdyy});
    }
    return deriv_image;
}

namespace std
{
template<typename T, typename K>
struct hash<std::pair<T, K>>
{
    auto operator()(const std::pair<T, K>& p) const { return (hash<T>()(p.first) << 1) ^ hash<K>()(p.second); }
};
}

int main()
{
    gfx::context_options opt;
    opt.use_window = false;
    const auto ctx = gfx::context::create(opt);
    ctx->make_current();

    if (auto img_file = gfx::file::open_dialog("Open Image", gfx::file(), {"*.png", "*.jpg", "*.jpeg"}); img_file) {
        gfx::hlog << "Threshold or Command:";
        const std::regex float_regex("[+-]?([0-9]*[.])?[0-9]+");
        std::string      line;

        enum class alg
        {
            harris,
            shi_tomasi_kanade
        } calg       = alg::harris;
        int win_size = 5;
        while (getline(std::cin, line)) {
            std::stringstream input(line);
            float             thresh = std::numeric_limits<float>::max();

            if (!std::regex_match(line, float_regex)) {
                if (line == "exit")
                    break;
                else if (line == "open")
                {
                    auto n_img_file = gfx::file::open_dialog("Open Image", gfx::file(), {"*.png", "*.jpg", "*.jpeg"});
                    if (n_img_file) img_file = n_img_file;
                }
                else if (line == "help")
                {
                    gfx::ilog << "Commands:";
                    gfx::ilog << "    \"exit\" - Exits the application";
                    gfx::ilog << "    \"open\" - Shows a file dialog to open an image";
                    gfx::ilog << "    \"ws\" - Alter window size";
                    gfx::ilog << "    \"alg [h|stk]\" - Sets the detection algorithm to [h]arris or [s]hi-[t]omasi-[k]anade";
                    gfx::ilog << "    \"help\" - Guess what.";
                }
                else if (memcmp(line.c_str(), "alg", 3) == 0)
                {
                    if (line.length() > 4) {
                        if (line.substr(4) == "stk") {
                            calg = alg::shi_tomasi_kanade;
                            gfx::ilog << "Switching to shi_tomasi-Kanade";
                        }
                        else if (line.substr(4) == "h")
                        {
                            calg = alg::harris;
                            gfx::ilog << "Switching to Harris";
                        }
                    }
                    else
                    {
                        gfx::ilog << "Current alg is " << (calg == alg::harris ? "Harris" : "Shi-Thomasi-Kanade")
                                  << ". To switch, use h or stk as parameters.";
                    }
                }
                else if (line == "ws")
                {
                    gfx::hlog << "Enter a new window size:";
                    std::cin >> win_size;
                }
                gfx::hlog << "Threshold or Command:";
                continue;
            }
            input >> thresh;

            gfx::host_image img(gfx::rgba32f, *img_file);

            const auto start       = std::chrono::steady_clock::now();
            auto       deriv_image = gradient_image(img, win_size);

            gfx::ilog << "Gradient Image marker: " << (std::chrono::steady_clock::now() - start).count() << "ns";

            std::unordered_set<std::pair<glm::uvec2, double>> h_features;
            std::unordered_set<std::pair<glm::uvec2, double>> tk_features;
            constexpr int                                     rd = 1;
            for (int x = rd; x < deriv_image.extents().width - rd; ++x)
                for (int y = rd; y < deriv_image.extents().height - rd; ++y) {
                    const auto der  = deriv_image.load({x, y, 0});
                    const auto fxx  = der.x;
                    const auto fxy  = der.y;
                    const auto fyx  = der.z;
                    const auto fyy  = der.a;
                    const auto pow2 = [](auto x) { return x * x; };

                    const glm::mat2 harris_mat = {{fxx, fyx}, {fxy, fyy}};
                    const auto      k          = 0.05f;
                    const auto      harris     = glm::determinant(harris_mat) - k * pow2(fxx + fyy);

                    const auto shi_tomasi_kanade = 0.5 * (fxx + fyy - glm::sqrt(pow2(fxx - fyy) + 4.0 * pow2(fxy * fyx)));

                    if (harris > thresh) h_features.emplace(glm::uvec2(x, y), harris);
                    if (shi_tomasi_kanade > thresh) tk_features.emplace(glm::uvec2(x, y), shi_tomasi_kanade);
                }

            gfx::ilog << "Feature extraction marker: " << (std::chrono::steady_clock::now() - start).count() << "ns";

            auto& features = calg == alg::harris ? h_features : tk_features;

            std::vector<int>                           counts;
            std::vector<std::pair<glm::uvec2, double>> collapsed;
            for (const auto& f : features) {
                if (auto it = std::find_if(collapsed.begin(), collapsed.end(),
                                           [&](const std::pair<glm::uvec2, double>& ref) {
                                               return glm::distance(glm::vec2(ref.first), glm::vec2(f.first)) < 5;
                                           });
                    it != collapsed.end())
                {
                    if (it->second < f.second) *it = f;
                }
                else
                {
                    collapsed.push_back(f);
                    counts.push_back(1);
                }
            }
            gfx::ilog << "Feature collapsing marker: " << (std::chrono::steady_clock::now() - start).count() << "ns";

            img *= 0.3f;
            for (const auto& fp : collapsed) {
                for (int x = -4; x <= 4; ++x)
                    img.store(glm::clamp<3, float, glm::highp>({glm::vec2(fp.first) + glm::vec2{x, 0}, 0}, {0, 0, 0},
                                                               {img.extents().width - 1, img.extents().height - 1, 0}),
                              {1, 0, 0, 1});

                for (int y = -4; y <= 4; ++y)
                    img.store(glm::clamp<3, float, glm::highp>({glm::vec2(fp.first) + glm::vec2{0, y}, 0}, {0, 0, 0},
                                                               {img.extents().width - 1, img.extents().height - 1, 0}),
                              {1, 0, 0, 1});
            }
            auto conv = img.converted(gfx::rgba8unorm);
            gfx::image_file::save_png("Out-" + std::to_string(thresh) + ".png", conv.extents().width, conv.extents().height, 4,
                                      reinterpret_cast<const uint8_t*>(conv.storage().data()));
            gfx::hlog << "Threshold or Command:";
        }
    }
}