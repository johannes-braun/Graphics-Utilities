#include "formats.hpp"
#include "pipeline_opengl.hpp"
#include <gfx/log.hpp>

namespace gfx {
	inline namespace v1 {
namespace opengl {
void enable_for(bool b, GLenum e)
{
    b ? glEnable(e) : glDisable(e);
}
std::function<void()> apply_func(pipe_state::rasterizer* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::rasterizer{}]
    {
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
        enable_for(obj.rasterizer_discard_enable, GL_RASTERIZER_DISCARD);
        enable_for(obj.cull != cull_mode::none, GL_CULL_FACE);

        if (obj.cull != cull_mode::none)
            glCullFace([&]() {
                switch (obj.cull)
                {
                case cull_mode::back: return GL_BACK;
                case cull_mode::front: return GL_FRONT;
                case cull_mode::front_and_back: return GL_FRONT_AND_BACK;
                default: return GL_NONE;
                }
            }());
        glFrontFace([&]() {
            switch (obj.front_face)
            {
            case orientation::ccw: return GL_CCW;
            case orientation::cw: return GL_CW;
            }
            return GLenum(0);
        }());
        glPolygonMode(GL_FRONT_AND_BACK, [&]() {
            switch (obj.polygon_mode)
            {
            case poly_mode::fill: return GL_FILL;
            case poly_mode::line: return GL_LINE;
            case poly_mode::point: return GL_POINT;
            }
            return GLenum(0);
        }());

        glLineWidth(obj.line_width);
        enable_for(obj.depth_clamp_enable, GL_DEPTH_CLAMP);
        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_FILL);
        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_LINE);
        enable_for(obj.depth_bias_enable, GL_POLYGON_OFFSET_POINT);
        glPolygonOffsetClamp(obj.depth_bias_slope_factor, obj.depth_bias_constant_factor, obj.depth_bias_clamp);
    };
}

std::function<void()> apply_func(pipe_state::render_area* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::render_area{}]
    {
        uint32_t i = 0;
        for (const auto& vp : obj.viewports) {
            glViewportIndexedf(i, vp.x, vp.y, vp.width, vp.height);
            glDepthRangeIndexed(i++, vp.min_depth, vp.max_depth);
        }
        i = 0;
        for (const auto& sc : obj.scissors) {
            const auto size = sc.size();
            glScissorIndexed(i++, sc.min.x, sc.min.y, size.x, size.y);
        }
    };
}

std::function<void()> apply_func(pipe_state::tesselation* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::tesselation{}] { glPatchParameteri(GL_PATCH_VERTICES, obj.patch_control_points); };
}

std::function<void()> apply_func(pipe_state::depth_stencil* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::depth_stencil{}]
    {
        enable_for(obj.depth_test_enable, GL_DEPTH_TEST);
        enable_for(obj.depth_bounds_test_enable, GL_DEPTH_BOUNDS_TEST_EXT);
        glDepthMask(obj.depth_write_enable);
        glDepthBoundsEXT(obj.depth_bounds_min, obj.depth_bounds_max);
        enable_for(obj.stencil_test_enable, GL_STENCIL_TEST);

        const auto get_compare = [&](compare_op o) {
            switch (o)
            {
            case compare_op::never: return GL_NEVER;
            case compare_op::less: return GL_LESS;
            case compare_op::equal: return GL_EQUAL;
            case compare_op::less_or_equal: return GL_LEQUAL;
            case compare_op::greater: return GL_GREATER;
            case compare_op::not_equal: return GL_NOTEQUAL;
            case compare_op::greater_or_equal: return GL_GEQUAL;
            case compare_op::always: return GL_ALWAYS;
            default: return GL_NEVER;
            }
        };

        glDepthFunc(get_compare(obj.depth_compare));

        const auto get_stencil_op = [](stencil_op o) {
            switch (o)
            {
            case stencil_op::keep: return GL_KEEP;
            case stencil_op::zero: return GL_ZERO;
            case stencil_op::replace: return GL_REPLACE;
            case stencil_op::inc_clamp: return GL_INCR;
            case stencil_op::dec_clamp: return GL_DECR;
            case stencil_op::inv: return GL_INVERT;
            case stencil_op::inc_wrap: return GL_INCR_WRAP;
            case stencil_op::dec_wrap: return GL_DECR_WRAP;
            }
            return GLenum(0);
        };

        glStencilFuncSeparate(GL_FRONT, get_compare(obj.front.compareOp), obj.front.reference, obj.front.compareMask);
        glStencilMaskSeparate(GL_FRONT, obj.front.writeMask);
        glStencilOpSeparate(GL_FRONT, get_stencil_op(obj.front.failOp), get_stencil_op(obj.front.depthFailOp),
                            get_stencil_op(obj.front.passOp));

        glStencilFuncSeparate(GL_BACK, get_compare(obj.back.compareOp), obj.back.reference, obj.back.compareMask);
        glStencilMaskSeparate(GL_BACK, obj.back.writeMask);
        glStencilOpSeparate(GL_BACK, get_stencil_op(obj.back.failOp), get_stencil_op(obj.back.depthFailOp),
                            get_stencil_op(obj.back.passOp));
    };
}

