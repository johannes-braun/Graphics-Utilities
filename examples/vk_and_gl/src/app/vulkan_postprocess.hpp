#pragma once

struct blur_pass
{
    blur_pass(gfx::vulkan::device& gpu, std::uint32_t images, vk::Extent2D size, std::uint32_t direction, std::uint32_t half_size)
          : _gpu(&gpu), size(size)
    {
        vk::RenderPassCreateInfo rpassc;

        vk::AttachmentDescription main_attachment_description;
        main_attachment_description.loadOp         = vk::AttachmentLoadOp::eDontCare;
        main_attachment_description.storeOp        = vk::AttachmentStoreOp::eStore;
        main_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        main_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;
        main_attachment_description.format         = vk::Format::eR32G32B32A32Sfloat;
        main_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        main_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        main_attachment_description.samples        = vk::SampleCountFlagBits::e1;

        rpassc.attachmentCount = 1;
        rpassc.pAttachments    = &main_attachment_description;

        vk::SubpassDependency dependency;
        dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask   = {};
        dependency.srcStageMask    = vk::PipelineStageFlagBits::eFragmentShader;
        dependency.dstSubpass      = 0;
        dependency.dstAccessMask   = {};
        dependency.dstStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe;
        dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        rpassc.dependencyCount = 1;
        rpassc.pDependencies   = &dependency;

        vk::SubpassDescription spd;
        spd.colorAttachmentCount = 1;
        vk::AttachmentReference attachment(0, vk::ImageLayout::eColorAttachmentOptimal);
        spd.pColorAttachments = &attachment;
        spd.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

        rpassc.subpassCount = 1;
        rpassc.pSubpasses   = &spd;
        renderpass          = gpu.get_device().createRenderPassUnique(rpassc);

        vk::DescriptorPoolCreateInfo dpoolc;
        dpoolc.maxSets = images;
        vk::DescriptorPoolSize dpsize(vk::DescriptorType::eCombinedImageSampler, images);
        dpoolc.poolSizeCount = 1;
        dpoolc.pPoolSizes    = &dpsize;
        dpool                = gpu.get_device().createDescriptorPoolUnique(dpoolc);

        vk::SamplerCreateInfo smc;
        smc.magFilter  = vk::Filter::eLinear;
        smc.minFilter  = vk::Filter::eLinear;
        smc.mipmapMode = vk::SamplerMipmapMode::eLinear;
        blur_sampler   = gpu.get_device().createSamplerUnique(smc);

        vk::DescriptorSetLayoutCreateInfo inslc;
        vk::DescriptorSetLayoutBinding    inbinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment,
                                                 &blur_sampler.get());
        inslc.bindingCount = 1;
        inslc.pBindings    = &inbinding;
        input_set_layout   = gpu.get_device().createDescriptorSetLayoutUnique(inslc);

        create_pipeline(direction, half_size);

