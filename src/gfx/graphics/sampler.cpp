#include "sampler.hpp"
#include <gfx/context.hpp>
#include <gfx/graphics/opengl/sampler_opengl.hpp>

namespace gfx
{
std::unique_ptr<detail::sampler_implementation> detail::make_sampler_implementation()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::sampler_implementation>();
    case gapi::vulkan:
        return nullptr;
    default:;
    }
    return nullptr;
}

sampler::sampler()
        : _implementation(detail::make_sampler_implementation())
{
}

void sampler::set_filter(filter_mode mode, filter filter) { _implementation->set_filter(mode, filter); }

void sampler::set_wrap(wrap w, wrap_mode mode) { _implementation->set_wrap(w, mode); }

void sampler::set_border(border_color color) { _implementation->set_border(color); }

void sampler::set_lod(lod mode, float value) { _implementation->set_lod(mode, value); }

void sampler::set_anisotropy(bool enable, float value) { _implementation->set_anisotropy(enable, value); }

void sampler::set_compare(bool enable, compare_op op) { _implementation->set_compare(enable, op); }
} // namespace gfx