std::function<void()> apply_func(pipe_state::blending* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::blending{}]
    {
        enable_for(obj.logic_op_enable, GL_COLOR_LOGIC_OP);
        glLogicOp([&]() {
            switch (obj.op)
            {
            case logic_op::op_and: return GL_AND;
            case logic_op::op_and_inv: return GL_AND_INVERTED;
            case logic_op::op_and_rev: return GL_AND_REVERSE;
            case logic_op::op_clear: return GL_CLEAR;
            case logic_op::op_copy: return GL_COPY;
            case logic_op::op_copy_inv: return GL_COPY_INVERTED;
            case logic_op::op_eqiv: return GL_EQUIV;
            case logic_op::op_inv: return GL_INVERT;
            case logic_op::op_nand: return GL_NAND;
            case logic_op::op_none: return GL_NONE;
            case logic_op::op_nor: return GL_NOR;
            case logic_op::op_or: return GL_OR;
            case logic_op::op_or_inv: return GL_OR_INVERTED;
            case logic_op::op_or_rev: return GL_OR_REVERSE;
            case logic_op::op_set: return GL_SET;
            case logic_op::op_xor: return GL_XOR;
            }
            return GLenum(0);
        }());
        glBlendColor(obj.blend_constants[0], obj.blend_constants[1], obj.blend_constants[2], obj.blend_constants[3]);

        for (int i = 0; i < 15; ++i) {
            if (i < obj.attachments.size()) {
                const auto& a = obj.attachments[i];
                a.blendEnable ? glEnablei(GL_BLEND, i) : glDisablei(GL_BLEND, i);

                const auto get_fac = [](blend_factor f) {
                    switch (f)
                    {
                    case blend_factor::constant_alpha: return GL_CONSTANT_ALPHA;
                    case blend_factor::constant_color: return GL_CONSTANT_COLOR;
                    case blend_factor::dst_alpha: return GL_DST_ALPHA;
                    case blend_factor::dst_color: return GL_DST_COLOR;
                    case blend_factor::one: return GL_ONE;
                    case blend_factor::one_minus_constant_alpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
                    case blend_factor::one_minus_constant_color: return GL_ONE_MINUS_CONSTANT_COLOR;
                    case blend_factor::one_minus_dst_alpha: return GL_ONE_MINUS_DST_ALPHA;
                    case blend_factor::one_minus_dst_color: return GL_ONE_MINUS_DST_COLOR;
                    case blend_factor::one_minus_src1_alpha: return GL_ONE_MINUS_SRC1_ALPHA;
                    case blend_factor::one_minus_src1_color: return GL_ONE_MINUS_SRC1_COLOR;
                    case blend_factor::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
                    case blend_factor::one_minus_src_color: return GL_ONE_MINUS_SRC_COLOR;
                    case blend_factor::src1_alpha: return GL_SRC1_ALPHA;
                    case blend_factor::src1_color: return GL_SRC1_COLOR;
                    case blend_factor::src_alpha: return GL_SRC_ALPHA;
                    case blend_factor::src_alpha_saturate: return GL_SRC_ALPHA_SATURATE;
                    case blend_factor::src_color: return GL_SRC_COLOR;
                    case blend_factor::zero: return GL_ZERO;
                    }
                    return GLenum(0);
                };

                const auto get_fun = [](blend_op op) {
                    switch (op)
                    {
                    case blend_op::op_add: return GL_FUNC_ADD;
                    case blend_op::op_sub: return GL_FUNC_SUBTRACT;
                    case blend_op::op_sub_rev: return GL_FUNC_REVERSE_SUBTRACT;
                    case blend_op::op_min: return GL_MIN;
                    case blend_op::op_max: return GL_MAX;
                    case blend_op::op_zero:
                    case blend_op::op_src:
                    case blend_op::op_dst:
                    case blend_op::op_src_over:
                    case blend_op::op_dst_out:
                    case blend_op::op_src_atop:
                    case blend_op::op_dst_atop:
                    case blend_op::op_xor:
                    case blend_op::op_mul:
                    case blend_op::op_screen:
                    case blend_op::op_overlay:
                    case blend_op::op_darken:
                    case blend_op::op_lighten:
                    case blend_op::op_color_dodge:
                    case blend_op::op_color_burn:
                    case blend_op::op_hard_light:
                    case blend_op::op_soft_light:
                    case blend_op::op_diff:
                    case blend_op::op_exclusion:
                    case blend_op::op_invert:
                    case blend_op::op_linear_burn:
                    case blend_op::op_vivid_light:
                    case blend_op::op_linear_light:
                    case blend_op::op_pin_light:
                    case blend_op::op_hard_mix:
                    case blend_op::op_hsl_hue:
                    case blend_op::op_hsl_saturation:
                    case blend_op::op_hsl_color:
                    case blend_op::op_hsl_luminosity:
                    case blend_op::op_plus:
                    case blend_op::op_plus_clamped:
                    case blend_op::op_plus_clamped_alpha:
                    case blend_op::op_plus_darker:
                    case blend_op::op_minus:
                    case blend_op::op_minus_clamped:
                    case blend_op::op_contrast:
                    case blend_op::op_invert_ovg:
                    case blend_op::op_red:
                    case blend_op::op_green:
                    case blend_op::op_blue: elog << "Blend op not supported."; return GL_FUNC_ADD;
                    }
                    return GLenum(0);
                };

                glBlendFuncSeparatei(i, get_fac(a.srcColorBlendFactor), get_fac(a.dstColorBlendFactor), get_fac(a.srcAlphaBlendFactor),
                                     get_fac(a.dstAlphaBlendFactor));
                glBlendEquationSeparatei(i, get_fun(a.colorBlendOp), get_fun(a.alphaBlendOp));
                glColorMaski(i, (a.colorWriteMask & color_components::r) == color_components::r,
                             (a.colorWriteMask & color_components::g) == color_components::g,
                             (a.colorWriteMask & color_components::b) == color_components::b,
                             (a.colorWriteMask & color_components::a) == color_components::a);
            }
        }
    };
}

