#include "catch.hpp"
#include <gfx/gfx.hpp>
#include <glm/glm.hpp>

struct context_provider_gl
{
    context_provider_gl()
    {
        gfx::context_options opt;
        opt.use_window = false;
        context        = gfx::context::create(opt);
        context->make_current();
    }

    std::shared_ptr<gfx::context> context;
};

TEST_CASE_METHOD(context_provider_gl, "Image load/store", "[image]")
{
    const glm::vec4   pixel_value{1.f, 0.4f, 0.22f, 103878.f};
    const glm::uvec3  pixel{9, 3, 5};
    const gfx::extent size(10, 4, 7);

    SECTION("Raw 32-bit float values should be equal.")
    {
        gfx::himage image(gfx::rgba32f, size);
        image.store(pixel, pixel_value);

        REQUIRE(image.load(pixel) == pixel_value);
    }

    SECTION("Check whether 8-bit-unorms are computed correctly.")
    {
        gfx::himage image(gfx::rgba8unorm, size);
        image.store(pixel, pixel_value);
        glm::vec4   val      = image.load(pixel);
        glm::u8vec4 expected = glm::clamp(pixel_value * 255.f, 0.f, 255.f);
        glm::u8vec4 got      = glm::clamp(val * 255.f, 0.f, 255.f);

        REQUIRE(expected == got);
    }

    SECTION("Check equivalence of packed formats.")
    {
        gfx::himage r11g11b10f(gfx::r11g11b10f, size);
        gfx::himage rgb9e5(gfx::rgb9e5, size);
        gfx::himage rgb10a2unorm(gfx::rgb10a2unorm, size);

        r11g11b10f.store(pixel, pixel_value);
        rgb9e5.store(pixel, pixel_value);
        rgb10a2unorm.store(pixel, pixel_value);

        const auto almost = [](const auto x, const auto y, const auto thresh) -> bool {
            return glm::all(glm::lessThan(glm::abs(x - y), thresh));
        };

        const glm::vec3 col1 = r11g11b10f.load(pixel);
        const glm::vec3 col2 = rgb9e5.load(pixel);
        const glm::vec4 col3 = rgb10a2unorm.load(pixel);

        REQUIRE(almost(col1, glm::vec3(pixel_value), glm::vec3(0.01f)));
        REQUIRE(almost(col2, glm::vec3(pixel_value), glm::vec3(0.01f)));
        REQUIRE(almost(glm::vec3(col3), glm::vec3(pixel_value), glm::vec3(0.001f)));
        REQUIRE(abs(col3.a - std::clamp(pixel_value.a, 0.f, 2.f * 2 * 2 * 2) < 0.3f));
    }
}

TEST_CASE_METHOD(context_provider_gl, "Image operations", "[image]")
{
    const glm::vec4   pixel_value{1.f, 0.4f, 0.22f, 103878.f};
    const glm::uvec3  pixel{9, 3, 5};
    const gfx::extent size(10, 4, 7);

    gfx::himage base(gfx::rgba32f, size);
    base.store(pixel, pixel_value);

    SECTION("Test for equality and copies")
    {
        gfx::himage copy = base;
        REQUIRE(copy == base);
        copy.store(pixel, {0, 0, 0, 0});
        REQUIRE(copy != base);
    }

    SECTION("Addition")
    {
        gfx::himage copy = base + base;
        REQUIRE(copy.load(pixel) == base.load(pixel) + base.load(pixel));
        copy += base;
        REQUIRE(copy.load(pixel) == base.load(pixel) + 2 * base.load(pixel));

        copy = base + 8;
        REQUIRE(copy.load(pixel) == base.load(pixel) + 8.f);
        copy += 2;
        REQUIRE(copy.load(pixel) == base.load(pixel) + 10.f);

        copy = base;
        copy += glm::vec4(1, 0, 3, 2);
        REQUIRE(copy.load(pixel) == base.load(pixel) + glm::vec4(1, 0, 3, 2));
    }

    SECTION("Subtraction")
    {
        gfx::himage copy = base - base;
        REQUIRE(copy.load(pixel) == glm::vec4(0));
        copy -= base;
        REQUIRE(copy.load(pixel) == -base.load(pixel));

        copy = base - 1;
        REQUIRE(copy.load(pixel) == base.load(pixel) - 1.f);
        copy -= 2;
        REQUIRE(copy.load(pixel) == base.load(pixel) - 3.f);
    }

    SECTION("Operations on uint images") { 
        gfx::himage iimage(gfx::rgba32u, size);
        iimage += 10;
        iimage -= 3;
        iimage *= 4;
        iimage /= 2;

        REQUIRE(iimage.loadu(pixel) == glm::uvec4((((0 + 10) - 3) * 4) / 2));

        iimage += iimage;
        REQUIRE(iimage.loadu(pixel) == glm::uvec4(2*((((0 + 10) - 3) * 4) / 2)));
    }

    // Remaining cases 
}


TEST_CASE_METHOD(context_provider_gl, "Image conversion", "[image]")
{
    const glm::vec4   pixel_value{1.f, 0.4f, 0.22f, 103878.f};
    const glm::uvec3  pixel{9, 3, 5};
    const gfx::extent size(10, 4, 7);

    gfx::himage base(gfx::rgba32f, size);
    base.store(pixel, pixel_value);

    gfx::himage rgba8(gfx::rgba8unorm, size);
    rgba8.store(pixel, pixel_value);

    gfx::himage rgb9e5(gfx::rgb9e5, size);
    rgb9e5.store(pixel, pixel_value);

    REQUIRE(base.converted(gfx::rgba8unorm) == rgba8);
    REQUIRE(base.converted(gfx::rgb9e5) == rgb9e5);
}