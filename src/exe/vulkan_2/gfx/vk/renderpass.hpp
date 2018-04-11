#pragma once

#include "array_view.hpp"

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace gfx::vk
{
    class device;
    class renderpass;
    class framebuffer;

    struct attachment_reference
    {
        constexpr attachment_reference(uint32_t index, VkImageLayout layout) : index(index), layout(layout) {}
        constexpr operator VkAttachmentReference() const noexcept { return { index, layout }; }

        uint32_t index;
        VkImageLayout layout;
    };

    class subpass_info
    {
    public:
        friend renderpass;
        subpass_info(VkPipelineBindPoint bind_point, VkSubpassDescriptionFlags flags,
            array_view<attachment_reference> color_references,
            std::optional<array_view<attachment_reference>> input_references ={},
            std::optional<array_view<attachment_reference>> resolve_references ={},
            std::optional<attachment_reference> depth_reference ={},
            std::optional<array_view<uint32_t>> preserve_attachments ={});

        operator VkSubpassDescription() const noexcept;

    private:
        VkPipelineBindPoint _bind_point;
        VkSubpassDescriptionFlags _flags;
        std::vector<attachment_reference> _color_references;
        std::vector<attachment_reference> _input_references;
        std::vector<attachment_reference> _resolve_references;
        std::optional<attachment_reference> _depth_reference;
        std::vector<uint32_t> _preserve_attachments;
    };

    class subpass_dependencies
    {
    public:
        friend renderpass;
        subpass_dependencies(uint32_t default_subpass_index = VK_SUBPASS_EXTERNAL, VkPipelineStageFlags default_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkAccessFlags default_access_mask = VK_ACCESS_MEMORY_READ_BIT);
        subpass_dependencies& to(uint32_t subpass_index, VkDependencyFlags dependency_flags, VkPipelineStageFlags stage_mask, VkAccessFlags access_mask);
        subpass_dependencies& finish(VkDependencyFlags dependency_flags, VkPipelineStageFlags stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    private:
        bool _finished = false;
        std::vector<VkSubpassDependency> _dependencies;
        uint32_t _default_subpass_index = VK_SUBPASS_EXTERNAL;
        VkPipelineStageFlags _default_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        VkAccessFlags _default_access_mask = VK_ACCESS_MEMORY_READ_BIT;
    };

    class attachment_description
    {
    public:
        attachment_description(VkAttachmentDescriptionFlags flags, VkFormat format, VkSampleCountFlagBits samples);
        attachment_description& on_load(VkImageLayout layout, VkAttachmentLoadOp op = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentLoadOp stencil_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE);
        attachment_description& on_store(VkImageLayout layout, VkAttachmentStoreOp op = VK_ATTACHMENT_STORE_OP_DONT_CARE, VkAttachmentStoreOp stencil_op = VK_ATTACHMENT_STORE_OP_DONT_CARE);

        operator const VkAttachmentDescription&() const noexcept;
    private:
        VkAttachmentDescription _description;
        uint32_t _complete_check = 0;
    };

    class renderpass
    {
    public:
        friend framebuffer;
        renderpass(const std::shared_ptr<device>& device, array_view<attachment_description> attachments, array_view<subpass_info> subpasses, const subpass_dependencies& dependencies);
        ~renderpass();
        renderpass(const renderpass&) = delete;
        renderpass& operator=(const renderpass&) = delete;
        renderpass(renderpass&&) = default;
        renderpass& operator=(renderpass&&) = default;

        operator VkRenderPass() const noexcept;

    private:
        std::shared_ptr<device> _device;
        VkRenderPass _renderpass;
    };
}