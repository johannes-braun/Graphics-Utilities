#include "framebuffer_vulkan.hpp"
#include "context_vulkan.hpp"
#include "device_image_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

framebuffer_implementation::~framebuffer_implementation()
{
    if (_fbo) vkDestroyFramebuffer(_device, _fbo, nullptr);
    if (_pass) vkDestroyRenderPass(_device, _pass, nullptr);
}

void framebuffer_implementation::create(u32 width, u32 height, u32 layers, const renderpass_layout& layout)
{
    auto& ctx  = context::current();
    auto  impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
    _device    = impl->device();

    _fbcreate.width  = width;
    _fbcreate.height = height;
    _fbcreate.layers = layers;

    _subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    init<VkAttachmentDescription> att;
    att.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    att.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    att.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    init<VkAttachmentReference> att_ref;
    att_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (int i = 0; i < layout.color_attachment_formats().size(); ++i)
    {
        att.format  = get_format(layout.color_attachment_formats()[i].first);
        att.samples = VkSampleCountFlagBits(layout.samples());
        _attachment_descriptions.push_back(att);

        att_ref.attachment = _attachment_descriptions.size() - 1;
        _color_attachment_refs.push_back(att_ref);

        if (u32(layout.samples()) > 1)
        {
            att.format  = v1::vulkan::get_format(layout.color_attachment_formats()[i].second);
            att.samples = VK_SAMPLE_COUNT_1_BIT;
            _attachment_descriptions.push_back(att);
            att_ref.attachment = _attachment_descriptions.size() - 1;
            _resolve_attachment_refs.push_back(att_ref);
        }
    }

    if (layout.depth_attachment_format() != format::unspecified)
    {
        att.format      = v1::vulkan::get_format(layout.depth_attachment_format());
        att.samples     = VkSampleCountFlagBits(layout.samples());
        att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        _attachment_descriptions.push_back(att);
    }

    att_ref.attachment            = _attachment_descriptions.size() - 1;
    att_ref.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    _depth_stencil_attachment_ref = att_ref;
    _subpass.colorAttachmentCount = _color_attachment_refs.size();
    _subpass.pColorAttachments    = _color_attachment_refs.data();
    if (u32(layout.samples()) > 1) _subpass.pResolveAttachments = _resolve_attachment_refs.data();
    if (layout.depth_attachment_format() != format::unspecified) { _subpass.pDepthStencilAttachment = &_depth_stencil_attachment_ref; }

    _rpcreate.attachmentCount = _attachment_descriptions.size();
    _rpcreate.pAttachments    = _attachment_descriptions.data();
    _rpcreate.subpassCount    = 1;
    _rpcreate.pSubpasses      = &_subpass;

    _subpass_dep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    _subpass_dep.dstSubpass      = 0;
    _subpass_dep.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    _subpass_dep.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    _subpass_dep.srcAccessMask   = 0;
    _subpass_dep.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    _subpass_dep.dependencyFlags = 0;
    _rpcreate.pDependencies      = &_subpass_dep;
    _rpcreate.dependencyCount    = 1;
}

void framebuffer_implementation::attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear)
{
    switch (att)
    {
    case attachment::color:
    {
        const u32 iv_id = _color_attachment_refs[index].attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _clear_values.resize(std::max(iv_id + 1, u32(_clear_values.size())));
        _attachments[iv_id] = handle_cast<VkImageView>(img_view);

        if (clear)
        {
            if (std::holds_alternative<load>(*clear))
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            else
            {
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                glm::vec4& cr                          = std::get<glm::vec4>(*clear);
                _clear_values[iv_id].color             = VkClearColorValue {cr.r, cr.g, cr.b, cr.a};
            }
        }

        _color_clear_values.resize(std::max<size_t>(_color_clear_values.size(), index + 1));
        _color_clear_values[index] = clear;
    }
    break;
    case attachment::resolve:
    {
        const u32 iv_id = _resolve_attachment_refs[index].attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _clear_values.resize(std::max(iv_id + 1, u32(_clear_values.size())));
        _attachments[iv_id] = handle_cast<VkImageView>(img_view);

        if (clear)
        {
            if (std::holds_alternative<load>(*clear))
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            else
            {
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                glm::vec4& cr                          = std::get<glm::vec4>(*clear);
                _clear_values[iv_id].color             = VkClearColorValue {cr.r, cr.g, cr.b, cr.a};
            }
        }
    }
    break;
    case attachment::depth_stencil:
    {
        const u32 iv_id = _depth_stencil_attachment_ref.attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _clear_values.resize(std::max(iv_id + 1, u32(_clear_values.size())));
        _attachments[iv_id] = handle_cast<VkImageView>(img_view);

        _depth_clear_value = clear;

        if (clear)
        {
            if (std::holds_alternative<load>(*clear))
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            else
            {
                _attachment_descriptions[iv_id].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depth_stencil& dsr                     = std::get<depth_stencil>(*clear);
                _clear_values[iv_id].depthStencil      = VkClearDepthStencilValue {dsr.depth, dsr.stencil};
            }
        }
    }
    break;
    default:;
    }
}

void framebuffer_implementation::detach(attachment att, u32 index)
{
    switch (att)
    {
    case attachment::color:
    {
        const u32 iv_id = _color_attachment_refs[index].attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _attachments[iv_id]        = nullptr;
        _color_clear_values[index] = {};
        _clear_values[iv_id]       = {};
    }
    break;
    case attachment::resolve:
    {
        const u32 iv_id = _resolve_attachment_refs[index].attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _attachments[iv_id] = nullptr;
    }
    break;
    case attachment::depth_stencil:
    {
        const u32 iv_id = _depth_stencil_attachment_ref.attachment;
        _attachments.resize(std::max(iv_id + 1, u32(_attachments.size())));
        _attachments[iv_id]  = nullptr;
        _depth_clear_value   = {};
        _clear_values[iv_id] = {};
    }
    break;
    }
}

handle framebuffer_implementation::api_handle()
{
    return VkFramebuffer(_fbo);
}

const VkRenderPassBeginInfo& framebuffer_implementation::begin_info()
{
    if (_invalid)
    {
        _invalid = false;
        check_result(vkCreateRenderPass(_device, &_rpcreate, nullptr, &_pass));
        _fbcreate.renderPass      = _pass;
        _fbcreate.attachmentCount = _attachments.size();
        _fbcreate.pAttachments    = _attachments.data();
        check_result(vkCreateFramebuffer(_device, &_fbcreate, nullptr, &_fbo));
        _begin_info.renderPass      = _pass;
        _begin_info.framebuffer     = _fbo;
        _begin_info.clearValueCount = u32(_clear_values.size());
        _begin_info.pClearValues    = _clear_values.data();
    }
    return _begin_info;
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx
