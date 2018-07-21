#pragma once

#include "implementation.hpp"
#include "state_info.hpp"
#include <any>
#include <gfx/api.hpp>
#include <memory>
#include <unordered_map>

namespace gfx {
inline namespace v1 {
enum class filter_mode
{
    mag,
    min,
    mipmap
};

enum class filter
{
    nearest,
    linear
};

enum class wrap
{
    u,
    v,
    w
};

enum class wrap_mode
{
    none,
    repeat,
    mirror_repeat,
    clamp_to_edge,
    clamp_to_border,
    mirror_clamp_to_edge
};

enum class border_color
{
    float_transparent_black,
    int_transparent_black,
    float_opaque_black,
    int_opaque_black,
    float_opaque_white,
    int_opaque_white
};

enum class lod
{
    bias,
    min,
    max
};

namespace detail {
class sampler_implementation
{
public:
    static std::unique_ptr<sampler_implementation> make();

    virtual ~sampler_implementation()                        = default;
    virtual void set_filter(filter_mode mode, filter filter) = 0;
    virtual void set_wrap(wrap w, wrap_mode mode)            = 0;
    virtual void set_border(border_color color)              = 0;
    virtual void set_lod(lod mode, float value)              = 0;
    virtual void set_anisotropy(bool enable, float value)    = 0;
    virtual void set_compare(bool enable, compare_op op)     = 0;

    virtual std::any api_handle() = 0;
};
}    // namespace detail

GFX_api_cast_type(gapi::opengl, sampler, mygl::sampler)

    class sampler : public impl::implements<detail::sampler_implementation>
{
public:
    sampler();
    void set_filter(filter_mode mode, filter filter);
    void set_wrap(wrap w, wrap_mode mode);
    void set_border(border_color color);

    void set_lod(lod mode, float value);

    void set_anisotropy(bool enable, float value);
    void set_compare(bool enable, compare_op op = compare_op::never);

    GFX_api_cast_op(gapi::opengl, sampler)

        private :

        std::unordered_map<filter_mode, filter> _filters;
    std::unordered_map<wrap, wrap_mode> _wraps;
    border_color                        _border_color = border_color::float_transparent_black;
    std::unordered_map<lod, float>      _lod;

    bool       _enable_anisotropy = false;
    float      _anisotropy_value  = 0.f;
    bool       _enable_compare    = false;
    compare_op _compare_op        = compare_op::never;
};

GFX_api_cast_impl(gapi::opengl, sampler)

}    // namespace v1
}    // namespace gfx