        for (std::uint32_t i = 0; i < images; ++i)
        {
            _frames.emplace_back(gpu, renderpass.get(), size);

            vk::DescriptorSetAllocateInfo dsa;
            dsa.descriptorPool     = dpool.get();
            dsa.descriptorSetCount = 1;
            dsa.pSetLayouts        = &input_set_layout.get();
            _dsets.emplace_back(std::move(gpu.get_device().allocateDescriptorSets(dsa)[0]));
        }
    }

    void render(gfx::vulkan::commands& cmd, std::uint32_t image, vk::Image input_base, vk::ImageView input, vk::ImageLayout dst_layout,
                vk::AccessFlagBits dst_access)
    {
        auto& cframe = _frames[image];
        auto& cdset  = _dsets[image];

        vk::WriteDescriptorSet updater;
        updater.descriptorCount = 1;
        updater.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        updater.dstArrayElement = 0;
        updater.dstBinding      = 0;
        updater.dstSet          = cdset.get();
        vk::DescriptorImageInfo ii(nullptr, input, vk::ImageLayout::eGeneral);
        updater.pImageInfo = &ii;
        _gpu->get_device().updateDescriptorSets(updater, nullptr);

        vk::ImageMemoryBarrier imb;
        imb.srcAccessMask       = {};
        imb.oldLayout           = vk::ImageLayout::eUndefined;
        imb.newLayout           = vk::ImageLayout::eGeneral;
        imb.dstAccessMask       = {};
        imb.image               = input_base;
        imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb.subresourceRange    = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
                                                 vk::DependencyFlagBits::eByRegion, {}, {}, imb);

        cmd.get_command_buffer().beginRenderPass(
            vk::RenderPassBeginInfo(renderpass.get(), cframe.fbuf.get(), vk::Rect2D({0, 0}, size), 0, nullptr),
            vk::SubpassContents::eInline);

        cmd.get_command_buffer().bindPipeline(vk::PipelineBindPoint::eGraphics, pipe.get());
        const vk::Viewport viewport(0, 0, size.width, size.height, 0.f, 1.f);
        const vk::Rect2D   scissor({0, 0}, size);
        cmd.get_command_buffer().setViewport(0, viewport);
        cmd.get_command_buffer().setScissor(0, scissor);
        cmd.get_command_buffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipe_layout.get(), 0, cdset.get(), {});
        cmd.get_command_buffer().draw(3, 1, 0, 0);

        cmd.get_command_buffer().pushConstants(pipe_layout.get(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(scissor.extent),
                                               &scissor.extent);
        cmd.get_command_buffer().endRenderPass();

        imb.srcAccessMask    = imb.dstAccessMask;
        imb.oldLayout        = imb.newLayout;
        imb.newLayout        = dst_layout;
        imb.dstAccessMask    = dst_access;
        imb.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eBottomOfPipe,
                                                 vk::DependencyFlagBits::eByRegion, {}, {}, imb);
    }

    const gfx::vulkan::image& get_image(std::uint32_t idx) const { return *_frames[idx].image; }
    const vk::ImageView&      get_image_view(std::uint32_t idx) const { return _frames[idx].view.get(); }

