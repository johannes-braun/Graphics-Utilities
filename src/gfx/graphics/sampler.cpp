#include "sampler.hpp"
#include <gfx/context.hpp>
#include <gfx/graphics/opengl/sampler_opengl.hpp>

namespace gfx
{
std::unique_ptr<detail::sampler_implementation> detail::sampler_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::sampler_implementation>();
    case gapi::vulkan: return nullptr;
    default:;
    }
    return nullptr;
}

sampler::sampler()
{
    set_filter(gfx::filter_mode::min, gfx::filter::linear);
    set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);
}

void sampler::set_filter(filter_mode mode, filter filter)
{
    implementation()->set_filter(mode, filter);
}

void sampler::set_wrap(wrap w, wrap_mode mode)
{
    implementation()->set_wrap(w, mode);
}

void sampler::set_border(border_color color)
{
    implementation()->set_border(color);
}

void sampler::set_lod(lod mode, float value)
{
    implementation()->set_lod(mode, value);
}

void sampler::set_anisotropy(bool enable, float value)
{
    implementation()->set_anisotropy(enable, value);
}

void sampler::set_compare(bool enable, compare_op op)
{
    implementation()->set_compare(enable, op);
}
}    // namespace gfx
