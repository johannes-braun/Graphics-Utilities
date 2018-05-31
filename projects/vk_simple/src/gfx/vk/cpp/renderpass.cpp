#include "../renderpass.hpp"
#include "../device.hpp"

#include <cassert>

namespace vk
{
    subpass_info::subpass_info(VkPipelineBindPoint bind_point, VkSubpassDescriptionFlags flags,
        array_view<attachment_reference> color_references,
        std::optional<array_view<attachment_reference>> input_references,
        std::optional<array_view<attachment_reference>> resolve_references,
        std::optional<attachment_reference> depth_reference,
        std::optional<array_view<uint32_t>> preserve_attachments)
        : _bind_point(bind_point), _flags(flags), _depth_reference(depth_reference)
    {
        assert(color_references.size() == (resolve_references ? resolve_references->size() : color_references.size()));
        _color_references       = { color_references.begin(), color_references.end() };
        _resolve_references     = resolve_references.has_value() ? std::vector<attachment_reference>{resolve_references->begin(), resolve_references->end()} : std::vector<attachment_reference>{};
        _input_references       = input_references.has_value() ? std::vector<attachment_reference>{input_references->begin(), input_references->end()} : std::vector<attachment_reference>{};
        _preserve_attachments   = preserve_attachments.has_value() ? std::vector<uint32_t>{preserve_attachments->begin(), preserve_attachments->end()} : std::vector<uint32_t>{};
    }

    subpass_info::operator VkSubpassDescription() const noexcept
    {
        VkSubpassDescription desc;
        desc.flags              = _flags;
        desc.pipelineBindPoint  = _bind_point;

        desc.colorAttachmentCount       = uint32_t(_color_references.size());
        desc.inputAttachmentCount       = uint32_t(_input_references.size());
        desc.preserveAttachmentCount    = uint32_t(_preserve_attachments.size());

        desc.pColorAttachments          = reinterpret_cast<const VkAttachmentReference*>(_color_references.data());
        desc.pResolveAttachments        = reinterpret_cast<const VkAttachmentReference*>(_resolve_references.data());
        desc.pInputAttachments          = reinterpret_cast<const VkAttachmentReference*>(_input_references.data());
        desc.pDepthStencilAttachment    = _depth_reference ? reinterpret_cast<const VkAttachmentReference*>(&_depth_reference) : nullptr;
        desc.pPreserveAttachments       = _preserve_attachments.data();
        return desc;
    }

    subpass_dependencies::subpass_dependencies(uint32_t default_subpass_index, VkPipelineStageFlags default_stage_mask, VkAccessFlags default_access_mask)
        : _default_subpass_index(default_subpass_index), _default_stage_mask(default_stage_mask), _default_access_mask(default_access_mask)
    {

    }

    subpass_dependencies& subpass_dependencies::to(uint32_t subpass_index, VkDependencyFlags dependency_flags, VkPipelineStageFlags stage_mask, VkAccessFlags access_mask)
    {
        assert(!_finished);
        if (_dependencies.empty())
        {
            VkSubpassDependency& dep = _dependencies.emplace_back();
            dep.srcAccessMask = _default_access_mask;
            dep.srcStageMask = _default_stage_mask;
            dep.srcSubpass = _default_subpass_index;

            dep.dstSubpass = subpass_index;
            dep.dstStageMask = stage_mask;
            dep.dstAccessMask = access_mask;
            dep.dependencyFlags = dependency_flags;
        }
        else
        {
            VkSubpassDependency& dep = _dependencies.emplace_back();
            VkSubpassDependency& prev = _dependencies[_dependencies.size()-2];
            dep.srcAccessMask = prev.dstAccessMask;
            dep.srcStageMask = prev.dstStageMask;
            dep.srcSubpass = prev.dstSubpass;

            dep.dstSubpass = subpass_index;
            dep.dstStageMask = stage_mask;
            dep.dstAccessMask = access_mask;
            dep.dependencyFlags = dependency_flags;
        }
        return *this;
    }

    subpass_dependencies& subpass_dependencies::finish(VkDependencyFlags dependency_flags, VkPipelineStageFlags stage_mask)
    {
        to(_default_subpass_index, stage_mask, _default_access_mask, dependency_flags);
        _finished = true;
        return *this;
    }

    attachment_description::attachment_description(VkAttachmentDescriptionFlags flags, VkFormat format, VkSampleCountFlagBits samples)
        : _description()
    {
        _description.flags = flags;
        _description.format = format;
        _description.samples = samples;
        _complete_check = 0b1;
    }

    attachment_description& attachment_description::on_load(VkImageLayout layout, VkAttachmentLoadOp op, VkAttachmentLoadOp stencil_op)
    {
        _description.initialLayout = layout;
        _description.loadOp = op;
        _description.stencilLoadOp = stencil_op;
        _complete_check |= 0b10;
        return *this;
    }

    attachment_description& attachment_description::on_store(VkImageLayout layout, VkAttachmentStoreOp op, VkAttachmentStoreOp stencil_op)
    {
        _description.finalLayout = layout;
        _description.storeOp = op;
        _description.stencilStoreOp = stencil_op;
        _complete_check |= 0b100;
        return *this;
    }

    attachment_description::operator const VkAttachmentDescription&() const noexcept
    {
        assert((_complete_check & 7) == 7 && "Attachment info incomplete.");
        return _description;
    }

    renderpass::renderpass(const std::shared_ptr<device>& device, array_view<attachment_description> attachments, array_view<subpass_info> subpasses, const subpass_dependencies& dependencies)
        : _device(device)
    {
        std::vector<VkSubpassDescription> sub(subpasses.size());
        for (int i=0; i<sub.size(); ++i) sub[i] = subpasses.data()[i];
        std::vector<VkAttachmentDescription> att(attachments.size());
        for (int i=0; i<att.size(); ++i) att[i] = attachments.data()[i];

        VkRenderPassCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.subpassCount = uint32_t(sub.size());
        info.pSubpasses = sub.data();
        info.dependencyCount = uint32_t(dependencies._dependencies.size());
        info.pDependencies = dependencies._dependencies.data();
        info.attachmentCount = uint32_t(att.size());
        info.pAttachments = att.data();

        vkCreateRenderPass(*_device, &info, nullptr, &_renderpass);
    }

    renderpass::~renderpass()
    {
        vkDestroyRenderPass(*_device, _renderpass, nullptr);
    }

    renderpass::operator VkRenderPass() const noexcept
    {
        return _renderpass;
    }
}