private:
    void create_pipeline(std::uint32_t direction, std::uint32_t half_size)
    {
        vk::PipelineLayoutCreateInfo plc;
        plc.setLayoutCount = 1;
        plc.pSetLayouts    = &input_set_layout.get();
        vk::PushConstantRange pc_range;
        pc_range.size              = sizeof(glm::ivec2);
        pc_range.offset            = 0;
        pc_range.stageFlags        = vk::ShaderStageFlagBits::eFragment;
        plc.pushConstantRangeCount = 1;
        plc.pPushConstantRanges    = &pc_range;
        pipe_layout                = _gpu->get_device().createPipelineLayoutUnique(plc);

        vk::GraphicsPipelineCreateInfo gpc;
        gpc.layout     = pipe_layout.get();
        gpc.renderPass = renderpass.get();
        gpc.subpass    = 0;

        vk::PipelineColorBlendStateCreateInfo cols;
        cols.attachmentCount = 1;
        vk::PipelineColorBlendAttachmentState att;
        att.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB
                             | vk::ColorComponentFlagBits::eA;
        cols.pAttachments    = &att;
        gpc.pColorBlendState = &cols;

        vk::PipelineDepthStencilStateCreateInfo dss;
        dss.depthTestEnable    = false;
        dss.depthWriteEnable   = false;
        gpc.pDepthStencilState = &dss;

        vk::PipelineDynamicStateCreateInfo dyns;
        vk::DynamicState                   states[] {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        dyns.dynamicStateCount = static_cast<std::uint32_t>(std::size(states));
        dyns.pDynamicStates    = states;
        gpc.pDynamicState      = &dyns;

        vk::PipelineInputAssemblyStateCreateInfo ins;
        ins.topology            = vk::PrimitiveTopology::eTriangleList;
        gpc.pInputAssemblyState = &ins;

        gpc.pTessellationState = nullptr;

        vk::PipelineRasterizationStateCreateInfo ras;
        ras.cullMode            = vk::CullModeFlagBits::eFront;
        ras.lineWidth           = 1.f;
        ras.polygonMode         = vk::PolygonMode::eFill;
        gpc.pRasterizationState = &ras;

        vk::PipelineVertexInputStateCreateInfo vins;
        vins.vertexAttributeDescriptionCount = 0;
        vins.vertexBindingDescriptionCount   = 0;
        gpc.pVertexInputState                = &vins;

        vk::PipelineMultisampleStateCreateInfo mss;
        mss.rasterizationSamples = vk::SampleCountFlagBits::e1;
        gpc.pMultisampleState    = &mss;

        vk::PipelineViewportStateCreateInfo vps;
        vps.scissorCount   = 1;
        vps.viewportCount  = 1;
        gpc.pViewportState = &vps;

        shader_lib.load("vk_and_gl.blur_shaders_vk");
        const auto          vs = gfx::import_shader(shader_lib, "shaders/screen.vert");
        const auto          fs = gfx::import_shader(shader_lib, "shaders/blur.frag");
        gfx::vulkan::shader vert(*_gpu, *vs);
        gfx::vulkan::shader frag(*_gpu, *fs);
        shader_lib.unload();

        struct spec_t
        {
            std::uint32_t kernel_half_size;
            std::uint32_t blur_direction;
        } specialization {half_size, direction};

        vk::SpecializationInfo spec_info;
        spec_info.mapEntryCount = 2;
        spec_info.dataSize      = sizeof(specialization);
        spec_info.pData         = &specialization;

        vk::SpecializationMapEntry entries[] {
            vk::SpecializationMapEntry(0, offsetof(spec_t, kernel_half_size), sizeof(spec_t::kernel_half_size)),
            vk::SpecializationMapEntry(1, offsetof(spec_t, blur_direction), sizeof(spec_t::blur_direction))};
        spec_info.pMapEntries = entries;

        vk::PipelineShaderStageCreateInfo stgs[] {
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vert.get_module(), "main", nullptr),
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, frag.get_module(), "main", &spec_info)};
        gpc.pStages    = stgs;
        gpc.stageCount = 2;

        pipe = _gpu->get_device().createGraphicsPipelineUnique(nullptr, gpc);
    }

    struct frame_t
    {
        frame_t(gfx::vulkan::device& gpu, vk::RenderPass rpass, vk::Extent2D size)
        {
            vk::ImageCreateInfo imgc;
            imgc.arrayLayers   = 1;
            imgc.extent        = vk::Extent3D {size.width, size.height, 1};
            imgc.format        = vk::Format::eR32G32B32A32Sfloat;
            imgc.imageType     = vk::ImageType::e2D;
            imgc.mipLevels     = 1;
            imgc.initialLayout = vk::ImageLayout::eUndefined;
            imgc.sharingMode   = vk::SharingMode::eExclusive;
            imgc.tiling        = vk::ImageTiling::eOptimal;
            imgc.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc
                         | vk::ImageUsageFlagBits::eTransferDst;

            image = std::make_unique<gfx::vulkan::image>(gpu, imgc);

            vk::ImageViewCreateInfo viewc;
            viewc.format           = imgc.format;
            viewc.image            = image->get_image();
            viewc.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            viewc.viewType         = vk::ImageViewType::e2D;

            view = gpu.get_device().createImageViewUnique(viewc);

            vk::FramebufferCreateInfo fbc;
            fbc.attachmentCount = 1;
            fbc.pAttachments    = &view.get();
            fbc.width           = size.width;
            fbc.height          = size.height;
            fbc.layers          = 1;
            fbc.renderPass      = rpass;

            fbuf = gpu.get_device().createFramebufferUnique(fbc);
        }

        std::unique_ptr<gfx::vulkan::image> image;
        vk::UniqueImageView                 view;
        vk::UniqueFramebuffer               fbuf;
    };
    gfx::vulkan::device* _gpu;

    vk::Extent2D size;

    vk::UniqueRenderPass                 renderpass;
    std::vector<frame_t>                 _frames;
    std::vector<vk::UniqueDescriptorSet> _dsets;

    vk::UniqueDescriptorPool      dpool;
    vk::UniqueDescriptorSetLayout input_set_layout;
    vk::UniqueSampler             blur_sampler;

    vk::UniquePipelineLayout pipe_layout;
    vk::UniquePipeline       pipe;
    gfx::dynamic_library_t   shader_lib;
};
