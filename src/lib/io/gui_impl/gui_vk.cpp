#include "gui_vk.hpp"
#include "vulkan/logical_device.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/shader.hpp"
#include "vulkan/command_execute.hpp"
#include "glm/detail/type_vec2.hpp"

namespace io::impl
{
    gui_vk::gui_vk(vkn::LogicalDevice* device, vkn::Swapchain* swapchain)
        : _device(device), _swapchain(swapchain)
    {
        _device->inc_ref();
        _swapchain->inc_ref();

        const auto color_resolve_attachment = vk::AttachmentDescription().setFormat(vk::Format::eB8G8R8A8Unorm)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference simple_color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
        vk::SubpassDescription simple_subpass;
        simple_subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&simple_color_reference);

        const auto simple_dependency = vk::SubpassDependency().setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
            .setDependencyFlags(vk::DependencyFlagBits::eByRegion);

        const auto simple_renderpass_attachments = { color_resolve_attachment };
        vk::RenderPassCreateInfo simple_render_pass_info;
        simple_render_pass_info.setAttachmentCount(static_cast<uint32_t>(std::size(simple_renderpass_attachments)))
            .setPAttachments(std::data(simple_renderpass_attachments))
            .setSubpassCount(1)
            .setPSubpasses(&simple_subpass)
            .setDependencyCount(1)
            .setPDependencies(&simple_dependency);
        m_renderpass = _device->createRenderPass(simple_render_pass_info);

