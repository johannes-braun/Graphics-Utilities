#pragma once

#include "../framebuffer.hpp"
#include "init_struct.hpp"
#include "../general/handle.hpp"

namespace gfx
{
    inline namespace v1
    {
        namespace vulkan
        {
			class framebuffer_implementation : public detail::framebuffer_implementation
			{
			public:
			        ~framebuffer_implementation() override;
			    void create(u32 width, u32 height, u32 layers, const v2::renderpass_layout& layout) override;
			    void attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear) override;
			    void detach(attachment att, u32 index) override;
			    handle api_handle() override;

				const VkRenderPassBeginInfo& begin_info();

			private:
				bool _invalid = true;
				VkDevice _device;
				movable_handle<VkFramebuffer> _fbo;
				movable_handle<VkRenderPass> _pass;
				init<VkFramebufferCreateInfo> _fbcreate{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
				init<VkRenderPassCreateInfo> _rpcreate{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
				std::vector<VkImageView> _attachments;
				init<VkSubpassDescription> _subpass;
				init<VkSubpassDependency> _subpass_dep;
				std::vector<VkAttachmentDescription>      _attachment_descriptions;
				std::vector<VkAttachmentReference>        _color_attachment_refs;
				std::vector<VkAttachmentReference>        _resolve_attachment_refs;
				VkAttachmentReference _depth_stencil_attachment_ref;
				init<VkRenderPassBeginInfo> _begin_info{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
				std::vector<std::optional<clear_value>> _color_clear_values;
				std::optional<clear_value>              _depth_clear_value;
			};
        }
    }
}