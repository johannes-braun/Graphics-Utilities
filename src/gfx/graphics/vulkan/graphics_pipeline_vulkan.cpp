#include "context_vulkan.hpp"
#include "device_image_vulkan.hpp"
#include "graphics_pipeline_vulkan.hpp"
#include "init_struct.hpp"
#include <gfx/log.hpp>

namespace gfx {
namespace v2 {
namespace vulkan {
graphics_pipeline_implementation::~graphics_pipeline_implementation()
{
    if (_pipeline) vkDestroyPipeline(_device, _pipeline, nullptr);
    if (_layout) vkDestroyPipelineLayout(_device, _layout, nullptr);
}

void graphics_pipeline_implementation::initialize(const pipeline_state& state, const renderpass_layout& renderpass,
                                                  span<const v1::shader* const> shaders)
{
    auto& ctx = context::current();
    auto impl = static_cast<v1::vulkan::context_implementation*>(std::any_cast<v1::detail::context_implementation*>(ctx->implementation()));
    _device   = impl->device();

    v1::vulkan::init<VkPipelineLayoutCreateInfo> pl_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    std::vector<VkDescriptorSetLayout> layouts;
    if (state.state_bindings) {
        for (auto& bl : state.state_bindings->binding_layouts) layouts.push_back(handle_cast<VkDescriptorSetLayout>(*bl));
    }
    pl_info.setLayoutCount = layouts.size();
    pl_info.pSetLayouts    = layouts.data();
    vkCreatePipelineLayout(_device, &pl_info, nullptr, &_layout);

    v1::vulkan::init<VkGraphicsPipelineCreateInfo> pp_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pp_info.layout = _layout;

    std::vector<VkShaderModule>                  modules;
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    for (auto* s : shaders) {
        assert(s->format() == shader_format::spirv);

        v1::vulkan::init<VkShaderModuleCreateInfo> smc{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        smc.codeSize = s->data().size();
        smc.pCode    = reinterpret_cast<const u32*>(s->data().data());
        vkCreateShaderModule(_device, &smc, nullptr, &modules.emplace_back());

        v1::vulkan::init<VkPipelineShaderStageCreateInfo> st{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        st.module = modules.back();
        st.pName  = "main";
        st.stage  = [&] {
            switch (s->stage())
            {
            case shader_type::vert: return VK_SHADER_STAGE_VERTEX_BIT;
            case shader_type::frag: return VK_SHADER_STAGE_FRAGMENT_BIT;
            case shader_type::tesc: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            case shader_type::tese: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            case shader_type::geom: return VK_SHADER_STAGE_GEOMETRY_BIT;
            case shader_type::comp:
                elog << "You should not attach a compute shader to a graphics pipeline!";
                return VK_SHADER_STAGE_COMPUTE_BIT;
            }
            return VK_SHADER_STAGE_VERTEX_BIT;
        }();
        stages.push_back(st);
    }
    pp_info.pStages    = stages.data();
    pp_info.stageCount = stages.size();


	v1::vulkan::init<VkRenderPassCreateInfo> rpc{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
	v1::vulkan::init<VkSubpassDescription> subpass;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<VkAttachmentDescription>      attachments;
    std::vector<VkAttachmentReference>        color_attachment_refs;
    std::vector<VkAttachmentReference>        resolve_attachment_refs;
    v1::vulkan::init<VkAttachmentDescription> att;
    att.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    att.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    att.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    v1::vulkan::init<VkAttachmentReference> att_ref;
    att_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (int i = 0; i < renderpass.color_attachment_formats().size(); ++i) {
        att.format  = v1::vulkan::get_format(renderpass.color_attachment_formats()[i].first);
        att.samples = VkSampleCountFlagBits(renderpass.samples());
        attachments.push_back(att);

        att_ref.attachment = attachments.size() - 1;
        color_attachment_refs.push_back(att_ref);

        if (u32(renderpass.samples()) > 1) {
            att.format  = v1::vulkan::get_format(renderpass.color_attachment_formats()[i].second);
            att.samples = VK_SAMPLE_COUNT_1_BIT;
            attachments.push_back(att);
            att_ref.attachment = attachments.size() - 1;
            resolve_attachment_refs.push_back(att_ref);
        }
    }

    att.format      = v1::vulkan::get_format(renderpass.depth_attachment_format());
    att.samples     = VkSampleCountFlagBits(renderpass.samples());
    att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments.push_back(att);

    att_ref.attachment           = attachments.size() - 1;
    att_ref.layout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    subpass.colorAttachmentCount = color_attachment_refs.size();
    subpass.pColorAttachments    = color_attachment_refs.data();
    if (u32(renderpass.samples()) > 1) subpass.pResolveAttachments = resolve_attachment_refs.data();
    subpass.pDepthStencilAttachment = &att_ref;

    rpc.attachmentCount = attachments.size();
    rpc.pAttachments    = attachments.data();
    rpc.subpassCount    = 1;
    rpc.pSubpasses      = &subpass;

    v1::vulkan::init<VkSubpassDependency> dep;
    dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dep.srcAccessMask   = 0;
    dep.srcStageMask    = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    dep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dep.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dep.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.dstSubpass      = 0;
    rpc.pDependencies   = &dep;
    rpc.dependencyCount = 1;

    movable_handle<VkRenderPass> renderpass_template;
    vkCreateRenderPass(_device, &rpc, nullptr, &renderpass_template);
    pp_info.renderPass = renderpass_template;

    v1::vulkan::init<VkPipelineVertexInputStateCreateInfo> vert_inp{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    std::vector<VkVertexInputAttributeDescription> attr_desc;
    std::vector<VkVertexInputBindingDescription>   bind_desc;
    if (state.state_vertex_input) {
        for (auto& d : state.state_vertex_input->attributes) {
            v1::vulkan::init<VkVertexInputAttributeDescription> attr;
            attr.location = d.location;
            attr.format   = v1::vulkan::get_format(d.fmt);
            attr.binding  = d.binding;
            attr.offset   = d.offset;
            attr_desc.push_back(attr);
        }
        for (auto& b : state.state_vertex_input->bindings) {
            v1::vulkan::init<VkVertexInputBindingDescription> bind;
            bind.binding   = b.binding;
            bind.inputRate = [&] {
                switch (b.rate)
                {
                case input_rate::vertex: return VK_VERTEX_INPUT_RATE_VERTEX;
                case input_rate::instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
                }
                return VK_VERTEX_INPUT_RATE_VERTEX;
            }();
            bind.stride = b.stride;
            bind_desc.push_back(bind);
        }
    }
    vert_inp.pVertexAttributeDescriptions    = attr_desc.data();
    vert_inp.vertexAttributeDescriptionCount = attr_desc.size();
    vert_inp.pVertexBindingDescriptions      = bind_desc.data();
    vert_inp.vertexBindingDescriptionCount   = bind_desc.size();
    pp_info.pVertexInputState                = &vert_inp;

    v1::vulkan::init<VkPipelineInputAssemblyStateCreateInfo> inp_ass{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

    static pipeline_state::input_assembly ia;
    pipeline_state::input_assembly*       iap = state.state_input_assembly ? state.state_input_assembly : &ia;
    inp_ass.primitiveRestartEnable            = iap->primitive_restart_enable;
    inp_ass.topology                          = [&]() {
        switch (iap->primitive_topology)
        {
        case topology::point_list: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case topology::line_list: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case topology::line_strip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case topology::triangle_list: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case topology::triangle_strip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case topology::triangle_fan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case topology::line_list_adj: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
        case topology::line_strip_adj: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
        case topology::triangle_list_adj: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case topology::triangle_strip_adj: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        case topology::patch_list: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        }
    }();
    pp_info.pInputAssemblyState = &inp_ass;

    static pipeline_state::rasterizer                        rst;
    pipeline_state::rasterizer*                              rstp = state.state_rasterizer ? state.state_rasterizer : &rst;
    v1::vulkan::init<VkPipelineRasterizationStateCreateInfo> rast{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rast.cullMode = [&] {
        switch (rstp->cull)
        {
        case cull_mode::none: return VK_CULL_MODE_NONE;
        case cull_mode::front: return VK_CULL_MODE_FRONT_BIT;
        case cull_mode::back: return VK_CULL_MODE_BACK_BIT;
        case cull_mode::front_and_back: return VK_CULL_MODE_FRONT_AND_BACK;
        }
        return VK_CULL_MODE_NONE;
    }();
    rast.depthBiasClamp          = rstp->depth_bias_clamp;
    rast.depthBiasConstantFactor = rstp->depth_bias_constant_factor;
    rast.depthBiasEnable         = rstp->depth_bias_enable;
    rast.depthBiasSlopeFactor    = rstp->depth_bias_slope_factor;
    rast.depthClampEnable        = rstp->depth_clamp_enable;
    rast.lineWidth               = rstp->line_width;
    rast.rasterizerDiscardEnable = rstp->rasterizer_discard_enable;
    rast.polygonMode             = [&] {
        switch (rstp->polygon_mode)
        {
        case poly_mode::fill: return VK_POLYGON_MODE_FILL;
        case poly_mode::line: return VK_POLYGON_MODE_LINE;
        case poly_mode::point: return VK_POLYGON_MODE_POINT;
        }
        return VK_POLYGON_MODE_FILL;
    }();
    rast.frontFace = [&] {
        switch (rstp->front_face)
        {
        case orientation::cw: return VK_FRONT_FACE_CLOCKWISE;
        case orientation::ccw: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }();
    pp_info.pRasterizationState = &rast;

    static pipeline_state::multisample                     mul;
    pipeline_state::multisample*                           mulp = state.state_multisample ? state.state_multisample : &mul;
    v1::vulkan::init<VkPipelineMultisampleStateCreateInfo> msaa{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    msaa.alphaToCoverageEnable = mulp->alpha_to_coverage_enable;
    msaa.alphaToOneEnable      = mulp->alpha_to_one_enable;
    msaa.minSampleShading      = mulp->min_sample_shading;
    msaa.pSampleMask           = mulp->sample_masks;
    msaa.rasterizationSamples  = VkSampleCountFlagBits(mulp->samples);
    msaa.sampleShadingEnable   = mulp->sample_shading_enable;
    pp_info.pMultisampleState  = &msaa;

    static pipeline_state::tesselation                      tes;
    pipeline_state::tesselation*                            tesp = state.state_tesselation ? state.state_tesselation : &tes;
    v1::vulkan::init<VkPipelineTessellationStateCreateInfo> tess{VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
    tess.patchControlPoints    = tesp->patch_control_points;
    pp_info.pTessellationState = &tess;

    std::vector<VkDynamicState> dyn_states;

    static pipeline_state::render_area                  vp;
    pipeline_state::render_area*                        vpp = state.state_viewports ? state.state_viewports : &vp;
    v1::vulkan::init<VkPipelineViewportStateCreateInfo> vps{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    std::vector<VkRect2D>                               scissors;
    std::vector<VkViewport>                             viewports;
    if (vpp->scissors.empty()) {
        vps.scissorCount = 1;
        dyn_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
    }
    else
    {
        for (auto& sc : vpp->scissors) {
            VkRect2D&  s    = scissors.emplace_back();
            const auto sz   = sc.size();
            s.extent.width  = sz.x;
            s.extent.height = sz.y;
            s.offset.x      = sc.min.x;
            s.offset.y      = sc.min.y;
        }
        vps.scissorCount = scissors.size();
        vps.pScissors    = scissors.data();
    }
    if (vpp->viewports.empty()) {
        vps.viewportCount = 1;
        dyn_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    }
    else
    {
        for (auto& sc : vpp->viewports) {
            VkViewport& v = viewports.emplace_back();
            v.x           = sc.x;
            v.y           = sc.y;
            v.width       = sc.width;
            v.height      = sc.height;
            v.minDepth    = sc.min_depth;
            v.maxDepth    = sc.max_depth;
        }
        vps.viewportCount = viewports.size();
        vps.pViewports    = viewports.data();
    }
    pp_info.pViewportState = &vps;

    v1::vulkan::init<VkPipelineDynamicStateCreateInfo> dyn{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dyn.dynamicStateCount = dyn_states.size();
    dyn.pDynamicStates    = dyn_states.data();
    pp_info.pDynamicState = &dyn;

    static pipeline_state::depth_stencil                    ds;
    pipeline_state::depth_stencil*                          dsp = state.state_depth_stencil ? state.state_depth_stencil : &ds;
    v1::vulkan::init<VkPipelineDepthStencilStateCreateInfo> depsten{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depsten.depthBoundsTestEnable = dsp->depth_bounds_test_enable;
    depsten.depthTestEnable       = dsp->depth_test_enable;
    depsten.depthWriteEnable      = dsp->depth_write_enable;
    depsten.minDepthBounds        = dsp->depth_bounds_min;
    depsten.maxDepthBounds        = dsp->depth_bounds_max;
    depsten.stencilTestEnable     = dsp->stencil_test_enable;

    const auto mk_op = [](auto op) {
        switch (op)
        {
        case compare_op::never: return VK_COMPARE_OP_NEVER;
        case compare_op::less: return VK_COMPARE_OP_LESS;
        case compare_op::equal: return VK_COMPARE_OP_EQUAL;
        case compare_op::less_or_equal: return VK_COMPARE_OP_LESS_OR_EQUAL;
        case compare_op::greater: return VK_COMPARE_OP_GREATER;
        case compare_op::not_equal: return VK_COMPARE_OP_NOT_EQUAL;
        case compare_op::greater_or_equal: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case compare_op::always: return VK_COMPARE_OP_ALWAYS;
        }
        return VK_COMPARE_OP_NEVER;
    };

    const auto mk_st_op = [](auto sop) {
        switch (sop)
        {
        case stencil_op::keep: return VK_STENCIL_OP_KEEP;
        case stencil_op::zero: return VK_STENCIL_OP_ZERO;
        case stencil_op::replace: return VK_STENCIL_OP_REPLACE;
        case stencil_op::inc_clamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case stencil_op::dec_clamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case stencil_op::inv: return VK_STENCIL_OP_INVERT;
        case stencil_op::inc_wrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case stencil_op::dec_wrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        }
        return VK_STENCIL_OP_KEEP;
    };

    depsten.depthCompareOp = mk_op(dsp->depth_compare);

    const auto init_state = [&](VkStencilOpState& out, const pipeline_state::stencil_state& in) {
        out.reference   = in.reference;
        out.writeMask   = in.writeMask;
        out.compareOp   = mk_op(in.compareOp);
        out.depthFailOp = mk_st_op(in.depthFailOp);
        out.failOp      = mk_st_op(in.failOp);
        out.passOp      = mk_st_op(in.passOp);
        out.compareMask = in.compareMask;
    };

    init_state(depsten.front, dsp->front);
    init_state(depsten.back, dsp->back);
    pp_info.pDepthStencilState = &depsten;

    static pipeline_state::blending                       bl;
    pipeline_state::blending*                             blp = state.state_blending ? state.state_blending : &bl;
    v1::vulkan::init<VkPipelineColorBlendStateCreateInfo> bls{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    memcpy(bls.blendConstants, blp->blend_constants, sizeof(bls.blendConstants));
    bls.logicOpEnable = blp->logic_op_enable;
    bls.logicOp       = [&]() {
        switch (blp->op)
        {
        case logic_op::op_clear: return VK_LOGIC_OP_CLEAR;
        case logic_op::op_and: return VK_LOGIC_OP_AND;
        case logic_op::op_and_rev: return VK_LOGIC_OP_AND_REVERSE;
        case logic_op::op_copy: return VK_LOGIC_OP_COPY;
        case logic_op::op_and_inv: return VK_LOGIC_OP_AND_INVERTED;
        case logic_op::op_none: return VK_LOGIC_OP_NO_OP;
        case logic_op::op_xor: return VK_LOGIC_OP_XOR;
        case logic_op::op_or: return VK_LOGIC_OP_OR;
        case logic_op::op_nor: return VK_LOGIC_OP_NOR;
        case logic_op::op_eqiv: return VK_LOGIC_OP_EQUIVALENT;
        case logic_op::op_inv: return VK_LOGIC_OP_INVERT;
        case logic_op::op_or_rev: return VK_LOGIC_OP_OR_REVERSE;
        case logic_op::op_copy_inv: return VK_LOGIC_OP_COPY_INVERTED;
        case logic_op::op_or_inv: return VK_LOGIC_OP_OR_INVERTED;
        case logic_op::op_nand: return VK_LOGIC_OP_NAND;
        case logic_op::op_set: return VK_LOGIC_OP_SET;
        }
        return VK_LOGIC_OP_NO_OP;
    }();

    const auto mk_fac = [](auto f) {
        switch (f)
        {
        case blend_factor::zero: return VK_BLEND_FACTOR_ZERO;
        case blend_factor::one: return VK_BLEND_FACTOR_ONE;
        case blend_factor::src_color: return VK_BLEND_FACTOR_SRC_COLOR;
        case blend_factor::one_minus_src_color: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case blend_factor::dst_color: return VK_BLEND_FACTOR_DST_COLOR;
        case blend_factor::one_minus_dst_color: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case blend_factor::src_alpha: return VK_BLEND_FACTOR_SRC_ALPHA;
        case blend_factor::one_minus_src_alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case blend_factor::dst_alpha: return VK_BLEND_FACTOR_DST_ALPHA;
        case blend_factor::one_minus_dst_alpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case blend_factor::constant_color: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case blend_factor::one_minus_constant_color: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case blend_factor::constant_alpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case blend_factor::one_minus_constant_alpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case blend_factor::src_alpha_saturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case blend_factor::src1_color: return VK_BLEND_FACTOR_SRC1_COLOR;
        case blend_factor::one_minus_src1_color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case blend_factor::src1_alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
        case blend_factor::one_minus_src1_alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }
        return VK_BLEND_FACTOR_ONE;
    };
    const auto mk_blob = [](auto op) {
        switch (op)
        {
        case blend_op::op_add: return VK_BLEND_OP_ADD;
        case blend_op::op_sub: return VK_BLEND_OP_SUBTRACT;
        case blend_op::op_sub_rev: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case blend_op::op_min: return VK_BLEND_OP_MIN;
        case blend_op::op_max: return VK_BLEND_OP_MAX;
        case blend_op::op_zero: return VK_BLEND_OP_ZERO_EXT;
        case blend_op::op_src: return VK_BLEND_OP_SRC_EXT;
        case blend_op::op_dst: return VK_BLEND_OP_DST_EXT;
        case blend_op::op_src_over: return VK_BLEND_OP_SRC_OVER_EXT;
        case blend_op::op_dst_out: return VK_BLEND_OP_DST_OUT_EXT;
        case blend_op::op_src_atop: return VK_BLEND_OP_SRC_ATOP_EXT;
        case blend_op::op_dst_atop: return VK_BLEND_OP_DST_ATOP_EXT;
        case blend_op::op_xor: return VK_BLEND_OP_XOR_EXT;
        case blend_op::op_mul: return VK_BLEND_OP_MULTIPLY_EXT;
        case blend_op::op_screen: return VK_BLEND_OP_SCREEN_EXT;
        case blend_op::op_overlay: return VK_BLEND_OP_OVERLAY_EXT;
        case blend_op::op_darken: return VK_BLEND_OP_DARKEN_EXT;
        case blend_op::op_lighten: return VK_BLEND_OP_LIGHTEN_EXT;
        case blend_op::op_color_dodge: return VK_BLEND_OP_COLORDODGE_EXT;
        case blend_op::op_color_burn: return VK_BLEND_OP_COLORBURN_EXT;
        case blend_op::op_hard_light: return VK_BLEND_OP_HARDLIGHT_EXT;
        case blend_op::op_soft_light: return VK_BLEND_OP_SOFTLIGHT_EXT;
        case blend_op::op_diff: return VK_BLEND_OP_DIFFERENCE_EXT;
        case blend_op::op_exclusion: return VK_BLEND_OP_EXCLUSION_EXT;
        case blend_op::op_invert: return VK_BLEND_OP_INVERT_EXT;
        case blend_op::op_linear_burn: return VK_BLEND_OP_LINEARBURN_EXT;
        case blend_op::op_vivid_light: return VK_BLEND_OP_VIVIDLIGHT_EXT;
        case blend_op::op_linear_light: return VK_BLEND_OP_LINEARLIGHT_EXT;
        case blend_op::op_pin_light: return VK_BLEND_OP_PINLIGHT_EXT;
        case blend_op::op_hard_mix: return VK_BLEND_OP_HARDMIX_EXT;
        case blend_op::op_hsl_hue: return VK_BLEND_OP_HSL_HUE_EXT;
        case blend_op::op_hsl_saturation: return VK_BLEND_OP_HSL_SATURATION_EXT;
        case blend_op::op_hsl_color: return VK_BLEND_OP_HSL_COLOR_EXT;
        case blend_op::op_hsl_luminosity: return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
        case blend_op::op_plus: return VK_BLEND_OP_PLUS_EXT;
        case blend_op::op_plus_clamped: return VK_BLEND_OP_PLUS_CLAMPED_EXT;
        case blend_op::op_plus_clamped_alpha: return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
        case blend_op::op_plus_darker: return VK_BLEND_OP_PLUS_DARKER_EXT;
        case blend_op::op_minus: return VK_BLEND_OP_MINUS_EXT;
        case blend_op::op_minus_clamped: return VK_BLEND_OP_MINUS_CLAMPED_EXT;
        case blend_op::op_contrast: return VK_BLEND_OP_CONTRAST_EXT;
        case blend_op::op_invert_ovg: return VK_BLEND_OP_INVERT_EXT;
        case blend_op::op_red: return VK_BLEND_OP_RED_EXT;
        case blend_op::op_green: return VK_BLEND_OP_GREEN_EXT;
        case blend_op::op_blue: return VK_BLEND_OP_BLUE_EXT;
        }
        return VK_BLEND_OP_SRC_EXT;
    };

    std::vector<VkPipelineColorBlendAttachmentState> blatt;
    for (auto& a : blp->attachments) {
        v1::vulkan::init<VkPipelineColorBlendAttachmentState> t;
        t.blendEnable = a.blendEnable;

        t.srcColorBlendFactor = mk_fac(a.srcColorBlendFactor);
        t.srcAlphaBlendFactor = mk_fac(a.srcAlphaBlendFactor);
        t.dstColorBlendFactor = mk_fac(a.dstColorBlendFactor);
        t.dstAlphaBlendFactor = mk_fac(a.dstAlphaBlendFactor);
        t.colorBlendOp        = mk_blob(a.colorBlendOp);
        t.alphaBlendOp        = mk_blob(a.alphaBlendOp);

        blatt.emplace_back(t);
    }

    if (blatt.size() < renderpass.color_attachment_formats().size()) {
        for (int i = blatt.size(); i < (renderpass.color_attachment_formats().size() - blatt.size()); ++i) {
            blatt.emplace_back().blendEnable = false;
        }
    }
    bls.attachmentCount = blatt.size();
    bls.pAttachments    = blatt.data();

    pp_info.pColorBlendState = &bls;
    vkCreateGraphicsPipelines(_device, nullptr, 1, &pp_info, nullptr, &_pipeline);
    vkDestroyRenderPass(_device, renderpass_template, nullptr);

    for (auto& psm : modules) {
        vkDestroyShaderModule(_device, psm, nullptr);
    }
}

handle graphics_pipeline_implementation::api_handle()
{
    return VkPipeline(_pipeline);
}

compute_pipeline_implementation::~compute_pipeline_implementation()
{
    if (_pipeline) vkDestroyPipeline(_device, _pipeline, nullptr);
    if (_layout) vkDestroyPipelineLayout(_device, _layout, nullptr);
}

void compute_pipeline_implementation::initialize(const pipeline_state::layout& layout, const v1::shader& cs)
{
    auto& ctx = context::current();
    auto impl = static_cast<v1::vulkan::context_implementation*>(std::any_cast<v1::detail::context_implementation*>(ctx->implementation()));
    _device   = impl->device();

    v1::vulkan::init<VkPipelineLayoutCreateInfo> pl_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    std::vector<VkDescriptorSetLayout> layouts;
    for (auto& bl : layout.binding_layouts) layouts.push_back(handle_cast<VkDescriptorSetLayout>(*bl));
    pl_info.setLayoutCount = layouts.size();
    pl_info.pSetLayouts    = layouts.data();
    vkCreatePipelineLayout(_device, &pl_info, nullptr, &_layout);

    v1::vulkan::init<VkComputePipelineCreateInfo> pp_info{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    pp_info.layout      = _layout;
    pp_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    v1::vulkan::init<VkShaderModuleCreateInfo> smc{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    smc.codeSize = cs.data().size();
    smc.pCode    = reinterpret_cast<const u32*>(cs.data().data());
    vkCreateShaderModule(_device, &smc, nullptr, &pp_info.stage.module);
    pp_info.stage.pName = "main";
    pp_info.stage.stage = [&] {
        switch (cs.stage())
        {
        case shader_type::vert:
            elog << "You should not attach a graphics shader to a graphics pipeline!";
            return VK_SHADER_STAGE_VERTEX_BIT;
        case shader_type::frag:
            elog << "You should not attach a graphics shader to a graphics pipeline!";
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case shader_type::tesc:
            elog << "You should not attach a graphics shader to a graphics pipeline!";
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case shader_type::tese:
            elog << "You should not attach a graphics shader to a graphics pipeline!";
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case shader_type::geom:
            elog << "You should not attach a graphics shader to a graphics pipeline!";
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case shader_type::comp: return VK_SHADER_STAGE_COMPUTE_BIT;
        }
        elog << "You should not attach a graphics shader to a graphics pipeline!";
        return VK_SHADER_STAGE_VERTEX_BIT;
    }();

    vkCreateComputePipelines(_device, nullptr, 1, &pp_info, nullptr, &_pipeline);

    vkDestroyShaderModule(_device, pp_info.stage.module, nullptr);
}

handle compute_pipeline_implementation::api_handle()
{
    return VkPipeline(_pipeline);
}
}    // namespace vulkan
}    // namespace v2
}    // namespace gfx