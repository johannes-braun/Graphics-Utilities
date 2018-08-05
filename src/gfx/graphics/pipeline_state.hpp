#pragma once

#include "../flags.hpp"
#include "../geometry.hpp"
#include "vertex_input.hpp"
#include <cinttypes>
#include <vector>

namespace gfx {
inline namespace v1 {
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

enum class color_components : uint32_t
{
    r    = 1 << 0,
    g    = 1 << 1,
    b    = 1 << 2,
    a    = 1 << 3,
    rg   = r | g,
    rgb  = r | g | b,
    rgba = r | g | b | a
};
using color_component_flags = flags<uint32_t, color_components>;
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

struct pipeline_state
{
    using bool32 = uint32_t;

    struct stencil_state
    {
        stencil_op failOp      = stencil_op::zero;
        stencil_op passOp      = stencil_op::zero;
        stencil_op depthFailOp = stencil_op::zero;
        compare_op compareOp   = compare_op::never;
        uint32_t   compareMask = 0;
        uint32_t   writeMask   = 0;
        uint32_t   reference   = 0;
    };

    struct state_info_depth_stencil
    {
        bool32     depth_test_enable        = true;
        bool32     depth_write_enable       = true;
        compare_op depth_compare            = compare_op::greater_or_equal;
        bool32     depth_bounds_test_enable = false;

        bool32        stencil_test_enable = false;
        stencil_state front;
        stencil_state back;

        float depth_bounds_min = 0.f;
        float depth_bounds_max = 1.f;
    } depth_stencil;

    struct state_info_rasterizer
    {
        bool32      depth_clamp_enable        = false;
        bool32      rasterizer_discard_enable = false;
        poly_mode   polygon_mode              = poly_mode::fill;
        cull_mode   cull                      = cull_mode::none;
        orientation front_face                = orientation::ccw;

        bool32 depth_bias_enable          = false;
        float  depth_bias_constant_factor = 1.f;
        float  depth_bias_clamp           = 1.f;
        float  depth_bias_slope_factor    = 0.f;

        float line_width = 1.f;
    } rasterizer;

    struct state_info_multisample
    {
        sample_count samples                  = sample_count::x1;
        bool32       sample_shading_enable    = false;
        float        min_sample_shading       = 0.f;
        uint32_t*    sample_masks             = nullptr;    // ptr?
        bool32       alpha_to_coverage_enable = false;
        bool32       alpha_to_one_enable      = false;
    } multisample;

    struct blend_attachment
    {
        bool32                blendEnable         = false;
        blend_factor          srcColorBlendFactor = blend_factor::one;
        blend_factor          dstColorBlendFactor = blend_factor::zero;
        blend_op              colorBlendOp        = blend_op::op_add;
        blend_factor          srcAlphaBlendFactor = blend_factor::one;
        blend_factor          dstAlphaBlendFactor = blend_factor::zero;
        blend_op              alphaBlendOp        = blend_op::op_add;
        color_component_flags colorWriteMask      = color_components::rgba;
    };

    struct state_info_blend
    {
        bool32                        logic_op_enable = false;
        logic_op                      op              = logic_op::op_copy;
        std::vector<blend_attachment> attachments;
        float                         blend_constants[4]{0, 0, 0, 0};
    } blend;

    struct state_info_tesselation
    {
        uint32_t patch_control_points = 3;
    } tesselation;

    struct state_info_viewport
    {
        std::vector<viewport> viewports;
        std::vector<scissor>  scissors;
    } viewport;
};
}    // namespace v1
}    // namespace gfx