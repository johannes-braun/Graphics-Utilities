#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "gfx/graphics/opengl/formats.hpp"
#include <gfx/graphics/graphics_pipeline.hpp>
#include <gfx/graphics/opengl/binding_set_opengl.hpp>

int main()
{
    gfx::context_options opt;
    opt.graphics_api = gfx::gapi::opengl;
    opt.debug        = true;
    opt.use_window   = true;

    auto context = gfx::context::create(opt);
    context->make_current();

    //// Buffer
    gfx::hbuffer<float> x{1.f, 23.f, 29.f};
    gfx::hbuffer<float> src = std::move(x);
    gfx::buffer<float>  dst(gfx::buffer_usage::uniform, {2.f, 1.f, 0.5f, 10.f, 1.f, 9.f});
    dst << src;

    gfx::buf_copy(src, dst, dst.capacity());

    for (auto f : src) gfx::ilog << f;

    // Images
    const auto cubemap_format = gfx::rgba8unorm;
    const auto info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::image file_texture(gfx::img_type::image2d, cubemap_format, gfx::extent(info.width, info.height, 6), 11);
    file_texture.layer(0) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3));
    file_texture.layer(1) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3));
    file_texture.layer(2) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3));
    file_texture.layer(3) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3));
    file_texture.layer(4) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3));
    file_texture.layer(5) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3));
    file_texture.generate_mipmaps();

    gfx::image another_image(gfx::himage(gfx::rgba8unorm, gfx::image_file("Lena.png", gfx::bits::b8, 4)));

    auto cubemap = file_texture.view(gfx::imgv_type::image_cube);
    auto texture = another_image.view(gfx::imgv_type::image2d);

    // Samplers
    gfx::sampler sampler;
    sampler.set_anisotropy(true, 16.f);
    sampler.set_filter(gfx::filter_mode::min, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);
    // TODO: for building/creating a sampler it must be casted once.
    // VkSampler finished = gfx::handle_cast<VkSampler>(sampler);

    gfx::binding_layout layout1;
    layout1
        .push(gfx::binding_type::uniform_buffer)    // VK: { set: 0, binding: 0 }; GL: { uniform buffer binding: 0 }
        .push(gfx::binding_type::sampled_image)     // VK: { set: 0, binding: 1 }; GL: { texture binding: 0 }
        .push(gfx::binding_type::sampled_image)     // VK: { set: 0, binding: 2 }; GL: { texture binding: 1 }
        .push(gfx::binding_type::storage_image);    // VK: { set: 0, binding: 3 }; GL: { image binding: 0 }
    gfx::binding_layout layout2;
    layout2
        .push(gfx::binding_type::uniform_buffer, 2)    // VK: { set: 1, binding: 0, arr:0, 1 }; GL: { uniform buffer binding: 1, 2 }
        .push(gfx::binding_type::uniform_buffer)       // VK: { set: 1, binding: 1 }; GL: { uniform buffer binding: 3 }
        .push(gfx::binding_type::storage_buffer);      // VK: { set: 1, binding: 0 }; GL: { uniform buffer binding: 0

    gfx::binding_set set1(layout1);
    set1.bind(0, dst);
    set1.bind(1, cubemap, sampler);
    set1.bind(2, texture, sampler);
    set1.bind(3, texture);

    gfx::binding_set set2(layout2);
    set2.bind(0, 0, dst);
    set2.bind(0, 1, dst);
    set2.bind(1, src);
    set2.bind(2, src);

    gfx::v2::renderpass_layout rpl;
    rpl.add_color_attachment(gfx::rgba8unorm);
    rpl.set_depth_stencil_attachment(gfx::d32f);
	gfx::v2::graphics_pipeline pipeline({}, rpl, {
		gfx::shader{ gfx::shader_type::vert, "05_implementation_tests/vert.vert" },
		gfx::shader{ gfx::shader_type::frag, "05_implementation_tests/frag.frag" }
	});

    while (context->run()) {
    }

    std::cin.ignore();
}

