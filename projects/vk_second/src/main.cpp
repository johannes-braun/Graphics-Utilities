#include "init.hpp"
#include <GLFW/glfw3.h>
#include <execution>
#include <fstream>
#include <gfx/camera.hpp>
#include <gfx/file.hpp>
#include <gfx/log.hpp>
#include <numeric>
#include <vulkan/vulkan.hpp>

vk::UniqueInstance           instance;
vk::UniqueDevice             device;
std::shared_ptr<gfx::window> window;
vk::UniqueSurfaceKHR         surface;
vk::UniqueSwapchainKHR       swapchain;
vk::UniqueRenderPass         forward_renderpass;
std::array<vk::Queue, 4>     queues;
std::array<uint32_t, 4>      families;

struct
{
    vk::UniqueDescriptorSetLayout scene;
    vk::UniqueDescriptorSetLayout models;
} descriptor_layouts;

vk::UniquePipelineLayout forward_pipeline_layout;
vk::UniquePipeline       create_fwd_pipeline();

int main()
{
    window = std::make_shared<gfx::window>(gfx::apis::vulkan::name, "Vulkan Thing", 1280, 720);
    window->set_max_framerate(50000.f);

    instance = create_instance();
    const auto debug_flags =
            vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
    auto debug_callback = create_debug_callback(instance, debug_flags);

    // use gpu[0]
    const vk::PhysicalDevice gpu = instance->enumeratePhysicalDevices()[0];

    vk::SurfaceKHR surf;
    glfwCreateWindowSurface(static_cast<VkInstance>(*instance), *window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surf));
    surface = vk::UniqueSurfaceKHR(surf, vk::UniqueHandleTraits<vk::SurfaceKHR>::deleter(*instance));

    auto infos       = create_device(instance, gpu, surface);
    device           = std::move(infos.device);
    families         = std::move(infos.queue_families);
    queues[graphics] = device->getQueue(families[graphics], infos.queue_indices[graphics]);
    queues[compute]  = device->getQueue(families[compute], infos.queue_indices[compute]);
    queues[transfer] = device->getQueue(families[transfer], infos.queue_indices[transfer]);
    queues[present]  = device->getQueue(families[present], infos.queue_indices[present]);

    vk::AttachmentDescription color_attachment;
    color_attachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    color_attachment.initialLayout  = vk::ImageLayout::eUndefined;
    color_attachment.format         = vk::Format::eB8G8R8A8Unorm;
    color_attachment.samples        = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp         = vk::AttachmentLoadOp::eDontCare;
    color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription msaa_attachment;
    msaa_attachment.finalLayout    = vk::ImageLayout::eColorAttachmentOptimal;
    msaa_attachment.initialLayout  = vk::ImageLayout::eUndefined;
    msaa_attachment.format         = vk::Format::eB8G8R8A8Unorm;
    msaa_attachment.samples        = vk::SampleCountFlagBits::e8;
    msaa_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
    msaa_attachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
    msaa_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    msaa_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription depth_attachment;
    depth_attachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth_attachment.initialLayout  = vk::ImageLayout::eUndefined;
    depth_attachment.format         = vk::Format::eD32Sfloat;
    depth_attachment.samples        = vk::SampleCountFlagBits::e8;
    depth_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
    depth_attachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
    depth_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::SubpassDependency dep;
    dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;
    dep.srcAccessMask   = vk::AccessFlagBits(0);
    dep.srcStageMask    = vk::PipelineStageFlagBits::eAllGraphics;
    dep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dep.dstAccessMask   = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    dep.dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.dstSubpass      = 0;

    vk::AttachmentReference color_attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference resolve_attachment_reference(1, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depth_attachment_reference(2, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &color_attachment_reference;
    subpass.pResolveAttachments     = &resolve_attachment_reference;
    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    const auto               attachments = {msaa_attachment, color_attachment, depth_attachment};
    vk::RenderPassCreateInfo fwd_renderpass_info;
    fwd_renderpass_info.attachmentCount = std::size(attachments);
    fwd_renderpass_info.pAttachments    = std::data(attachments);
    fwd_renderpass_info.subpassCount    = 1;
    fwd_renderpass_info.pSubpasses      = &subpass;
    fwd_renderpass_info.dependencyCount = 1;
    fwd_renderpass_info.pDependencies   = &dep;
    forward_renderpass                  = device->createRenderPassUnique(fwd_renderpass_info);

    const vk::SurfaceCapabilitiesKHR capabilities = gpu.getSurfaceCapabilitiesKHR(surface.get());
    swapchain                                     = create_swapchain(gpu, device, surface, families[present], capabilities.currentExtent);
    std::vector<vk::Image>              swapchain_images = device->getSwapchainImagesKHR(swapchain.get());
    std::vector<vk::UniqueFramebuffer>  main_framebuffers(swapchain_images.size());
    std::vector<vk::UniqueImageView>    swapchain_image_views(swapchain_images.size());
    std::vector<vk::UniqueImage>        depth_attachments(swapchain_images.size());
    std::vector<vk::UniqueImage>        msaa_attachments(swapchain_images.size());
    std::vector<vk::UniqueImageView>    depth_attachment_views(swapchain_images.size());
    std::vector<vk::UniqueImageView>    msaa_attachment_views(swapchain_images.size());
    std::vector<vk::UniqueDeviceMemory> image_memories;
    const auto                          graphics_present_families = {families[graphics], families[present]};
    for(int i = 0; i < main_framebuffers.size(); ++i)
    {
        vk::ImageViewCreateInfo ivi;
        ivi.viewType             = vk::ImageViewType::e2D;
        ivi.format               = vk::Format::eB8G8R8A8Unorm;
        ivi.image                = swapchain_images[i];
        ivi.subresourceRange     = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        swapchain_image_views[i] = device->createImageViewUnique(ivi);

        vk::ImageCreateInfo depth_info;
        depth_info.arrayLayers           = 1;
        depth_info.extent                = vk::Extent3D{1280, 720, 1};
        depth_info.format                = vk::Format::eD32Sfloat;
        depth_info.imageType             = vk::ImageType::e2D;
        depth_info.initialLayout         = vk::ImageLayout::eUndefined;
        depth_info.mipLevels             = 1;
        depth_info.pQueueFamilyIndices   = std::data(graphics_present_families);
        depth_info.queueFamilyIndexCount = std::size(graphics_present_families);
        depth_info.samples               = vk::SampleCountFlagBits::e8;
        depth_info.usage                 = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        depth_attachments[i]             = device->createImageUnique(depth_info);
        depth_info.usage                 = vk::ImageUsageFlagBits::eColorAttachment;
        depth_info.format                = vk::Format::eB8G8R8A8Unorm;
        msaa_attachments[i]              = device->createImageUnique(depth_info);

        // mem...
        {
            const auto req = device->getImageMemoryRequirements(depth_attachments[i].get());
            auto&&     mem = image_memories.emplace_back(std::move(device->allocateMemoryUnique(
                    vk::MemoryAllocateInfo(req.size, memory_index(gpu, req, vk::MemoryPropertyFlagBits::eDeviceLocal)))));
            device->bindImageMemory(depth_attachments[i].get(), mem.get(), 0);
        }
        {
            const auto req = device->getImageMemoryRequirements(msaa_attachments[i].get());
            auto&&     mem = image_memories.emplace_back(std::move(device->allocateMemoryUnique(
                    vk::MemoryAllocateInfo(req.size, memory_index(gpu, req, vk::MemoryPropertyFlagBits::eDeviceLocal)))));
            device->bindImageMemory(msaa_attachments[i].get(), mem.get(), 0);
        }

        ivi.image                       = msaa_attachments[i].get();
        msaa_attachment_views[i]        = device->createImageViewUnique(ivi);
        ivi.format                      = vk::Format::eD32Sfloat;
        ivi.image                       = depth_attachments[i].get();
        ivi.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        depth_attachment_views[i]       = device->createImageViewUnique(ivi);

        const auto fbo_attachments = {msaa_attachment_views[i].get(), swapchain_image_views[i].get(), depth_attachment_views[i].get()};

        vk::FramebufferCreateInfo fbc;
        fbc.attachmentCount  = std::size(fbo_attachments);
        fbc.height           = capabilities.currentExtent.height;
        fbc.width            = capabilities.currentExtent.width;
        fbc.layers           = 1;
        fbc.pAttachments     = std::data(fbo_attachments);
        fbc.renderPass       = forward_renderpass.get();
        main_framebuffers[i] = device->createFramebufferUnique(fbc);
    }

    vk::CommandPoolCreateInfo pool_info;
    pool_info.flags                             = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    pool_info.queueFamilyIndex                  = families[graphics];
    vk::UniqueCommandPool graphics_command_pool = device->createCommandPoolUnique(pool_info);

    vk::CommandBufferAllocateInfo primary_buffers_allocation;
    primary_buffers_allocation.commandBufferCount         = static_cast<uint32_t>(swapchain_images.size());
    primary_buffers_allocation.level                      = vk::CommandBufferLevel::ePrimary;
    primary_buffers_allocation.commandPool                = graphics_command_pool.get();
    std::vector<vk::UniqueCommandBuffer> primary_commands = device->allocateCommandBuffersUnique(primary_buffers_allocation);

    vk::UniqueSemaphore          swap_semaphore   = device->createSemaphoreUnique({});
    vk::UniqueSemaphore          render_semaphore = device->createSemaphoreUnique({});
    std::vector<vk::UniqueFence> render_fences(swapchain_images.size());
    std::generate(render_fences.begin(), render_fences.end(), [&]() {
        return device->createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    });

    vk::QueryPoolCreateInfo timer_pool_info;
    timer_pool_info.queryType            = vk::QueryType::eTimestamp;
    timer_pool_info.queryCount           = static_cast<uint32_t>(2 * swapchain_images.size());
    vk::UniqueQueryPool timer_query_pool = device->createQueryPoolUnique(timer_pool_info);

    gfx::scene_file scene("sponza.obj");

    struct mesh_info
    {
        alignas(16) vk::DrawIndexedIndirectCommand indirect;
        alignas(16) glm::mat4 model_matrix;
        gfx::bounds3f bounds;
    };

    std::vector<gfx::vertex3d> vertices;
    std::vector<gfx::index32>  indices;
    std::vector<mesh_info>     mesh_infos(scene.meshes.size());
    for(size_t i = 0; i < scene.meshes.size(); ++i)
    {
        mesh_infos[i].indirect.vertexOffset  = vertices.size();
        mesh_infos[i].indirect.firstIndex    = indices.size();
        mesh_infos[i].indirect.firstInstance = i;
        mesh_infos[i].indirect.instanceCount = 1;
        mesh_infos[i].indirect.indexCount    = scene.meshes[i].indices.size();
        struct reduction
        {
            gfx::bounds3f operator()(const gfx::bounds3f& b, const gfx::vertex3d& x) const { return b + x.position; }

            gfx::bounds3f operator()(const gfx::vertex3d& x, const gfx::bounds3f& b) const { return b + x.position; }

            gfx::bounds3f operator()(const gfx::vertex3d& b, const gfx::vertex3d& x) const
            {
                gfx::bounds3f bounds;
                bounds += b.position;
                return bounds + x.position;
            }

            gfx::bounds3f operator()(const gfx::bounds3f& b, const gfx::bounds3f& x) const { return b + x; }
        };

        mesh_infos[i].bounds = std::reduce(std::execution::par_unseq, vertices.begin(), vertices.end(), mesh_infos[i].bounds, reduction());

        gfx::transform tf          = scene.meshes[i].transform;
        tf.scale                   = glm::vec3(0.01f);
        mesh_infos[i].model_matrix = tf;

        indices.insert(indices.end(), scene.meshes[i].indices.begin(), scene.meshes[i].indices.end());
        vertices.insert(vertices.end(), scene.meshes[i].vertices.begin(), scene.meshes[i].vertices.end());
    }

    vk::BufferCreateInfo indirect_buffer_info;
    indirect_buffer_info.size = mesh_infos.size() * sizeof(mesh_info);
    indirect_buffer_info.usage =
            vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
    std::unordered_set<uint32_t> indirect_family_indices{families[graphics], families[transfer], families[compute]};
    std::vector<uint32_t>        indirect_family_indices_v(indirect_family_indices.begin(), indirect_family_indices.end());
    indirect_buffer_info.pQueueFamilyIndices     = indirect_family_indices_v.data();
    indirect_buffer_info.queueFamilyIndexCount   = indirect_family_indices_v.size();
    indirect_buffer_info.sharingMode             = vk::SharingMode::eExclusive;
    vk::UniqueBuffer             indirect_buffer = device->createBufferUnique(indirect_buffer_info);
    const vk::MemoryRequirements indireq         = device->getBufferMemoryRequirements(indirect_buffer.get());
    vk::UniqueDeviceMemory       indirect_memory = device->allocateMemoryUnique(
            vk::MemoryAllocateInfo(indireq.size, memory_index(gpu, indireq, vk::MemoryPropertyFlagBits::eDeviceLocal)));
    device->bindBufferMemory(indirect_buffer.get(), indirect_memory.get(), 0);

    vk::BufferCreateInfo buffer_info;
    buffer_info.size = vertices.size() * sizeof(gfx::vertex3d);
    std::unordered_set<uint32_t> family_indices{families[graphics], families[transfer]};
    std::vector<uint32_t>        family_indices_v{family_indices.begin(), family_indices.end()};
    buffer_info.pQueueFamilyIndices   = family_indices_v.data();
    buffer_info.queueFamilyIndexCount = static_cast<uint32_t>(family_indices_v.size());
    buffer_info.usage                 = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    buffer_info.sharingMode           = vk::SharingMode::eConcurrent;
    uint32_t         index_count      = static_cast<uint32_t>(indices.size());
    vk::UniqueBuffer vbo              = device->createBufferUnique(buffer_info);
    buffer_info.size                  = indices.size() * sizeof(gfx::index32);
    buffer_info.usage                 = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    vk::UniqueBuffer ibo              = device->createBufferUnique(buffer_info);

    const vk::MemoryRequirements vbo_req = device->getBufferMemoryRequirements(vbo.get());
    const vk::MemoryRequirements ibo_req = device->getBufferMemoryRequirements(ibo.get());

    vk::MemoryRequirements combined;
    combined.size           = vbo_req.size + ibo_req.size;
    combined.alignment      = glm::max(vbo_req.alignment, ibo_req.alignment);
    combined.memoryTypeBits = vbo_req.memoryTypeBits | ibo_req.memoryTypeBits;

    vk::UniqueDeviceMemory shared_ibo_vbo_mem =
            device->allocateMemoryUnique({combined.size, memory_index(gpu, combined, vk::MemoryPropertyFlagBits::eDeviceLocal)});
    device->bindBufferMemory(vbo.get(), shared_ibo_vbo_mem.get(), 0);
    device->bindBufferMemory(ibo.get(), shared_ibo_vbo_mem.get(), vbo_req.size);

    // - TRANSFER - //
    vk::BufferCreateInfo transfer_src;
    transfer_src.size                                = combined.size + indireq.size;
    transfer_src.queueFamilyIndexCount               = 1;
    transfer_src.pQueueFamilyIndices                 = &families[transfer];
    transfer_src.sharingMode                         = vk::SharingMode::eExclusive;
    transfer_src.usage                               = vk::BufferUsageFlagBits::eTransferSrc;
    vk::UniqueBuffer             transfer_src_buffer = device->createBufferUnique(transfer_src);
    const vk::MemoryRequirements transfer_req        = device->getBufferMemoryRequirements(transfer_src_buffer.get());
    vk::UniqueDeviceMemory       tf_src_mem =
            device->allocateMemoryUnique({transfer_req.size, memory_index(gpu, transfer_req, vk::MemoryPropertyFlagBits::eHostVisible)});
    device->bindBufferMemory(transfer_src_buffer.get(), tf_src_mem.get(), 0);

    std::byte* mapped = static_cast<std::byte*>(device->mapMemory(tf_src_mem.get(), 0, transfer_req.size, {}));
    memcpy(mapped, vertices.data(), vertices.size() * sizeof(gfx::vertex3d));
    memcpy(mapped + vbo_req.size, indices.data(), indices.size() * sizeof(gfx::index32));
    memcpy(mapped + vbo_req.size + ibo_req.size, mesh_infos.data(), mesh_infos.size() * sizeof(mesh_info));
    device->unmapMemory(tf_src_mem.get());

    vk::CommandPoolCreateInfo tfcmdpoolinfo;
    tfcmdpoolinfo.queueFamilyIndex              = families[transfer];
    vk::UniqueCommandPool transfer_command_pool = device->createCommandPoolUnique(tfcmdpoolinfo);

    vk::CommandBufferAllocateInfo tfbufalloc(transfer_command_pool.get(), vk::CommandBufferLevel::ePrimary, 1);
    vk::UniqueCommandBuffer       transfer_command = std::move(device->allocateCommandBuffersUnique(tfbufalloc)[0]);

    vk::CommandBufferBeginInfo transfer_begin;
    transfer_begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    transfer_command->begin(transfer_begin);
    {
        vk::BufferCopy copy;
        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size      = vbo_req.size;
        transfer_command->copyBuffer(transfer_src_buffer.get(), vbo.get(), copy);
        copy.srcOffset = vbo_req.size;
        copy.size      = ibo_req.size;
        transfer_command->copyBuffer(transfer_src_buffer.get(), ibo.get(), copy);
        copy.srcOffset = vbo_req.size + ibo_req.size;
        copy.size      = indireq.size;
        transfer_command->copyBuffer(transfer_src_buffer.get(), indirect_buffer.get(), copy);
    }
    transfer_command->end();
    vk::SubmitInfo transfer_submit;
    transfer_submit.commandBufferCount = 1;
    transfer_submit.pCommandBuffers    = &transfer_command.get();
    queues[transfer].submit(transfer_submit, nullptr);

    vk::UniquePipeline pipeline = create_fwd_pipeline();

    vk::ComputePipelineCreateInfo cull_pp_info;
    cull_pp_info.layout = forward_pipeline_layout.get();

    std::ifstream shader_file;

    shader_file.open("../shd/cull.comp.spv", std::ios::in | std::ios::binary);
    shader_file.ignore(std::numeric_limits<std::streamsize>::max());
    shader_file.clear();
    std::streamsize size = shader_file.gcount();
    shader_file.seekg(0, std::ios::beg);
    std::vector<uint32_t> cull_shader_data(size / sizeof(uint32_t));
    shader_file.read(reinterpret_cast<char*>(cull_shader_data.data()), size);
    shader_file.close();
    
    vk::ShaderModuleCreateInfo csinfo;
    csinfo.codeSize                        = sizeof(uint32_t) * cull_shader_data.size();
    csinfo.pCode                           = cull_shader_data.data();
    vk::UniqueShaderModule cull_module = device->createShaderModuleUnique(csinfo);

    vk::PipelineShaderStageCreateInfo cull_stage;
    cull_pp_info.stage.module         = cull_module.get();
    cull_pp_info.stage.pName          = "main";
    cull_pp_info.stage.stage          = vk::ShaderStageFlagBits::eCompute;

    vk::UniquePipeline cull_pipeline = device->createComputePipelineUnique(nullptr, cull_pp_info);

    vk::CommandPoolCreateInfo compute_command_pool_info;
    compute_command_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    compute_command_pool_info.queueFamilyIndex = families[compute];
    vk::UniqueCommandPool compute_pool = device->createCommandPoolUnique(compute_command_pool_info);

    vk::CommandBufferAllocateInfo cull_cmd_alloc;
    cull_cmd_alloc.commandBufferCount = 1;
    cull_cmd_alloc.commandPool = compute_pool.get();
    cull_cmd_alloc.level = vk::CommandBufferLevel::ePrimary;
    vk::UniqueCommandBuffer cull_command_buffer = std::move(device->allocateCommandBuffersUnique(cull_cmd_alloc)[0]);

    vk::CommandBufferAllocateInfo sec_cmd_alloc;
    sec_cmd_alloc.commandBufferCount                  = swapchain_images.size();
    sec_cmd_alloc.commandPool                         = graphics_command_pool.get();
    sec_cmd_alloc.level                               = vk::CommandBufferLevel::eSecondary;
    std::vector<vk::UniqueCommandBuffer> sec_commands = device->allocateCommandBuffersUnique(sec_cmd_alloc);

    vk::DescriptorPoolCreateInfo descriptor_pool_create;
    descriptor_pool_create.maxSets = 1;
    enum descriptor_type_sizes
    {
        uniform_buf = 0,
        storage_buf = 1
    };
    vk::DescriptorPoolSize buffer_count[2];
    buffer_count[uniform_buf].descriptorCount = 5;
    buffer_count[uniform_buf].type            = vk::DescriptorType::eUniformBuffer;
    buffer_count[storage_buf].descriptorCount = 5;
    buffer_count[storage_buf].type            = vk::DescriptorType::eStorageBuffer;

    descriptor_pool_create.poolSizeCount = 1;
    descriptor_pool_create.pPoolSizes    = &buffer_count[uniform_buf];
    descriptor_pool_create.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    vk::UniqueDescriptorPool scene_pool  = device->createDescriptorPoolUnique(descriptor_pool_create);

    descriptor_pool_create.poolSizeCount = 1;
    descriptor_pool_create.pPoolSizes    = &buffer_count[storage_buf];
    descriptor_pool_create.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    vk::UniqueDescriptorPool model_pool  = device->createDescriptorPoolUnique(descriptor_pool_create);

    auto                          desc_sets = {descriptor_layouts.scene.get(), descriptor_layouts.models.get()};
    vk::DescriptorSetAllocateInfo scene_desc_alloc;
    scene_desc_alloc.descriptorPool               = scene_pool.get();
    scene_desc_alloc.descriptorSetCount           = 1;
    scene_desc_alloc.pSetLayouts                  = &descriptor_layouts.scene.get();
    auto                    sets                  = device->allocateDescriptorSetsUnique(scene_desc_alloc);
    vk::UniqueDescriptorSet scene_descriptor_set  = std::move(sets[0]);
    scene_desc_alloc.descriptorPool               = model_pool.get();
    scene_desc_alloc.pSetLayouts                  = &descriptor_layouts.models.get();
    sets                                          = device->allocateDescriptorSetsUnique(scene_desc_alloc);
    vk::UniqueDescriptorSet models_descriptor_set = std::move(sets[0]);
    
    struct scene_data
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 camera_position;
        glm::uint object_count;
    };
    vk::BufferCreateInfo scene_buffer_create_info;
    scene_buffer_create_info.pQueueFamilyIndices   = &families[graphics];
    scene_buffer_create_info.queueFamilyIndexCount = 1;
    scene_buffer_create_info.sharingMode           = vk::SharingMode::eExclusive;
    scene_buffer_create_info.usage                 = vk::BufferUsageFlagBits::eUniformBuffer;
    scene_buffer_create_info.size                  = sizeof(scene_data);
    vk::UniqueBuffer             scene_buffer      = device->createBufferUnique(scene_buffer_create_info);
    const vk::MemoryRequirements scene_buf_req     = device->getBufferMemoryRequirements(scene_buffer.get());
    vk::UniqueDeviceMemory       scene_memory      = device->allocateMemoryUnique(
            {scene_buf_req.size,
             memory_index(gpu, scene_buf_req, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)});
    device->bindBufferMemory(scene_buffer.get(), scene_memory.get(), 0);

    gfx::camera camera;
    camera.projection.perspective().negative_y = true;
    camera.transform.position                  = glm::vec3(0, 0, 4);

    scene_data* current_scene_data      = static_cast<scene_data*>(device->mapMemory(scene_memory.get(), 0, sizeof(scene_data), {}));
    current_scene_data->view            = inverse(camera.transform.matrix());
    current_scene_data->projection      = camera.projection;
    current_scene_data->camera_position = camera.transform.position;
    current_scene_data->object_count    = mesh_infos.size();

    vk::DescriptorBufferInfo scene_buffer_info;
    scene_buffer_info.buffer = scene_buffer.get();
    scene_buffer_info.range  = sizeof(scene_data);
    vk::WriteDescriptorSet scene_write;
    scene_write.descriptorCount = 1;
    scene_write.descriptorType  = vk::DescriptorType::eUniformBuffer;
    scene_write.dstArrayElement = 0;
    scene_write.dstBinding      = 0;
    scene_write.dstSet          = scene_descriptor_set.get();
    scene_write.pBufferInfo     = &scene_buffer_info;
    device->updateDescriptorSets(scene_write, nullptr);

    vk::DescriptorBufferInfo models_buffer_info;
    models_buffer_info.buffer = indirect_buffer.get();
    models_buffer_info.range  = sizeof(mesh_info);
    vk::WriteDescriptorSet models_write;
    models_write.descriptorCount = 1;
    models_write.descriptorType  = vk::DescriptorType::eStorageBuffer;
    models_write.dstArrayElement = 0;
    models_write.dstBinding      = 0;
    models_write.dstSet          = models_descriptor_set.get();
    models_write.pBufferInfo     = &models_buffer_info;
    device->updateDescriptorSets(models_write, nullptr);

    vk::CommandBufferBeginInfo cull_begin;
    cull_begin.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    cull_command_buffer->begin(cull_begin);

    vk::BufferMemoryBarrier info_barrier;
    info_barrier.buffer = indirect_buffer.get();
    info_barrier.size = mesh_infos.size() * sizeof(mesh_info);
    info_barrier.srcAccessMask = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
    info_barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
    info_barrier.srcQueueFamilyIndex = families[graphics];
    info_barrier.dstQueueFamilyIndex = families[compute];
    cull_command_buffer->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eComputeShader, {}, {}, info_barrier, {});
    cull_command_buffer->bindPipeline(vk::PipelineBindPoint::eCompute, cull_pipeline.get());
    cull_command_buffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute, forward_pipeline_layout.get(), 0, {scene_descriptor_set.get(),models_descriptor_set.get() }, nullptr);
    cull_command_buffer->dispatch((mesh_infos.size() + 16) / 32, 1, 1);
    info_barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
    info_barrier.dstAccessMask = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
    info_barrier.dstQueueFamilyIndex = families[graphics];
    info_barrier.srcQueueFamilyIndex = families[compute];
    cull_command_buffer->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eAllCommands, {}, {}, info_barrier, {});
    cull_command_buffer->end();

    for(int i = 0; i < sec_commands.size(); ++i)
    {
        vk::CommandBufferInheritanceInfo inheritance;
        inheritance.framebuffer          = main_framebuffers[i].get();
        inheritance.occlusionQueryEnable = false;
        inheritance.renderPass           = forward_renderpass.get();
        vk::CommandBufferBeginInfo sec_begin;
        sec_begin.pInheritanceInfo = &inheritance;
        sec_begin.flags            = vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse;

        sec_commands[i]->begin(sec_begin);
        sec_commands[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
        sec_commands[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                            forward_pipeline_layout.get(),
                                            0,
                                            {scene_descriptor_set.get(), models_descriptor_set.get()},
                                            nullptr);
        sec_commands[i]->setViewport(0, vk::Viewport(0, 0, 1280, 720, 0.f, 1.f));
        sec_commands[i]->setScissor(0, vk::Rect2D({0, 0}, {1280, 720}));

        // ... render ...
        sec_commands[i]->bindVertexBuffers(0, vbo.get(), 0ull);
        sec_commands[i]->bindIndexBuffer(ibo.get(), 0ull, vk::IndexType::eUint32);
        /*for(int j = 0; j < mesh_infos.size(); ++j)
            sec_commands[i]->drawIndexedIndirect(indirect_buffer.get(), j * sizeof(mesh_info), 1, sizeof(mesh_info));
        */
        sec_commands[i]->drawIndexedIndirect(indirect_buffer.get(), 0, mesh_infos.size(), sizeof(mesh_info));

        sec_commands[i]->end();
    }

    gfx::camera_controller ctrl(window);
    while(window->update())
    {
        // TODO: HACK! double buffer or something.
        ctrl.update(camera);
        current_scene_data->view            = inverse(camera.transform.matrix());
        current_scene_data->projection      = camera.projection;
        current_scene_data->camera_position = camera.transform.position;
        
        uint32_t image = device->acquireNextImageKHR(*swapchain, std::numeric_limits<uint64_t>::max(), *swap_semaphore, nullptr).value;
        device->waitForFences(*render_fences[image], true, std::numeric_limits<uint64_t>::max());
        device->resetFences(*render_fences[image]);

        // TODO:
        //vk::SubmitInfo cull_submit;
        //cull_submit.commandBufferCount = 1;
        //cull_submit.pCommandBuffers = &cull_command_buffer.get();
        //queues[compute].submit(cull_submit, nullptr);

        primary_commands[image]->reset({});
        primary_commands[image]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        primary_commands[image]->resetQueryPool(timer_query_pool.get(), 2 * image, 2);
        primary_commands[image]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), 2 * image);

        std::vector<vk::ClearValue> clear_values{vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.3f, 0.4f, 1.f})),
                                                 vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.3f, 0.4f, 1.f})),
                                                 vk::ClearValue(vk::ClearDepthStencilValue(0.f, 0))};
        vk::RenderPassBeginInfo     begin;
        begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
        begin.pClearValues    = clear_values.data();
        begin.framebuffer     = main_framebuffers[image].get();
        begin.renderArea      = vk::Rect2D({0, 0}, {1280, 720});
        begin.renderPass      = forward_renderpass.get();
        primary_commands[image]->beginRenderPass(begin, vk::SubpassContents::eSecondaryCommandBuffers);
        primary_commands[image]->executeCommands(sec_commands[image].get());
        primary_commands[image]->endRenderPass();
        primary_commands[image]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), 2 * image + 1);
        primary_commands[image]->end();

        vk::SubmitInfo submit;
        submit.commandBufferCount              = 1;
        submit.pCommandBuffers                 = &primary_commands[image].get();
        submit.pWaitSemaphores                 = &swap_semaphore.get();
        submit.waitSemaphoreCount              = 1;
        submit.pSignalSemaphores               = &render_semaphore.get();
        submit.signalSemaphoreCount            = 1;
        const vk::PipelineStageFlags wait_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submit.pWaitDstStageMask               = &wait_mask;
        queues[graphics].submit(submit, render_fences[image].get());

        vk::PresentInfoKHR present_info;
        present_info.pImageIndices      = &image;
        present_info.pSwapchains        = &swapchain.get();
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &render_semaphore.get();
        present_info.waitSemaphoreCount = 1;
        vk::Result present_result       = queues[present].presentKHR(present_info);

        uint64_t times[2];
        device->getQueryPoolResults(
                timer_query_pool.get(), 2 * image, 2, 2 * sizeof(uint64_t), times, sizeof(uint64_t), vk::QueryResultFlagBits::eWait);
        log_i << (times[1] - times[0]);

        glfwPollEvents();
    }
    device->waitIdle();
}

