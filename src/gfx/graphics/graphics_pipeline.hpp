#pragma once

#include "../api.hpp"
#include "../flags.hpp"
#include "../geometry.hpp"
#include "../type.hpp"
#include "binding_layout.hpp"
#include "formats.hpp"
#include "shader.hpp"
#include <vector>

namespace gfx {
inline namespace v1 {

enum class input_rate
{
    vertex,
    instance
};

enum class topology
{
    point_list,
    line_list,
    line_strip,
    triangle_list,
    triangle_strip,
    triangle_fan,
    line_list_adj,
    line_strip_adj,
    triangle_list_adj,
    triangle_strip_adj,
    patch_list
};

enum class compare_op
{
    never,
    less,
    equal,
    less_or_equal,
    greater,
    not_equal,
    greater_or_equal,
    always
};

enum class poly_mode
{
    fill,
    line,
    point
};

enum class cull_mode
{
    none,
    front,
    back,
    front_and_back
};

enum class orientation
{
    cw,
    ccw
};

enum class sample_count
{
    x1  = 1 << 0,
    x2  = 1 << 1,
    x4  = 1 << 2,
    x8  = 1 << 3,
    x16 = 1 << 4,
    x32 = 1 << 5,
    x64 = 1 << 6
};

enum class logic_op
{
    op_clear,
    op_and,
    op_and_rev,
    op_copy,
    op_and_inv,
    op_none,
    op_xor,
    op_or,
    op_nor,
    op_eqiv,
    op_inv,
    op_or_rev,
    op_copy_inv,
    op_or_inv,
    op_nand,
    op_set
};

enum class blend_factor
{
    zero,
    one,
    src_color,
    one_minus_src_color,
    dst_color,
    one_minus_dst_color,
    src_alpha,
    one_minus_src_alpha,
    dst_alpha,
    one_minus_dst_alpha,
    constant_color,
    one_minus_constant_color,
    constant_alpha,
    one_minus_constant_alpha,
    src_alpha_saturate,
    src1_color,
    one_minus_src1_color,
    src1_alpha,
    one_minus_src1_alpha
};

enum class blend_op
{
    op_add,
    op_sub,
    op_sub_rev,
    op_min,
    op_max,
    op_zero,
    op_src,
    op_dst,
    op_src_over,
    op_dst_over,
    op_src_in,
    op_dst_in,
    op_src_out,
    op_dst_out,
    op_src_atop,
    op_dst_atop,
    op_xor,
    op_mul,
    op_screen,
    op_overlay,
    op_darken,
    op_lighten,
    op_color_dodge,
    op_color_burn,
    op_hard_light,
    op_soft_light,
    op_diff,
    op_exclusion,
    op_invert,
    op_linear_burn,
    op_vivid_light,
    op_linear_light,
    op_pin_light,
    op_hard_mix,
    op_hsl_hue,
    op_hsl_saturation,
    op_hsl_color,
    op_hsl_luminosity,
    op_plus,
    op_plus_clamped,
    op_plus_clamped_alpha,
    op_plus_darker,
    op_minus,
    op_minus_clamped,
    op_contrast,
    op_invert_ovg,
    op_red,
    op_green,
    op_blue
};

enum class stencil_op
{
    keep,
    zero,
    replace,
    inc_clamp,
    dec_clamp,
    inv,
    inc_wrap,
    dec_wrap
};

enum class color_components : u32
{
    r    = 1 << 0,
    g    = 1 << 1,
    b    = 1 << 2,
    a    = 1 << 3,
    rg   = r | g,
    rgb  = r | g | b,
    rgba = r | g | b | a
};
using color_component_flags = flags<u32, color_components>;
inline color_component_flags operator|(color_components a, color_components b)
{
    return color_component_flags(a) | b;
}

struct viewport
{
    viewport() = default;
    viewport(float x, float y, float w, float h, float n, float f) : x(x), y(y), width(w), height(h), min_depth(n), max_depth(f) {}

    float x;
    float y;
    float width;
    float height;
    float min_depth;
    float max_depth;
};

using scissor = bounds<int, 2, 2>;

class renderpass_layout
{
public:
    renderpass_layout(sample_count samples = sample_count::x1) : _samples(samples) {}

    void add_color_attachment(format fmt, format resolve_fmt = format::unspecified)
    {
        auto& p  = _color_attachment_formats.emplace_back();
        p.first  = fmt;
        p.second = resolve_fmt == format::unspecified ? fmt : resolve_fmt;
    }
    void set_depth_stencil_attachment(format fmt) { _depth_attachment_format = fmt; }


    sample_count                                  samples() const noexcept { return _samples; }
    const std::vector<std::pair<format, format>>& color_attachment_formats() const noexcept { return _color_attachment_formats; }
    format                                        depth_attachment_format() const noexcept { return _depth_attachment_format; }

private:
    sample_count                           _samples;
    std::vector<std::pair<format, format>> _color_attachment_formats;
	format                                 _depth_attachment_format = format::unspecified;
};

struct pipeline_state
{
    struct stencil_state
    {
        stencil_op failOp      = stencil_op::zero;
        stencil_op passOp      = stencil_op::zero;
        stencil_op depthFailOp = stencil_op::zero;
        compare_op compareOp   = compare_op::never;
        u32        compareMask = 0;
        u32        writeMask   = 0;
        u32        reference   = 0;
    };

    struct vertex_attribute
    {
		vertex_attribute(u32 loc, u32 binding, format fmt, i64 offset) : location(loc), binding(binding), fmt(fmt), offset(offset) {}
        u32       location = 0;
        u32       binding  = 0;
        format    fmt      = r32f;
        i64 offset   = 0;
    };

    struct vertex_binding
    {
		vertex_binding(u32 binding, size_t stride, input_rate rate = input_rate::vertex) : binding(binding), stride(stride), rate(rate) {}
        u32        binding = 0;
        size_t     stride  = 0;
        input_rate rate    = input_rate::vertex;
    };

    struct blend_attachment
    {
        bool                  blendEnable         = false;
        blend_factor          srcColorBlendFactor = blend_factor::one;
        blend_factor          dstColorBlendFactor = blend_factor::zero;
        blend_op              colorBlendOp        = blend_op::op_add;
        blend_factor          srcAlphaBlendFactor = blend_factor::one;
        blend_factor          dstAlphaBlendFactor = blend_factor::zero;
        blend_op              alphaBlendOp        = blend_op::op_add;
        color_component_flags colorWriteMask      = color_components::rgba;
    };

    struct layout
    {
        std::vector<binding_layout*> binding_layouts;
    }* state_bindings = nullptr;

    struct vertex_input
    {
        std::vector<vertex_attribute> attributes;
        std::vector<vertex_binding>   bindings;
    }* state_vertex_input = nullptr;

    struct input_assembly
    {
        topology primitive_topology       = topology::triangle_list;
        bool     primitive_restart_enable = false;
    }* state_input_assembly = nullptr;

    struct depth_stencil
    {
        bool          depth_test_enable        = true;
        bool          depth_write_enable       = true;
        compare_op    depth_compare            = compare_op::greater_or_equal;
        bool          depth_bounds_test_enable = false;
        bool          stencil_test_enable      = false;
        stencil_state front;
        stencil_state back;
        float         depth_bounds_min = 0.f;
        float         depth_bounds_max = 1.f;
    }* state_depth_stencil = nullptr;

    struct rasterizer
    {
        bool        depth_clamp_enable         = false;
        bool        rasterizer_discard_enable  = false;
        poly_mode   polygon_mode               = poly_mode::fill;
        cull_mode   cull                       = cull_mode::none;
        orientation front_face                 = orientation::ccw;
        bool        depth_bias_enable          = false;
        float       depth_bias_constant_factor = 0.f;
        float       depth_bias_clamp           = 0.f;
        float       depth_bias_slope_factor    = 0.f;
        float       line_width                 = 1.f;
    }* state_rasterizer = nullptr;

    struct multisample
    {
        sample_count samples                  = sample_count::x1;
        bool         sample_shading_enable    = false;
        float        min_sample_shading       = 0.f;
        u32*         sample_masks             = nullptr;    // ptr?
        bool         alpha_to_coverage_enable = false;
        bool         alpha_to_one_enable      = false;
    }* state_multisample = nullptr;

    struct blending
    {
        bool                          logic_op_enable = false;
        logic_op                      op              = logic_op::op_copy;
        std::vector<blend_attachment> attachments;
        float                         blend_constants[4]{0, 0, 0, 0};
    }* state_blending = nullptr;

    struct tesselation
    {
        u32 patch_control_points = 3;
    }* state_tesselation = nullptr;

    struct render_area
    {
        std::vector<viewport> viewports;
        std::vector<scissor>  scissors;
    }* state_viewports = nullptr;
};

namespace detail {
class graphics_pipeline_implementation
{
public:
    virtual ~graphics_pipeline_implementation() = default;
    static std::unique_ptr<graphics_pipeline_implementation> make();

    virtual void   initialize(const pipeline_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders) = 0;
    virtual handle api_handle()                                                                                                        = 0;
};

class compute_pipeline_implementation
{
public:
    virtual ~compute_pipeline_implementation() = default;
    static std::unique_ptr<compute_pipeline_implementation> make();

    virtual void   initialize(const pipeline_state::layout& layout, const v1::shader& cs) = 0;
    virtual handle api_handle()                                                           = 0;
};
}    // namespace detail

class graphics_pipeline : public impl::implements<detail::graphics_pipeline_implementation>
{
public:
    graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass, std::initializer_list<v1::shader> shaders);
    graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass,
                      std::initializer_list<const v1::shader* const> shaders);
    graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders);
};

class compute_pipeline : public impl::implements<detail::compute_pipeline_implementation>
{
public:
    compute_pipeline(const pipeline_state::layout& layout, const v1::shader& cs);
};

}    // namespace v2
}    // namespace gfx