std::function<void()> apply_func(pipe_state::multisample* ptr)
{
    return [obj = ptr ? *ptr : pipe_state::multisample{}]
    {
        enable_for(obj.sample_shading_enable, GL_MULTISAMPLE);
        enable_for(obj.alpha_to_coverage_enable, GL_SAMPLE_ALPHA_TO_COVERAGE);
        enable_for(obj.alpha_to_one_enable, GL_SAMPLE_ALPHA_TO_ONE);
        glMinSampleShading(obj.min_sample_shading);
        for (auto i = 0; i < static_cast<int>(obj.samples) && obj.sample_masks; ++i) {
            glSampleMaski(i, GLbitfield(obj.sample_masks[i]));
        }
        // no equivalent to sample count};
    };
}    // namespace v1

GLbitfield stage_bitfield(GLenum st) noexcept
{
    switch (st)
    {
    case GL_VERTEX_SHADER: return GL_VERTEX_SHADER_BIT;
    case GL_FRAGMENT_SHADER: return GL_FRAGMENT_SHADER_BIT;
    case GL_GEOMETRY_SHADER: return GL_GEOMETRY_SHADER_BIT;
    case GL_TESS_CONTROL_SHADER: return GL_TESS_CONTROL_SHADER_BIT;
    case GL_TESS_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER_BIT;
    case GL_COMPUTE_SHADER: return GL_COMPUTE_SHADER_BIT;
    default: return GL_ZERO;
    }
}