//#include <gfx/graphics/general/handle.hpp>
//
// namespace gfx {
// inline namespace v1 {
// struct pipeline_state
//{
//    struct stencil_state
//    {
//        stencil_op failOp      = stencil_op::zero;
//        stencil_op passOp      = stencil_op::zero;
//        stencil_op depthFailOp = stencil_op::zero;
//        compare_op compareOp   = compare_op::never;
//        uint32_t   compareMask = 0;
//        uint32_t   writeMask   = 0;
//        uint32_t   reference   = 0;
//    };
//
//    struct vertex_attribute
//    {
//        u32       location = 0;
//        u32       binding  = 0;
//        format    fmt      = r32f;
//        ptrdiff_t offset   = 0;
//    };
//
//    struct vertex_binding
//    {
//        u32        binding = 0;
//        size_t     stride  = 0;
//        input_rate rate    = input_rate::vertex;
//    };
//
//    struct blend_attachment
//    {
//        bool                  blendEnable         = false;
//        blend_factor          srcColorBlendFactor = blend_factor::one;
//        blend_factor          dstColorBlendFactor = blend_factor::zero;
//        blend_op              colorBlendOp        = blend_op::op_add;
//        blend_factor          srcAlphaBlendFactor = blend_factor::one;
//        blend_factor          dstAlphaBlendFactor = blend_factor::zero;
//        blend_op              alphaBlendOp        = blend_op::op_add;
//        color_component_flags colorWriteMask      = color_components::rgba;
//    };
//
//    struct layout
//    {
//		std::vector<binding_layout*> binding_layouts;
//    }* state_bindings;
//
//    struct vertex_input
//    {
//        std::vector<vertex_attribute> attributes;
//        std::vector<vertex_binding>   bindings;
//    }* state_vertex_input = nullptr;
//
//    struct input_assembly
//    {
//        topology primitive_topology       = topology::triangle_list;
//        bool     primitive_restart_enable = false;
//    }* state_input_assembly = nullptr;
//
//    struct depth_stencil
//    {
//        bool          depth_test_enable        = true;
//        bool          depth_write_enable       = true;
//        compare_op    depth_compare            = compare_op::greater_or_equal;
//        bool          depth_bounds_test_enable = false;
//        bool          stencil_test_enable      = false;
//        stencil_state front;
//        stencil_state back;
//        float         depth_bounds_min = 0.f;
//        float         depth_bounds_max = 1.f;
//    }* state_depth_stencil = nullptr;
//
//    struct rasterizer
//    {
//        bool        depth_clamp_enable         = false;
//        bool        rasterizer_discard_enable  = false;
//        poly_mode   polygon_mode               = poly_mode::fill;
//        cull_mode   cull                       = cull_mode::none;
//        orientation front_face                 = orientation::ccw;
//        bool        depth_bias_enable          = false;
//        float       depth_bias_constant_factor = 1.f;
//        float       depth_bias_clamp           = 1.f;
//        float       depth_bias_slope_factor    = 0.f;
//        float       line_width                 = 1.f;
//    }* state_rasterizer = nullptr;
//
//    struct multisample
//    {
//        sample_count samples                  = sample_count::x1;
//        bool         sample_shading_enable    = false;
//        float        min_sample_shading       = 0.f;
//        uint32_t*    sample_masks             = nullptr;    // ptr?
//        bool         alpha_to_coverage_enable = false;
//        bool         alpha_to_one_enable      = false;
//    }* state_multisample = nullptr;
//
//    struct blending
//    {
//        bool                          logic_op_enable = false;
//        logic_op                      op              = logic_op::op_copy;
//        std::vector<blend_attachment> attachments;
//        float                         blend_constants[4]{0, 0, 0, 0};
//    }* state_blending = nullptr;
//
//    struct tesselation
//    {
//        uint32_t patch_control_points = 3;
//    }* state_tesselation = nullptr;
//
//    struct render_area
//    {
//        std::vector<viewport> viewports;
//        std::vector<scissor>  scissors;
//    }* state_viewports = nullptr;
//};
//
// void enable_for(bool b, GLenum e)
//{
//	b ? glEnable(e) : glDisable(e);
//}
//
// std::function<void()> apply_func(pipeline_state::rasterizer* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::rasterizer{}]
//    {
//        enable_for(obj.rasterizer_discard_enable, GL_RASTERIZER_DISCARD);
//        enable_for(obj.cull != cull_mode::none, GL_CULL_FACE);
//
//        if (obj.cull != cull_mode::none)
//            glCullFace([&]() {
//                switch (obj.cull)
//                {
//                case cull_mode::back: return GL_BACK;
//                case cull_mode::front: return GL_FRONT;
//                case cull_mode::front_and_back: return GL_FRONT_AND_BACK;
//                default: return GL_NONE;
//                }
//            }());
//        glFrontFace([&]() {
//            switch (obj.front_face)
//            {
//            case orientation::ccw: return GL_CCW;
//            case orientation::cw: return GL_CW;
//            }
//            return GLenum(0);
//        }());
//        glPolygonMode(GL_FRONT_AND_BACK, [&]() {
//            switch (obj.polygon_mode)
//            {
//            case poly_mode::fill: return GL_FILL;
//            case poly_mode::line: return GL_LINE;
//            case poly_mode::point: return GL_POINT;
//            }
//            return GLenum(0);
//        }());
//
//        glLineWidth(obj.line_width);
//        enable_for(obj.depth_clamp_enable, GL_DEPTH_CLAMP);
//        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_FILL);
//        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_LINE);
//        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_POINT);
//        glPolygonOffsetClamp(obj.depth_bias_slope_factor, obj.depth_bias_constant_factor, obj.depth_bias_clamp);
//    };
//}
//
// std::function<void()> apply_func(pipeline_state::render_area* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::render_area{}]
//    {
//        uint32_t i = 0;
//        for (const auto& vp : obj.viewports) {
//            glViewportIndexedf(i, vp.x, vp.y, vp.width, vp.height);
//            glDepthRangeIndexed(i++, vp.min_depth, vp.max_depth);
//        }
//        i = 0;
//        for (const auto& sc : obj.scissors) {
//            const auto size = sc.size();
//            glScissorIndexed(i++, sc.min.x, sc.min.y, size.x, size.y);
//        }
//    };
//}
//
// std::function<void()> apply_func(pipeline_state::tesselation* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::tesselation{}] { glPatchParameteri(GL_PATCH_VERTICES, obj.patch_control_points); };
//}
//
// std::function<void()> apply_func(pipeline_state::depth_stencil* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::depth_stencil{}]
//    {
//        enable_for(obj.depth_test_enable, GL_DEPTH_TEST);
//        enable_for(obj.depth_bounds_test_enable, GL_DEPTH_BOUNDS_TEST_EXT);
//        glDepthMask(obj.depth_write_enable);
//        glDepthBoundsEXT(obj.depth_bounds_min, obj.depth_bounds_max);
//        enable_for(obj.stencil_test_enable, GL_STENCIL_TEST);
//
//        const auto get_compare = [&](compare_op o) {
//            switch (o)
//            {
//            case compare_op::never: return GL_NEVER;
//            case compare_op::less: return GL_LESS;
//            case compare_op::equal: return GL_EQUAL;
//            case compare_op::less_or_equal: return GL_LEQUAL;
//            case compare_op::greater: return GL_GREATER;
//            case compare_op::not_equal: return GL_NOTEQUAL;
//            case compare_op::greater_or_equal: return GL_GEQUAL;
//            case compare_op::always: return GL_ALWAYS;
//            default: return GL_NEVER;
//            }
//        };
//
//        glDepthFunc(get_compare(obj.depth_compare));
//
//        const auto get_stencil_op = [](stencil_op o) {
//            switch (o)
//            {
//            case stencil_op::keep: return GL_KEEP;
//            case stencil_op::zero: return GL_ZERO;
//            case stencil_op::replace: return GL_REPLACE;
//            case stencil_op::inc_clamp: return GL_INCR;
//            case stencil_op::dec_clamp: return GL_DECR;
//            case stencil_op::inv: return GL_INVERT;
//            case stencil_op::inc_wrap: return GL_INCR_WRAP;
//            case stencil_op::dec_wrap: return GL_DECR_WRAP;
//            }
//            return GLenum(0);
//        };
//
//        glStencilFuncSeparate(GL_FRONT, get_compare(obj.front.compareOp), obj.front.reference, obj.front.compareMask);
//        glStencilMaskSeparate(GL_FRONT, obj.front.writeMask);
//        glStencilOpSeparate(GL_FRONT, get_stencil_op(obj.front.failOp), get_stencil_op(obj.front.depthFailOp),
//                            get_stencil_op(obj.front.passOp));
//
//        glStencilFuncSeparate(GL_BACK, get_compare(obj.back.compareOp), obj.back.reference, obj.back.compareMask);
//        glStencilMaskSeparate(GL_BACK, obj.back.writeMask);
//        glStencilOpSeparate(GL_BACK, get_stencil_op(obj.back.failOp), get_stencil_op(obj.back.depthFailOp),
//                            get_stencil_op(obj.back.passOp));
//    };
//}
//
// std::function<void()> apply_func(pipeline_state::blending* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::blending{}]
//    {
//        enable_for(obj.logic_op_enable, GL_COLOR_LOGIC_OP);
//        glLogicOp([&]() {
//            switch (obj.op)
//            {
//            case logic_op::op_and: return GL_AND;
//            case logic_op::op_and_inv: return GL_AND_INVERTED;
//            case logic_op::op_and_rev: return GL_AND_REVERSE;
//            case logic_op::op_clear: return GL_CLEAR;
//            case logic_op::op_copy: return GL_COPY;
//            case logic_op::op_copy_inv: return GL_COPY_INVERTED;
//            case logic_op::op_eqiv: return GL_EQUIV;
//            case logic_op::op_inv: return GL_INVERT;
//            case logic_op::op_nand: return GL_NAND;
//            case logic_op::op_none: return GL_NONE;
//            case logic_op::op_nor: return GL_NOR;
//            case logic_op::op_or: return GL_OR;
//            case logic_op::op_or_inv: return GL_OR_INVERTED;
//            case logic_op::op_or_rev: return GL_OR_REVERSE;
//            case logic_op::op_set: return GL_SET;
//            case logic_op::op_xor: return GL_XOR;
//            }
//            return GLenum(0);
//        }());
//        glBlendColor(obj.blend_constants[0], obj.blend_constants[1], obj.blend_constants[2], obj.blend_constants[3]);
//
//        for (int i = 0; i < 15; ++i) {
//            if (i < obj.attachments.size()) {
//                const auto& a = obj.attachments[i];
//                a.blendEnable ? glEnablei(GL_BLEND, i) : glDisablei(GL_BLEND, i);
//
//                const auto get_fac = [](blend_factor f) {
//                    switch (f)
//                    {
//                    case blend_factor::constant_alpha: return GL_CONSTANT_ALPHA;
//                    case blend_factor::constant_color: return GL_CONSTANT_COLOR;
//                    case blend_factor::dst_alpha: return GL_DST_ALPHA;
//                    case blend_factor::dst_color: return GL_DST_COLOR;
//                    case blend_factor::one: return GL_ONE;
//                    case blend_factor::one_minus_constant_alpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
//                    case blend_factor::one_minus_constant_color: return GL_ONE_MINUS_CONSTANT_COLOR;
//                    case blend_factor::one_minus_dst_alpha: return GL_ONE_MINUS_DST_ALPHA;
//                    case blend_factor::one_minus_dst_color: return GL_ONE_MINUS_DST_COLOR;
//                    case blend_factor::one_minus_src1_alpha: return GL_ONE_MINUS_SRC1_ALPHA;
//                    case blend_factor::one_minus_src1_color: return GL_ONE_MINUS_SRC1_COLOR;
//                    case blend_factor::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
//                    case blend_factor::one_minus_src_color: return GL_ONE_MINUS_SRC_COLOR;
//                    case blend_factor::src1_alpha: return GL_SRC1_ALPHA;
//                    case blend_factor::src1_color: return GL_SRC1_COLOR;
//                    case blend_factor::src_alpha: return GL_SRC_ALPHA;
//                    case blend_factor::src_alpha_saturate: return GL_SRC_ALPHA_SATURATE;
//                    case blend_factor::src_color: return GL_SRC_COLOR;
//                    case blend_factor::zero: return GL_ZERO;
//                    }
//                    return GLenum(0);
//                };
//
//                const auto get_fun = [](blend_op op) {
//                    switch (op)
//                    {
//                    case blend_op::op_add: return GL_FUNC_ADD;
//                    case blend_op::op_sub: return GL_FUNC_SUBTRACT;
//                    case blend_op::op_sub_rev: return GL_FUNC_REVERSE_SUBTRACT;
//                    case blend_op::op_min: return GL_MIN;
//                    case blend_op::op_max: return GL_MAX;
//                    case blend_op::op_zero:
//                    case blend_op::op_src:
//                    case blend_op::op_dst:
//                    case blend_op::op_src_over:
//                    case blend_op::op_dst_out:
//                    case blend_op::op_src_atop:
//                    case blend_op::op_dst_atop:
//                    case blend_op::op_xor:
//                    case blend_op::op_mul:
//                    case blend_op::op_screen:
//                    case blend_op::op_overlay:
//                    case blend_op::op_darken:
//                    case blend_op::op_lighten:
//                    case blend_op::op_color_dodge:
//                    case blend_op::op_color_burn:
//                    case blend_op::op_hard_light:
//                    case blend_op::op_soft_light:
//                    case blend_op::op_diff:
//                    case blend_op::op_exclusion:
//                    case blend_op::op_invert:
//                    case blend_op::op_linear_burn:
//                    case blend_op::op_vivid_light:
//                    case blend_op::op_linear_light:
//                    case blend_op::op_pin_light:
//                    case blend_op::op_hard_mix:
//                    case blend_op::op_hsl_hue:
//                    case blend_op::op_hsl_saturation:
//                    case blend_op::op_hsl_color:
//                    case blend_op::op_hsl_luminosity:
//                    case blend_op::op_plus:
//                    case blend_op::op_plus_clamped:
//                    case blend_op::op_plus_clamped_alpha:
//                    case blend_op::op_plus_darker:
//                    case blend_op::op_minus:
//                    case blend_op::op_minus_clamped:
//                    case blend_op::op_contrast:
//                    case blend_op::op_invert_ovg:
//                    case blend_op::op_red:
//                    case blend_op::op_green:
//                    case blend_op::op_blue: elog << "Blend op not supported."; return GL_FUNC_ADD;
//                    }
//                    return GLenum(0);
//                };
//
//                glBlendFuncSeparatei(i, get_fac(a.srcColorBlendFactor), get_fac(a.dstColorBlendFactor), get_fac(a.srcAlphaBlendFactor),
//                                     get_fac(a.dstAlphaBlendFactor));
//                glBlendEquationSeparatei(i, get_fun(a.colorBlendOp), get_fun(a.alphaBlendOp));
//                glColorMaski(i, (a.colorWriteMask & color_components::r) == color_components::r,
//                             (a.colorWriteMask & color_components::g) == color_components::g,
//                             (a.colorWriteMask & color_components::b) == color_components::b,
//                             (a.colorWriteMask & color_components::a) == color_components::a);
//            }
//        }
//    };
//}
//
// std::function<void()> apply_func(pipeline_state::multisample* ptr)
//{
//    return [obj = ptr ? *ptr : pipeline_state::multisample{}]
//    {
//        enable_for(obj.sample_shading_enable, GL_MULTISAMPLE);
//        enable_for(obj.alpha_to_coverage_enable, GL_SAMPLE_ALPHA_TO_COVERAGE);
//        enable_for(obj.alpha_to_one_enable, GL_SAMPLE_ALPHA_TO_ONE);
//        glMinSampleShading(obj.min_sample_shading);
//        for (auto i = 0; i < static_cast<int>(obj.samples) && obj.sample_masks; ++i) {
//            glSampleMaski(i, GLbitfield(obj.sample_masks[i]));
//        }
//        // no equivalent to sample count};
//    };
//}    // namespace v1
//}    // namespace v1
// inline namespace v2 {
// class graphics_pipeline
//{
// public:
//	graphics_pipeline(const pipeline_state& state)
//	{
//		_apply_state.push_back(apply_func(state.state_blending));
//		_apply_state.push_back(apply_func(state.state_multisample));
//		_apply_state.push_back(apply_func(state.state_depth_stencil));
//		_apply_state.push_back(apply_func(state.state_tesselation));
//		_apply_state.push_back(apply_func(state.state_viewports));
//		_apply_state.push_back(apply_func(state.state_rasterizer));
//		_apply_state.push_back([en = state.state_input_assembly->primitive_restart_enable]{ enable_for(en, GL_PRIMITIVE_RESTART); });
//
//		glCreateVertexArrays(1, &_vao);
//
//		if (state.state_vertex_input)
//		{
//			for (auto& attr : state.state_vertex_input->attributes)
//			{
//				glEnableVertexArrayAttrib(_vao, attr.location);
//				opengl::format_info info = opengl::format_from(attr.fmt);
//
//				int count = [&]() {
//					switch (info.external_format)
//					{
//					case GL_RED: return 1;
//					case GL_RG: return 2;
//					case GL_RGB: return 3;
//					case GL_RGBA: return 4;
//					default: return 0;
//					}
//				}();
//
//				glVertexArrayAttribFormat(_vao, attr.location, count, info.external_type, info.normalized, static_cast<u32>(attr.offset));
//				glVertexArrayAttribBinding(_vao, attr.location, attr.binding);
//			}
//
//			for (auto& bnd : state.state_vertex_input->bindings)
//			{
//				glVertexArrayBindingDivisor(_vao, bnd.binding, static_cast<u32>(bnd.rate));
//				_binding_strides[bnd.binding] = bnd.stride;
//			}
//		}
//
//        if(state.state_bindings)
//        {
//            // Nothing.
//        }
//	}
//
// private:
//	movable_handle<mygl::vertex_array> _vao;
//	std::unordered_map<uint32_t, size_t> _binding_strides;
//	std::vector<std::function<void()>> _apply_state;
//};
//}    // namespace v2
//}    // namespace gfx
//
// void dos()
//{
//    gfx::pipeline_state                state;
//    gfx::pipeline_state::depth_stencil ds;
//    state.state_depth_stencil = &ds;
//}