vk::UniquePipeline create_fwd_pipeline()
{
    vk::DescriptorSetLayoutBinding scene_matrix_binding;
    scene_matrix_binding.binding         = 0;
    scene_matrix_binding.descriptorCount = 1;
    scene_matrix_binding.descriptorType  = vk::DescriptorType::eUniformBuffer;
    scene_matrix_binding.stageFlags      = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
    vk::DescriptorSetLayoutCreateInfo scene_matrix_layout_info;
    scene_matrix_layout_info.bindingCount = 1;
    scene_matrix_layout_info.pBindings    = &scene_matrix_binding;
    descriptor_layouts.scene              = device->createDescriptorSetLayoutUnique(scene_matrix_layout_info);

    vk::DescriptorSetLayoutBinding models_binding;
    models_binding.binding         = 0;
    models_binding.descriptorCount = 1;
    models_binding.descriptorType  = vk::DescriptorType::eStorageBuffer;
    models_binding.stageFlags      = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
    vk::DescriptorSetLayoutCreateInfo models_layout_info;
    models_layout_info.bindingCount = 1;
    models_layout_info.pBindings    = &models_binding;
    descriptor_layouts.models       = device->createDescriptorSetLayoutUnique(models_layout_info);

    const auto                   layouts = {descriptor_layouts.scene.get(), descriptor_layouts.models.get()};
    vk::PipelineLayoutCreateInfo layout_info;
    layout_info.setLayoutCount = std::size(layouts);
    layout_info.pSetLayouts    = std::data(layouts);
    forward_pipeline_layout    = device->createPipelineLayoutUnique(layout_info);

    vk::GraphicsPipelineCreateInfo pp_info;
    pp_info.renderPass = forward_renderpass.get();
    pp_info.subpass    = 0;
    pp_info.layout     = forward_pipeline_layout.get();

    std::ifstream shader_file;

    shader_file.open("../shd/vs.vert.spv", std::ios::in | std::ios::binary);
    shader_file.ignore(std::numeric_limits<std::streamsize>::max());
    shader_file.clear();
    std::streamsize size = shader_file.gcount();
    shader_file.seekg(0, std::ios::beg);
    std::vector<uint32_t> vertex_shader_data(size / sizeof(uint32_t));
    shader_file.read(reinterpret_cast<char*>(vertex_shader_data.data()), size);
    shader_file.close();

    shader_file.open("../shd/fs.frag.spv", std::ios::in | std::ios::binary);
    shader_file.ignore(std::numeric_limits<std::streamsize>::max());
    shader_file.clear();
    size = shader_file.gcount();
    shader_file.seekg(0, std::ios::beg);
    std::vector<uint32_t> fragment_shader_data(size / sizeof(uint32_t));
    shader_file.read(reinterpret_cast<char*>(fragment_shader_data.data()), size);
    shader_file.close();

    enum stage
    {
        vertex   = 0,
        fragment = 1
    };
    vk::PipelineShaderStageCreateInfo shader_stages[2];

    vk::ShaderModuleCreateInfo fsinfo;
    fsinfo.codeSize                        = sizeof(uint32_t) * fragment_shader_data.size();
    fsinfo.pCode                           = fragment_shader_data.data();
    vk::UniqueShaderModule fragment_shader = device->createShaderModuleUnique(fsinfo);
    shader_stages[fragment].module         = fragment_shader.get();
    shader_stages[fragment].pName          = "main";
    shader_stages[fragment].stage          = vk::ShaderStageFlagBits::eFragment;

    vk::ShaderModuleCreateInfo vsinfo;
    vsinfo.codeSize                      = sizeof(uint32_t) * vertex_shader_data.size();
    vsinfo.pCode                         = vertex_shader_data.data();
    vk::UniqueShaderModule vertex_shader = device->createShaderModuleUnique(vsinfo);
    shader_stages[vertex].module         = vertex_shader.get();
    shader_stages[vertex].pName          = "main";
    shader_stages[vertex].stage          = vk::ShaderStageFlagBits::eVertex;

    pp_info.stageCount = std::size(shader_stages);
    pp_info.pStages    = std::data(shader_stages);

    enum attribute
    {
        position = 0,
        uv       = 1,
        normal   = 2
    };
    std::array<vk::VertexInputAttributeDescription, 3> attributes;

    attributes[position].binding  = 0;
    attributes[position].location = position;
    attributes[position].format   = vk::Format::eR32G32B32Sfloat;
    attributes[position].offset   = offsetof(gfx::vertex3d, position);

    attributes[uv].binding  = 0;
    attributes[uv].location = uv;
    attributes[uv].format   = vk::Format::eR32G32Sfloat;
    attributes[uv].offset   = offsetof(gfx::vertex3d, uv);

    attributes[normal].binding  = 0;
    attributes[normal].location = normal;
    attributes[normal].format   = vk::Format::eR32G32B32Sfloat;
    attributes[normal].offset   = offsetof(gfx::vertex3d, normal);

    vk::VertexInputBindingDescription vertex_binding;
    vertex_binding.binding   = 0;
    vertex_binding.inputRate = vk::VertexInputRate::eVertex;
    vertex_binding.stride    = sizeof(gfx::vertex3d);

    vk::PipelineVertexInputStateCreateInfo input;
    input.pVertexAttributeDescriptions    = std::data(attributes);
    input.vertexAttributeDescriptionCount = static_cast<uint32_t>(std::size(attributes));
    input.pVertexBindingDescriptions      = &vertex_binding;
    input.vertexBindingDescriptionCount   = 1;
    pp_info.pVertexInputState             = &input;

    vk::PipelineInputAssemblyStateCreateInfo input_assembly;
    input_assembly.primitiveRestartEnable = true;
    input_assembly.topology               = vk::PrimitiveTopology::eTriangleList;
    pp_info.pInputAssemblyState           = &input_assembly;

    pp_info.pTessellationState = nullptr;

    vk::PipelineViewportStateCreateInfo viewport;
    viewport.scissorCount  = 1;
    viewport.viewportCount = 1;
    pp_info.pViewportState = &viewport;

    vk::PipelineRasterizationStateCreateInfo raster;
    raster.cullMode                = vk::CullModeFlagBits::eBack;
    raster.depthBiasEnable         = false;
    raster.frontFace               = vk::FrontFace::eCounterClockwise;
    raster.polygonMode             = vk::PolygonMode::eFill;
    raster.lineWidth               = 1.f;
    raster.rasterizerDiscardEnable = false;
    raster.depthClampEnable        = false;
    pp_info.pRasterizationState    = &raster;

    vk::PipelineMultisampleStateCreateInfo msaa;
    msaa.sampleShadingEnable  = true;
    msaa.rasterizationSamples = vk::SampleCountFlagBits::e8;
    pp_info.pMultisampleState = &msaa;

    vk::PipelineDepthStencilStateCreateInfo depth;
    depth.minDepthBounds        = 0.f;
    depth.maxDepthBounds        = 1.f;
    depth.depthTestEnable       = true;
    depth.depthWriteEnable      = true;
    depth.depthBoundsTestEnable = true;
    depth.depthCompareOp        = vk::CompareOp::eGreaterOrEqual;
    depth.stencilTestEnable     = false;
    pp_info.pDepthStencilState  = &depth;

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    vk::PipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    color_blend_attachment.blendEnable = false;
    blend.attachmentCount              = 1;
    blend.pAttachments                 = &color_blend_attachment;
    pp_info.pColorBlendState           = &blend;

    vk::DynamicState                   states[2]{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic;
    dynamic.dynamicStateCount = std::size(states);
    dynamic.pDynamicStates    = std::data(states);
    pp_info.pDynamicState     = &dynamic;

    return device->createGraphicsPipelineUnique(nullptr, pp_info);
}