auto make_shader(shader_type t, const shader& s, mygl::pipeline handle)
{
    const auto ty = [&]() {
        switch (t)
        {
        case shader_type::vert: return GL_VERTEX_SHADER;
        case shader_type::frag: return GL_FRAGMENT_SHADER;
        case shader_type::comp: return GL_COMPUTE_SHADER;
        case shader_type::geom: return GL_GEOMETRY_SHADER;
        case shader_type::tesc: return GL_TESS_CONTROL_SHADER;
        case shader_type::tese: return GL_TESS_EVALUATION_SHADER;
        }
        return GLenum(0);
    }();
    if (s.format() == shader_format::text) {
        const char*   src[2] = {reinterpret_cast<const char*>(s.data().data()), "\n"};
        shader_module temp{glCreateShaderProgramv(ty, 2, src)};

        int success = 0;
        glGetProgramiv(temp.s, GL_LINK_STATUS, &success);
        if (!success) {
            int log_length;
            glGetProgramiv(temp.s, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramInfoLog(temp.s, log_length, &log_length, log.data());
            glDeleteProgram(temp.s);
            elog("shader") << log;
            return shader_module{mygl::shader_program::zero};
        }

        glUseProgramStages(handle, stage_bitfield(ty), temp.s);
        return temp;
    }
    else if (s.format() == shader_format::spirv)
    {
        mygl::shader cs = glCreateShader(ty);
        glShaderBinary(1, &cs, GL_SHADER_BINARY_FORMAT_SPIR_V, s.data().data(), static_cast<int>(s.data().size()));
        glSpecializeShader(cs, "main", 0, nullptr, nullptr);
        shader_module prog{glCreateProgram()};
        glProgramParameteri(prog.s, GL_PROGRAM_SEPARABLE, true);
        glAttachShader(prog.s, cs);
        glLinkProgram(prog.s);
        glDetachShader(prog.s, cs);
        glDeleteShader(cs);
        int success = 0;
        glGetProgramiv(prog.s, GL_LINK_STATUS, &success);
        if (!success) {
            int log_length;
            glGetProgramiv(prog.s, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramInfoLog(prog.s, log_length, &log_length, log.data());
            glDeleteProgram(prog.s);
            elog("shader") << log;
            return shader_module{mygl::shader_program::zero};
        }
        glUseProgramStages(handle, stage_bitfield(ty), prog.s);
        return prog;
    }
    return shader_module{mygl::shader_program::zero};
}

bool validate(mygl::pipeline handle)
{
    glValidateProgramPipeline(handle);

    int success = 0;
    glGetProgramPipelineiv(handle, GL_VALIDATE_STATUS, &success);
    if (!success) {
        int log_length;
        glGetProgramPipelineiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length, ' ');
        glGetProgramPipelineInfoLog(handle, log_length, &log_length, log.data());
        elog("shader") << log;
        return false;
    }
    return true;
}

graphics_pipeline_implementation::~graphics_pipeline_implementation()
{
    if (glIsProgramPipeline(_pipeline)) glDeleteProgramPipelines(1, &_pipeline);
}

void graphics_pipeline_implementation::initialize(const pipe_state& state, const renderpass_layout& renderpass,
                                                  span<const v1::shader* const> shaders)
{
    _apply_state.push_back(apply_func(state.state_blending));
    _apply_state.push_back(apply_func(state.state_multisample));
    _apply_state.push_back(apply_func(state.state_depth_stencil));
    _apply_state.push_back(apply_func(state.state_tesselation));
    _apply_state.push_back(apply_func(state.state_render_area));
    _apply_state.push_back(apply_func(state.state_rasterizer));
    _apply_state.push_back([en = state.state_input_assembly ? state.state_input_assembly->primitive_restart_enable : false] {
        enable_for(en, GL_PRIMITIVE_RESTART);
    });

    glCreateVertexArrays(1, &_vao);
    if (state.state_input_assembly) {
        _prim_topology = state.state_input_assembly->primitive_topology;
    }
    if (state.state_vertex_input) {
        for (auto& attr : state.state_vertex_input->attributes) {
            glEnableVertexArrayAttrib(_vao, attr.location);
            v1::opengl::format_info info = v1::opengl::format_from(attr.fmt);

            int count = [&]() {
                switch (info.external_format)
                {
                case GL_RED: return 1;
                case GL_RG: return 2;
                case GL_RGB: return 3;
                case GL_RGBA: return 4;
                default: return 0;
                }
            }();

            glVertexArrayAttribFormat(_vao, attr.location, count, info.external_type, info.normalized, static_cast<u32>(attr.offset));
            glVertexArrayAttribBinding(_vao, attr.location, attr.binding);
        }

        for (auto& bnd : state.state_vertex_input->bindings) {
            glVertexArrayBindingDivisor(_vao, bnd.binding, static_cast<u32>(bnd.rate));
            _binding_strides[bnd.binding] = bnd.stride;
        }
    }

    if (state.state_bindings) {
		for (auto& l : state.state_bindings->layouts)
			_proxy_sets.emplace_back(*l);
    }

    glCreateProgramPipelines(1, &_pipeline);

    _shds.resize(shaders.size());
    for (int i = 0; i < _shds.size(); ++i)
        if ((_shds[i] = make_shader(shaders[i]->stage(), *shaders[i], _pipeline)).s.get() == mygl::shader_program::zero)
            elog << "A shader compilation failed.";

    validate(_pipeline);
}

handle graphics_pipeline_implementation::api_handle()
{
    return mygl::pipeline(_pipeline);
}

void graphics_pipeline_implementation::apply_all()
{
	glBindProgramPipeline(_pipeline);
	for (auto& s : _apply_state)
		s();
	glBindVertexArray(_vao);
}

compute_pipeline_implementation::~compute_pipeline_implementation()
{
    if (glIsProgramPipeline(_pipeline)) glDeleteProgramPipelines(1, &_pipeline);
}

void compute_pipeline_implementation::initialize(const pipe_state::binding_layouts& layout, const shader& cs)
{
    glCreateProgramPipelines(1, &_pipeline);
    _shader = make_shader(cs.stage(), cs, _pipeline);
    validate(_pipeline);

	for (auto& l : layout.layouts)
		_proxy_sets.emplace_back(*l);
}

handle compute_pipeline_implementation::api_handle()
{
    return mygl::pipeline(_pipeline);
}
}    // namespace opengl
}    // namespace v2
}    // namespace gfx