        update_swapchain(_swapchain);
        create_device_objects();
    }

    gui_vk::~gui_vk()
    {
        for (auto&& info : m_frame_infos)
        {
            if (info.index_buffer) _device->destroyBuffer(info.index_buffer);
            if (info.vertex_buffer) _device->destroyBuffer(info.vertex_buffer);
            if (info.buffer_memory) _device->memory()->free(info.buffer_memory);
        }

        _device->destroyImage(m_font_image);
        _device->memory()->free(m_font_image_memory);
        _device->destroyImageView(m_font_image_view);

        _device->destroySampler(m_font_sampler);
        _device->destroyDescriptorSetLayout(m_descriptor_set_layout);
        _device->destroyPipelineLayout(m_pipeline_layout);
        _device->destroyPipeline(m_pipeline);

        for (auto&& fbo : m_framebuffers) _device->destroyFramebuffer(fbo);
        _device->destroyRenderPass(m_renderpass);

        _swapchain->dec_ref();
        _device->dec_ref();
    }

    ImTextureID gui_vk::build_font_atlas()
    {
        _device->oneTimeCommand(vk::QueueFlagBits::eTransfer, [&](vk::CommandBuffer command_buffer) {
            uint8_t* pixels;
            int width, height;
            ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            const vk::DeviceSize image_size = width * height * 4 * sizeof(decltype(pixels[0]));

            vk::ImageCreateInfo image_info;
            image_info.setImageType(vk::ImageType::e2D)
                .setFormat(vk::Format::eR8G8B8A8Unorm)
                .setExtent(vk::Extent3D(width, height, 1))
                .setMipLevels(1)
                .setArrayLayers(1)
                .setTiling(vk::ImageTiling::eOptimal)
                .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setInitialLayout(vk::ImageLayout::eUndefined);
            m_font_image = _device->createImage(image_info);
            const auto image_requirements = _device->getImageMemoryRequirements(m_font_image);
            m_font_image_memory = _device->memory()->allocate(image_requirements.size, image_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
            _device->bindImageMemory(m_font_image, m_font_image_memory->memory, m_font_image_memory->offset);

            vk::ImageViewCreateInfo image_view_info;
            image_view_info.setFormat(vk::Format::eR8G8B8A8Unorm)
                .setImage(m_font_image)
                .setViewType(vk::ImageViewType::e2D)
                .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
            m_font_image_view = _device->createImageView(image_view_info);

            m_font_descriptor_image = vk::DescriptorImageInfo(m_font_sampler, m_font_image_view, vk::ImageLayout::eShaderReadOnlyOptimal);
            m_font_write_descriptor = vk::WriteDescriptorSet(nullptr, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &m_font_descriptor_image);
            //device->updateDescriptorSets({ m_font_write_descriptor }, nullptr);

            // Staging Buffer
            const auto staging_buffer = _device->createBuffer(vk::BufferCreateInfo({}, image_size, vk::BufferUsageFlagBits::eTransferSrc));
            const auto staging_req = _device->getBufferMemoryRequirements(staging_buffer);
            const auto staging_memory = _device->memory()->allocate(staging_req.size, staging_req.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
            _device->bindBufferMemory(staging_buffer, staging_memory->memory, staging_memory->offset);

            const vk::MappedMemoryRange range(staging_memory->memory, staging_memory->offset, staging_memory->size);
            // const auto data = _device->mapMemory(range.memory, range.offset, range.size);
            memcpy(staging_memory->data, pixels, image_size);
            _device->flushMappedMemoryRanges({ range });
            //_device->unmapMemory(range.memory);

            // Upload from staging buffer to image
            {
                vkn::command::transformImageLayout(command_buffer, m_font_image, image_view_info.subresourceRange, vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer);
                command_buffer.copyBufferToImage(staging_buffer, m_font_image, vk::ImageLayout::eTransferDstOptimal,
                    { vk::BufferImageCopy(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),{}, vk::Extent3D(width, height, 1)) });

                vkn::command::transformImageLayout(command_buffer, m_font_image, image_view_info.subresourceRange, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader);
            }
        });
        return reinterpret_cast<void*>(static_cast<VkImageView>(m_font_image_view));
    }

    void gui_vk::pre_render(ImDrawData* draw_data)
    {
        const auto size = draw_data->TotalVtxCount * sizeof(ImDrawVert) + draw_data->TotalIdxCount * sizeof(ImDrawIdx);
        auto&& frame_infos = m_frame_infos[(++_current_frame) %= queued_frames];

        auto&& command_buffer = _command_buffer;

        const vk::Rect2D scissor_rect({ 0, 0 }, { static_cast<uint32_t>(_swapchain->extent().width), static_cast<uint32_t>(_swapchain->extent().height) });
        command_buffer.beginRenderPass(vk::RenderPassBeginInfo(m_renderpass, m_framebuffers[_swapchain->currentImage()], scissor_rect, 0, nullptr), vk::SubpassContents::eInline);
        if (frame_infos.last_size < size)
        {
            _device->waitIdle();
            frame_infos.last_size = size;
            if (frame_infos.buffer_memory)
                _device->memory()->free(frame_infos.buffer_memory);
            if (frame_infos.vertex_buffer)
                _device->destroyBuffer(frame_infos.vertex_buffer);
            if (frame_infos.index_buffer)
                _device->destroyBuffer(frame_infos.index_buffer);

            const vk::BufferCreateInfo vertex_buffer_info({}, draw_data->TotalVtxCount * sizeof(ImDrawVert), vk::BufferUsageFlagBits::eVertexBuffer);
            frame_infos.vertex_buffer = _device->createBuffer(vertex_buffer_info);

            const vk::BufferCreateInfo index_buffer_info({}, draw_data->TotalIdxCount * sizeof(ImDrawIdx), vk::BufferUsageFlagBits::eIndexBuffer);
            frame_infos.index_buffer = _device->createBuffer(index_buffer_info);

            auto combined_requirements = _device->getBufferMemoryRequirements(frame_infos.vertex_buffer);
            const auto vtx_req = combined_requirements;
            frame_infos.idx_offset = 1024 * ((vtx_req.size + 1024 - 1) / 1024);
            const auto idx_req = _device->getBufferMemoryRequirements(frame_infos.index_buffer);
            combined_requirements.size = frame_infos.idx_offset + idx_req.size;
            combined_requirements.memoryTypeBits &= idx_req.memoryTypeBits;
            frame_infos.buffer_memory = _device->memory()->allocate(combined_requirements.size, combined_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);

            _device->bindBufferMemory(frame_infos.vertex_buffer, frame_infos.buffer_memory->memory, frame_infos.buffer_memory->offset);
            _device->bindBufferMemory(frame_infos.index_buffer, frame_infos.buffer_memory->memory, frame_infos.buffer_memory->offset + frame_infos.idx_offset);
        }

        {
            std::array<vk::MappedMemoryRange, 1> mem_ranges = {
                vk::MappedMemoryRange(frame_infos.buffer_memory->memory, frame_infos.buffer_memory->offset, frame_infos.buffer_memory->size),
            };

            //const auto data = m_info.device->mapMemory(mem_ranges[0].memory, mem_ranges[0].offset, mem_ranges[0].size);
            ImDrawVert* vertices_dst = static_cast<ImDrawVert*>(frame_infos.buffer_memory->data);
            ImDrawIdx* indices_dst = reinterpret_cast<ImDrawIdx*>(static_cast<uint8_t*>(frame_infos.buffer_memory->data) + frame_infos.idx_offset);

            for (int list = 0; list < draw_data->CmdListsCount; ++list)
            {
                const auto cmd_list = draw_data->CmdLists[list];
                memcpy(vertices_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(indices_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

                vertices_dst += cmd_list->VtxBuffer.Size;
                indices_dst += cmd_list->IdxBuffer.Size;
            }

            _device->flushMappedMemoryRanges(mem_ranges);
        }

        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        command_buffer.bindVertexBuffers(0, { frame_infos.vertex_buffer }, { 0 });
        command_buffer.bindIndexBuffer(frame_infos.index_buffer, 0, vk::IndexType::eUint16);

        const auto vp = vk::Viewport(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.f, 1.f);
        command_buffer.setViewport(0, 1, &vp);

        glm::vec2 scale(2.f / ImGui::GetIO().DisplaySize.x, 2.f / ImGui::GetIO().DisplaySize.y);
        glm::vec2 translate(-1.f);
        command_buffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::vec2), &scale);
        command_buffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec2), sizeof(glm::vec2), &translate);
    }

    void gui_vk::render(const ImDrawCmd& cmd, int index_offset, int vertex_offset)
    {
        auto&& command_buffer = _command_buffer;
        if (cmd.TextureId)
        {
            const auto image = static_cast<vk::ImageView>(reinterpret_cast<VkImageView>(cmd.TextureId));
            m_font_descriptor_image.imageView = image;
            command_buffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, m_pipeline_layout, 0, { m_font_write_descriptor });
        }

        vk::Rect2D scissor;
        scissor.offset.x = std::max(static_cast<int>(cmd.ClipRect.x), 0);
        scissor.offset.y = std::max(static_cast<int>(cmd.ClipRect.y), 0);
        scissor.extent.width = static_cast<uint32_t>(cmd.ClipRect.z - cmd.ClipRect.x);
        scissor.extent.height = static_cast<uint32_t>(cmd.ClipRect.w - cmd.ClipRect.y);
        command_buffer.setScissor(0, scissor);
        command_buffer.drawIndexed(cmd.ElemCount, 1, index_offset, vertex_offset, 0);
    }

    void gui_vk::post_render()
    {
        _command_buffer.endRenderPass();
    }

    void gui_vk::update_swapchain(vkn::Swapchain* swapchain)
    {
        for (auto&& fbo : m_framebuffers) _device->destroyFramebuffer(fbo);
        m_framebuffers.clear();
        _swapchain = swapchain;
        _swapchain->inc_ref();
        vk::FramebufferCreateInfo framebuffer_info({}, m_renderpass, 1, nullptr, _swapchain->extent().width, _swapchain->extent().height, 1);
        for (auto i = 0u; i < static_cast<int32_t>(_swapchain->images().size()); ++i)
        {
            const std::array<vk::ImageView, 1> att = { *_swapchain->imageViews()[i] };
            framebuffer_info.setPAttachments(std::data(att));
            m_framebuffers.push_back(_device->createFramebuffer(framebuffer_info));
        }
    }

    void gui_vk::set_next_command_buffer(vk::CommandBuffer buffer)
    {
        _command_buffer = buffer;
    }

    void gui_vk::create_device_objects()
    {
        // Create Font Sampler
        {
            vk::SamplerCreateInfo sampler_create_info;
            sampler_create_info.setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setMinLod(-1000)
                .setMaxLod(1000)
                .setMaxAnisotropy(1.f);
            m_font_sampler = _device->createSampler(sampler_create_info);
        }

        // Descriptor Set Layout and the set itself
        {
            vk::DescriptorSetLayoutBinding sampler_binding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, &m_font_sampler);
            m_descriptor_set_layout = _device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR, 1, &sampler_binding));
        }

        // Pipeline Layout
        {
            vk::PushConstantRange push_constant_range(vk::ShaderStageFlagBits::eVertex, 0, sizeof(float) * 4);
            m_pipeline_layout = _device->createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &m_descriptor_set_layout, 1, &push_constant_range));
        }

        const auto vs = jpu::make_ref<vkn::ShaderModule>(vkn::ShaderModuleCreateInfo(_device, "../shaders/gui/vk/imgui.vert"));
        const auto fs = jpu::make_ref<vkn::ShaderModule>(vkn::ShaderModuleCreateInfo(_device, "../shaders/gui/vk/imgui.frag"));
        const auto shader_stages = { vs->makePipelineStage(), fs->makePipelineStage() };

        vk::VertexInputBindingDescription vertex_input_binding(0, sizeof(ImDrawVert), vk::VertexInputRate::eVertex);
        const auto vertex_attributes = {
            vk::VertexInputAttributeDescription(0, vertex_input_binding.binding, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, pos)),
            vk::VertexInputAttributeDescription(1, vertex_input_binding.binding, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, uv)),
            vk::VertexInputAttributeDescription(2, vertex_input_binding.binding, vk::Format::eR8G8B8A8Unorm, offsetof(ImDrawVert, col)),
        };
        vk::PipelineVertexInputStateCreateInfo vertex_input({}, 1, &vertex_input_binding, static_cast<uint32_t>(std::size(vertex_attributes)), std::data(vertex_attributes));

        vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);

        vk::PipelineViewportStateCreateInfo viewport({}, 1, nullptr, 1, nullptr);

        vk::PipelineRasterizationStateCreateInfo rasterizer({}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, false, 0, 0, 0, 1.f);

        vk::PipelineMultisampleStateCreateInfo multisample({}, vk::SampleCountFlagBits::e1);

        vk::PipelineColorBlendAttachmentState color_attachment;
        color_attachment.setBlendEnable(true).setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        vk::PipelineColorBlendStateCreateInfo blending;
        blending.setPAttachments(&color_attachment)
            .setAttachmentCount(1);

        vk::PipelineDepthStencilStateCreateInfo depth_stencil;

        const auto dynamic_states = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynamic_state({}, static_cast<uint32_t>(std::size(dynamic_states)), std::data(dynamic_states));

        vk::GraphicsPipelineCreateInfo pipeline_info;
        pipeline_info.setLayout(m_pipeline_layout)
            .setPColorBlendState(&blending)
            .setPDepthStencilState(&depth_stencil)
            .setPDynamicState(&dynamic_state)
            .setPInputAssemblyState(&input_assembly)
            .setPMultisampleState(&multisample)
            .setPRasterizationState(&rasterizer)
            .setPStages(std::data(shader_stages))
            .setStageCount(static_cast<uint32_t>(std::size(shader_stages)))
            .setPVertexInputState(&vertex_input)
            .setPViewportState(&viewport)
            .setRenderPass(m_renderpass)
            .setSubpass(0);

        m_pipeline = _device->createGraphicsPipeline(nullptr, pipeline_info);
    }
}
