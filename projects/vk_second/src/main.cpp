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
std::array<vk::Queue, 4>     queues;
std::array<uint32_t, 4>      families;

struct
{
    vk::UniqueDescriptorSetLayout scene;
    vk::UniqueDescriptorSetLayout models;
    vk::UniqueDescriptorSetLayout shadow_textures;
} descriptor_layouts;

struct
{
    struct
    {
        vk::UniquePipelineLayout default_mesh;
    } layouts;

    vk::UniquePipeline forward_shade;
    vk::UniquePipeline shadow_pass;
    vk::UniquePipeline culling;
} pipelines;

struct
{
    vk::UniqueRenderPass forward;
    vk::UniqueRenderPass shadow;
} renderpasses;

std::array<vk::UniqueCommandPool, 3> command_pools;
vk::UniqueDescriptorPool             main_descriptor_pool;

vk::UniqueSampler shadow_sampler;

struct
{
    vk::UniqueDescriptorSet scene_info;
    vk::UniqueDescriptorSet light_info;
    vk::UniqueDescriptorSet models_info;
    vk::UniqueDescriptorSet shadow_samplers;
} descriptor_sets;

struct
{
    vk::UniqueBuffer indirect;
    vk::UniqueBuffer vertices;
    vk::UniqueBuffer indices;
} model_buffers;

void create_pipelines();
void create_renderpasses();

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
    queues[fam::graphics] = device->getQueue(families[fam::graphics], infos.queue_indices[fam::graphics]);
    queues[fam::compute]  = device->getQueue(families[fam::compute], infos.queue_indices[fam::compute]);
    queues[fam::transfer] = device->getQueue(families[fam::transfer], infos.queue_indices[fam::transfer]);
    queues[fam::present]  = device->getQueue(families[fam::present], infos.queue_indices[fam::present]);

    create_renderpasses();

    /********************************************** Swapchain & Main FBO setup *****************************************************/
    const vk::SurfaceCapabilitiesKHR capabilities = gpu.getSurfaceCapabilitiesKHR(surface.get());
    swapchain                                     = create_swapchain(gpu, device, surface, families[fam::present], capabilities.currentExtent);
    std::vector<vk::Image>              swapchain_images = device->getSwapchainImagesKHR(swapchain.get());
    std::vector<vk::UniqueFramebuffer>  main_framebuffers(swapchain_images.size());
    std::vector<vk::UniqueImageView>    swapchain_image_views(swapchain_images.size());
    std::vector<vk::UniqueImage>        depth_attachments(swapchain_images.size());
    std::vector<vk::UniqueImage>        msaa_attachments(swapchain_images.size());
    std::vector<vk::UniqueImageView>    depth_attachment_views(swapchain_images.size());
    std::vector<vk::UniqueImageView>    msaa_attachment_views(swapchain_images.size());
    std::vector<vk::UniqueDeviceMemory> image_memories;
    const auto                          graphics_present_families = {families[fam::graphics], families[fam::present]};
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
        fbc.renderPass       = renderpasses.forward.get();
        main_framebuffers[i] = device->createFramebufferUnique(fbc);
    }

    /********************************************** Shadow map fbo setup *****************************************************/
    vk::ImageCreateInfo shadow_depth_image_create_info;
    shadow_depth_image_create_info.arrayLayers           = 1;
    shadow_depth_image_create_info.extent                = vk::Extent3D{256, 256, 1};
    shadow_depth_image_create_info.format                = vk::Format::eD32Sfloat;
    shadow_depth_image_create_info.imageType             = vk::ImageType::e2D;
    shadow_depth_image_create_info.initialLayout         = vk::ImageLayout::eUndefined;
    shadow_depth_image_create_info.mipLevels             = 1;
    shadow_depth_image_create_info.queueFamilyIndexCount = 1;
    shadow_depth_image_create_info.pQueueFamilyIndices   = &families[fam::graphics];
    shadow_depth_image_create_info.samples               = vk::SampleCountFlagBits::e1;
    shadow_depth_image_create_info.sharingMode           = vk::SharingMode::eExclusive;
    shadow_depth_image_create_info.tiling                = vk::ImageTiling::eOptimal;
    shadow_depth_image_create_info.usage           = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
    vk::UniqueImage              shadow_map        = device->createImageUnique(shadow_depth_image_create_info);
    const vk::MemoryRequirements shadow_map_req    = device->getImageMemoryRequirements(shadow_map.get());
    vk::UniqueDeviceMemory       shadow_map_memory = device->allocateMemoryUnique(
            vk::MemoryAllocateInfo(shadow_map_req.size, memory_index(gpu, shadow_map_req, vk::MemoryPropertyFlagBits::eDeviceLocal)));
    device->bindImageMemory(shadow_map.get(), shadow_map_memory.get(), 0);

    vk::ImageViewCreateInfo shadow_map_view_info;
    shadow_map_view_info.format           = shadow_depth_image_create_info.format;
    shadow_map_view_info.image            = shadow_map.get();
    shadow_map_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
    shadow_map_view_info.viewType         = vk::ImageViewType::e2D;
    vk::UniqueImageView shadow_map_view   = device->createImageViewUnique(shadow_map_view_info);

    vk::FramebufferCreateInfo shadow_map_fbo_info;
    shadow_map_fbo_info.attachmentCount  = 1;
    shadow_map_fbo_info.pAttachments     = &shadow_map_view.get();
    shadow_map_fbo_info.height           = 256;
    shadow_map_fbo_info.width            = 256;
    shadow_map_fbo_info.layers           = 1;
    shadow_map_fbo_info.renderPass       = renderpasses.shadow.get();
    vk::UniqueFramebuffer shadow_map_fbo = device->createFramebufferUnique(shadow_map_fbo_info);

    /********************************************** Command pool creation *****************************************************/
    vk::CommandPoolCreateInfo pool_info;
    pool_info.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    pool_info.queueFamilyIndex = families[fam::graphics];
    command_pools[fam::graphics]    = device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex = families[fam::compute];
    command_pools[fam::compute]     = device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex = families[fam::transfer];
    command_pools[fam::transfer]    = device->createCommandPoolUnique(pool_info);

    /********************************************** Model loading *****************************************************/
    gfx::scene_file scene("Bistro_Research_Exterior.fbx");
    struct mesh_info
    {
        alignas(16) vk::DrawIndexedIndirectCommand indirect;
        alignas(16) glm::mat4 model_matrix;
        alignas(16) gfx::bounds3f bounds;
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

        mesh_infos[i].bounds = std::reduce(std::execution::par_unseq,
                                           scene.meshes[i].vertices.begin(),
                                           scene.meshes[i].vertices.end(),
                                           mesh_infos[i].bounds,
                                           reduction());

        gfx::transform tf          = scene.meshes[i].transform;
        tf.scale                   = glm::vec3(0.01f);
        mesh_infos[i].model_matrix = tf;

        indices.insert(indices.end(), scene.meshes[i].indices.begin(), scene.meshes[i].indices.end());
        vertices.insert(vertices.end(), scene.meshes[i].vertices.begin(), scene.meshes[i].vertices.end());
    }

    /********************************************** Model buffer creation *****************************************************/
    vk::BufferCreateInfo indirect_buffer_info;
    indirect_buffer_info.size = mesh_infos.size() * sizeof(mesh_info);
    indirect_buffer_info.usage =
            vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
    std::unordered_set<uint32_t> indirect_family_indices{families[fam::graphics], families[fam::transfer], families[fam::compute]};
    std::vector<uint32_t>        indirect_family_indices_v(indirect_family_indices.begin(), indirect_family_indices.end());
    indirect_buffer_info.pQueueFamilyIndices   = indirect_family_indices_v.data();
    indirect_buffer_info.queueFamilyIndexCount = indirect_family_indices_v.size();
    indirect_buffer_info.sharingMode           = vk::SharingMode::eExclusive;
    model_buffers.indirect                     = device->createBufferUnique(indirect_buffer_info);

    vk::BufferCreateInfo         buffer_info;
    std::unordered_set<uint32_t> family_indices{families[fam::graphics], families[fam::transfer]};
    std::vector<uint32_t>        family_indices_v{family_indices.begin(), family_indices.end()};
    buffer_info.pQueueFamilyIndices   = family_indices_v.data();
    buffer_info.queueFamilyIndexCount = static_cast<uint32_t>(family_indices_v.size());
    buffer_info.sharingMode           = vk::SharingMode::eConcurrent;

    buffer_info.size       = vertices.size() * sizeof(gfx::vertex3d);
    buffer_info.usage      = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    model_buffers.vertices = device->createBufferUnique(buffer_info);
    buffer_info.size       = indices.size() * sizeof(gfx::index32);
    buffer_info.usage      = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    model_buffers.indices  = device->createBufferUnique(buffer_info);

    const vk::MemoryRequirements vbo_req = device->getBufferMemoryRequirements(model_buffers.vertices.get());
    const vk::MemoryRequirements ibo_req = device->getBufferMemoryRequirements(model_buffers.indices.get());
    const vk::MemoryRequirements ind_req = device->getBufferMemoryRequirements(model_buffers.indirect.get());

    vk::MemoryRequirements combined;
    combined.size           = vbo_req.size + ibo_req.size + ind_req.size;
    combined.alignment      = glm::max(vbo_req.alignment, ibo_req.alignment, ind_req.size);
    combined.memoryTypeBits = vbo_req.memoryTypeBits | ibo_req.memoryTypeBits | ind_req.memoryTypeBits;

    vk::UniqueDeviceMemory shared_ibo_vbo_ind_mem =
            device->allocateMemoryUnique({combined.size, memory_index(gpu, combined, vk::MemoryPropertyFlagBits::eDeviceLocal)});
    device->bindBufferMemory(model_buffers.vertices.get(), shared_ibo_vbo_ind_mem.get(), 0);
    device->bindBufferMemory(model_buffers.indices.get(), shared_ibo_vbo_ind_mem.get(), vbo_req.size);
    device->bindBufferMemory(model_buffers.indirect.get(), shared_ibo_vbo_ind_mem.get(), vbo_req.size + ibo_req.size);

    /********************************************** Model data transfer *****************************************************/
    // transfer everything at once
    // Scoped, such that resources are being freed afterwards.
    {
        vk::BufferCreateInfo transfer_src;
        transfer_src.size                                = combined.size;
        transfer_src.queueFamilyIndexCount               = 1;
        transfer_src.pQueueFamilyIndices                 = &families[fam::transfer];
        transfer_src.sharingMode                         = vk::SharingMode::eExclusive;
        transfer_src.usage                               = vk::BufferUsageFlagBits::eTransferSrc;
        vk::UniqueBuffer             transfer_src_buffer = device->createBufferUnique(transfer_src);
        const vk::MemoryRequirements transfer_req        = device->getBufferMemoryRequirements(transfer_src_buffer.get());
        vk::UniqueDeviceMemory       tf_src_mem          = device->allocateMemoryUnique(
                {transfer_req.size, memory_index(gpu, transfer_req, vk::MemoryPropertyFlagBits::eHostVisible)});
        device->bindBufferMemory(transfer_src_buffer.get(), tf_src_mem.get(), 0);

        // Copy to staging buffer
        std::byte* mapped = static_cast<std::byte*>(device->mapMemory(tf_src_mem.get(), 0, transfer_req.size, {}));
        // Vertices
        memcpy(mapped, vertices.data(), vertices.size() * sizeof(gfx::vertex3d));
        // Indices
        memcpy(mapped + vbo_req.size, indices.data(), indices.size() * sizeof(gfx::index32));
        // Indirect commands
        memcpy(mapped + vbo_req.size + ibo_req.size, mesh_infos.data(), mesh_infos.size() * sizeof(mesh_info));
        device->flushMappedMemoryRanges(vk::MappedMemoryRange(tf_src_mem.get(), 0, transfer_src.size));
        device->unmapMemory(tf_src_mem.get());

        const vk::CommandBufferAllocateInfo tfbufalloc(command_pools[fam::transfer].get(), vk::CommandBufferLevel::ePrimary, 1);
        vk::UniqueCommandBuffer             transfer_command = std::move(device->allocateCommandBuffersUnique(tfbufalloc)[0]);
        vk::CommandBufferBeginInfo          transfer_begin;
        transfer_begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        transfer_command->begin(transfer_begin);
        {
            vk::BufferCopy copy;
            copy.srcOffset = 0;
            copy.dstOffset = 0;
            copy.size      = vbo_req.size;
            transfer_command->copyBuffer(transfer_src_buffer.get(), model_buffers.vertices.get(), copy);
            copy.srcOffset = vbo_req.size;
            copy.size      = ibo_req.size;
            transfer_command->copyBuffer(transfer_src_buffer.get(), model_buffers.indices.get(), copy);
            copy.srcOffset = vbo_req.size + ibo_req.size;
            copy.size      = ind_req.size;
            transfer_command->copyBuffer(transfer_src_buffer.get(), model_buffers.indirect.get(), copy);
        }
        transfer_command->end();
        vk::SubmitInfo transfer_submit;
        transfer_submit.commandBufferCount = 1;
        transfer_submit.pCommandBuffers    = &transfer_command.get();
        vk::UniqueFence transfer_fence = device->createFenceUnique({});
        queues[fam::transfer].submit(transfer_submit, transfer_fence.get());
        device->waitForFences(transfer_fence.get(), true, std::numeric_limits<uint64_t>::max());
    }

    /********************************************** Pipelines! *****************************************************/
    create_pipelines();

    /********************************************** Descriptor pool/sets creation *****************************************************/
    std::array<vk::DescriptorPoolSize, 3> pool_sizes{vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 5),
                                                     vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 5),
                                                     vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 16)};

    vk::DescriptorPoolCreateInfo descriptor_pool_create;
    descriptor_pool_create.maxSets       = 16;
    descriptor_pool_create.poolSizeCount = std::size(pool_sizes);
    descriptor_pool_create.pPoolSizes    = std::data(pool_sizes);
    descriptor_pool_create.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    main_descriptor_pool                 = device->createDescriptorPoolUnique(descriptor_pool_create);

    vk::DescriptorSetAllocateInfo scene_desc_alloc;
    scene_desc_alloc.descriptorPool     = main_descriptor_pool.get();
    scene_desc_alloc.descriptorSetCount = 1;
    scene_desc_alloc.pSetLayouts        = &descriptor_layouts.scene.get();
    descriptor_sets.scene_info          = std::move(device->allocateDescriptorSetsUnique(scene_desc_alloc)[0]);
    descriptor_sets.light_info          = std::move(device->allocateDescriptorSetsUnique(scene_desc_alloc)[0]);
    scene_desc_alloc.pSetLayouts        = &descriptor_layouts.models.get();
    descriptor_sets.models_info         = std::move(device->allocateDescriptorSetsUnique(scene_desc_alloc)[0]);
    scene_desc_alloc.pSetLayouts        = &descriptor_layouts.shadow_textures.get();
    descriptor_sets.shadow_samplers     = std::move(device->allocateDescriptorSetsUnique(scene_desc_alloc)[0]);

    struct scene_data
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 camera_position;
        glm::uint object_count;
    };
    /********************************************** Camera buffer for main cam *****************************************************/
    vk::BufferCreateInfo scene_buffer_create_info;
    scene_buffer_create_info.pQueueFamilyIndices   = &families[fam::graphics];
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
    scene_data* current_scene_data             = static_cast<scene_data*>(device->mapMemory(scene_memory.get(), 0, sizeof(scene_data), {}));
    current_scene_data->view                   = inverse(camera.transform.matrix());
    current_scene_data->projection             = camera.projection;
    current_scene_data->camera_position        = camera.transform.position;
    current_scene_data->object_count           = mesh_infos.size();
    device->flushMappedMemoryRanges(vk::MappedMemoryRange(scene_memory.get(), 0, sizeof(scene_data)));

    /********************************************** Camera buffer for light *****************************************************/
    vk::BufferCreateInfo light_buffer_create_info;
    light_buffer_create_info.pQueueFamilyIndices   = &families[fam::graphics];
    light_buffer_create_info.queueFamilyIndexCount = 1;
    light_buffer_create_info.sharingMode           = vk::SharingMode::eExclusive;
    light_buffer_create_info.usage                 = vk::BufferUsageFlagBits::eUniformBuffer;
    light_buffer_create_info.size                  = sizeof(scene_data);
    vk::UniqueBuffer             light_buffer      = device->createBufferUnique(light_buffer_create_info);
    const vk::MemoryRequirements light_buf_req     = device->getBufferMemoryRequirements(light_buffer.get());
    vk::UniqueDeviceMemory       light_memory      = device->allocateMemoryUnique(
            {light_buf_req.size,
             memory_index(gpu, light_buf_req, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)});
    device->bindBufferMemory(light_buffer.get(), light_memory.get(), 0);

    gfx::camera light_camera;
    light_camera.projection             = gfx::projection(glm::radians(70.f), 256, 256, 0.01f, 100.f, true, true);
    light_camera.transform              = inverse(glm::lookAt(glm::vec3(1, 17, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    scene_data* current_light_data      = static_cast<scene_data*>(device->mapMemory(light_memory.get(), 0, sizeof(scene_data), {}));
    current_light_data->view            = inverse(light_camera.transform.matrix());
    current_light_data->projection      = light_camera.projection;
    current_light_data->camera_position = light_camera.transform.position;
    current_light_data->object_count    = mesh_infos.size();
    device->flushMappedMemoryRanges(vk::MappedMemoryRange(light_memory.get(), 0, sizeof(scene_data)));

    /********************************************** Descriptor set updates *****************************************************/
    vk::DescriptorBufferInfo scene_buffer_info;
    scene_buffer_info.buffer = scene_buffer.get();
    scene_buffer_info.range  = sizeof(scene_data);
    vk::WriteDescriptorSet scene_write;
    scene_write.descriptorCount = 1;
    scene_write.descriptorType  = vk::DescriptorType::eUniformBuffer;
    scene_write.dstArrayElement = 0;
    scene_write.dstBinding      = 0;
    scene_write.dstSet          = descriptor_sets.scene_info.get();
    scene_write.pBufferInfo     = &scene_buffer_info;
    device->updateDescriptorSets(scene_write, nullptr);

    vk::DescriptorBufferInfo ld_buffer_info;
    ld_buffer_info.buffer = light_buffer.get();
    ld_buffer_info.range  = sizeof(scene_data);
    vk::WriteDescriptorSet ld_write;
    ld_write.descriptorCount = 1;
    ld_write.descriptorType  = vk::DescriptorType::eUniformBuffer;
    ld_write.dstArrayElement = 0;
    ld_write.dstBinding      = 0;
    ld_write.dstSet          = descriptor_sets.light_info.get();
    ld_write.pBufferInfo     = &ld_buffer_info;
    device->updateDescriptorSets(ld_write, nullptr);

    vk::DescriptorBufferInfo models_buffer_info;
    models_buffer_info.buffer = model_buffers.indirect.get();
    models_buffer_info.range  = mesh_infos.size() * sizeof(mesh_info);
    vk::WriteDescriptorSet models_write;
    models_write.descriptorCount = 1;
    models_write.descriptorType  = vk::DescriptorType::eStorageBuffer;
    models_write.dstArrayElement = 0;
    models_write.dstBinding      = 0;
    models_write.dstSet          = descriptor_sets.models_info.get();
    models_write.pBufferInfo     = &models_buffer_info;
    device->updateDescriptorSets(models_write, nullptr);

    vk::DescriptorImageInfo shadow_samplers_info;
    shadow_samplers_info.imageView   = shadow_map_view.get();
    shadow_samplers_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    vk::WriteDescriptorSet shadow_samplers_write;
    shadow_samplers_write.descriptorCount = 1;
    shadow_samplers_write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
    shadow_samplers_write.dstArrayElement = 0;
    shadow_samplers_write.dstBinding      = 0;
    shadow_samplers_write.dstSet          = descriptor_sets.shadow_samplers.get();
    shadow_samplers_write.pImageInfo      = &shadow_samplers_info;
    device->updateDescriptorSets(shadow_samplers_write, nullptr);

    /********************************************** Late setup *****************************************************/
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

    /********************************************** Record main cmd buffer *****************************************************/
    vk::CommandBufferAllocateInfo primary_buffers_allocation;
    primary_buffers_allocation.commandBufferCount         = static_cast<uint32_t>(swapchain_images.size());
    primary_buffers_allocation.level                      = vk::CommandBufferLevel::ePrimary;
    primary_buffers_allocation.commandPool                = command_pools[fam::graphics].get();
    std::vector<vk::UniqueCommandBuffer> primary_commands = device->allocateCommandBuffersUnique(primary_buffers_allocation);
    for(int i = 0; i < swapchain_images.size(); ++i)
    {
        primary_commands[i]->reset({});
        primary_commands[i]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        primary_commands[i]->resetQueryPool(timer_query_pool.get(), 2 * i, 2);
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), 2 * i);

        /********************************************** Frustum culling for light **************************************************/
        vk::BufferMemoryBarrier info_barrier;
        info_barrier.buffer              = model_buffers.indirect.get();
        info_barrier.size                = mesh_infos.size() * sizeof(mesh_info);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.srcQueueFamilyIndex = families[fam::graphics];
        info_barrier.dstQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eComputeShader, {}, {}, info_barrier, {});
        primary_commands[i]->bindPipeline(vk::PipelineBindPoint::eCompute, pipelines.culling.get());
        primary_commands[i]->bindDescriptorSets(
                vk::PipelineBindPoint::eCompute,
                pipelines.layouts.default_mesh.get(),
                0,
                {descriptor_sets.light_info.get(), descriptor_sets.models_info.get(), descriptor_sets.shadow_samplers.get()},
                nullptr);
        primary_commands[i]->dispatch((mesh_infos.size() + 16) / 32, 1, 1);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstQueueFamilyIndex = families[fam::graphics];
        info_barrier.srcQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eAllCommands, {}, {}, info_barrier, {});

        /************************************************* Shadow map rendering *****************************************************/
        vk::RenderPassBeginInfo shadow_pass_begin_info;
        shadow_pass_begin_info.clearValueCount = 1;
        shadow_pass_begin_info.framebuffer     = shadow_map_fbo.get();
        vk::ClearValue depth_clear(vk::ClearDepthStencilValue(0.f, 0));
        shadow_pass_begin_info.pClearValues = &depth_clear;
        shadow_pass_begin_info.renderArea   = vk::Rect2D{{0, 0}, {256, 256}};
        shadow_pass_begin_info.renderPass   = renderpasses.shadow.get();
        primary_commands[i]->beginRenderPass(shadow_pass_begin_info, vk::SubpassContents::eInline);
        primary_commands[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.shadow_pass.get());
        primary_commands[i]->bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipelines.layouts.default_mesh.get(),
                0,
                {descriptor_sets.light_info.get(), descriptor_sets.models_info.get(), descriptor_sets.shadow_samplers.get()},
                nullptr);
        primary_commands[i]->bindVertexBuffers(0, model_buffers.vertices.get(), 0ull);
        primary_commands[i]->bindIndexBuffer(model_buffers.indices.get(), 0ull, vk::IndexType::eUint32);
        primary_commands[i]->drawIndexedIndirect(
                model_buffers.indirect.get(), offsetof(mesh_info, indirect), mesh_infos.size(), sizeof(mesh_info));
        primary_commands[i]->endRenderPass();

        /********************************************** Frustum culling for main *****************************************************/
        info_barrier.buffer              = model_buffers.indirect.get();
        info_barrier.size                = mesh_infos.size() * sizeof(mesh_info);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.srcQueueFamilyIndex = families[fam::graphics];
        info_barrier.dstQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eComputeShader, {}, {}, info_barrier, {});
        primary_commands[i]->bindPipeline(vk::PipelineBindPoint::eCompute, pipelines.culling.get());
        primary_commands[i]->bindDescriptorSets(
                vk::PipelineBindPoint::eCompute,
                pipelines.layouts.default_mesh.get(),
                0,
                {descriptor_sets.scene_info.get(), descriptor_sets.models_info.get(), descriptor_sets.shadow_samplers.get()},
                nullptr);
        primary_commands[i]->dispatch((mesh_infos.size() + 16) / 32, 1, 1);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstQueueFamilyIndex = families[fam::graphics];
        info_barrier.srcQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eAllCommands, {}, {}, info_barrier, {});

        /**************************************************** Main render pass ********************************************************/
        std::vector<vk::ClearValue> clear_values{vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.3f, 0.4f, 1.f})),
                                                 vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.3f, 0.4f, 1.f})),
                                                 vk::ClearValue(vk::ClearDepthStencilValue(0.f, 0))};
        vk::RenderPassBeginInfo     begin;
        begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
        begin.pClearValues    = clear_values.data();
        begin.framebuffer     = main_framebuffers[i].get();
        begin.renderArea      = vk::Rect2D({0, 0}, {1280, 720});
        begin.renderPass      = renderpasses.forward.get();
        primary_commands[i]->beginRenderPass(begin, vk::SubpassContents::eInline);
        primary_commands[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.forward_shade.get());
        primary_commands[i]->bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipelines.layouts.default_mesh.get(),
                0,
                {descriptor_sets.scene_info.get(), descriptor_sets.models_info.get(), descriptor_sets.shadow_samplers.get()},
                nullptr);
        primary_commands[i]->setViewport(0, vk::Viewport(0, 0, 1280, 720, 0.f, 1.f));
        primary_commands[i]->setScissor(0, vk::Rect2D({0, 0}, {1280, 720}));
        primary_commands[i]->bindVertexBuffers(0, model_buffers.vertices.get(), 0ull);
        primary_commands[i]->bindIndexBuffer(model_buffers.indices.get(), 0ull, vk::IndexType::eUint32);
        primary_commands[i]->drawIndexedIndirect(
                model_buffers.indirect.get(), offsetof(mesh_info, indirect), mesh_infos.size(), sizeof(mesh_info));
        primary_commands[i]->endRenderPass();
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), 2 * i + 1);
        primary_commands[i]->end();
    }

    /**************************************************** Main loop ********************************************************/
    gfx::camera_controller ctrl(window);
    while(window->update())
    {
        // TODO: Not "stable"! double buffer or something.
        ctrl.update(camera);
        current_scene_data->view            = inverse(camera.transform.matrix());
        current_scene_data->projection      = camera.projection;
        current_scene_data->camera_position = camera.transform.position;
        device->flushMappedMemoryRanges(vk::MappedMemoryRange(scene_memory.get(), 0, sizeof(scene_data)));

        uint32_t image = device->acquireNextImageKHR(*swapchain, std::numeric_limits<uint64_t>::max(), *swap_semaphore, nullptr).value;
        device->waitForFences(*render_fences[image], true, std::numeric_limits<uint64_t>::max());
        device->resetFences(*render_fences[image]);

        std::array<vk::Semaphore, 1>          wait_semaphores{swap_semaphore.get()};
        std::array<vk::PipelineStageFlags, 2> wait_masks{vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                         vk::PipelineStageFlagBits::eComputeShader};
        vk::SubmitInfo                        submit;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &primary_commands[image].get();
        submit.pWaitSemaphores      = std::data(wait_semaphores);
        submit.waitSemaphoreCount   = std::size(wait_semaphores);
        submit.pSignalSemaphores    = &render_semaphore.get();
        submit.signalSemaphoreCount = 1;
        submit.pWaitDstStageMask    = std::data(wait_masks);
        queues[fam::graphics].submit(submit, render_fences[image].get());

        vk::PresentInfoKHR present_info;
        present_info.pImageIndices      = &image;
        present_info.pSwapchains        = &swapchain.get();
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &render_semaphore.get();
        present_info.waitSemaphoreCount = 1;
        vk::Result present_result       = queues[fam::present].presentKHR(present_info);

        /**************************************************** Timer output ********************************************************/
        uint64_t times[2];
        device->getQueryPoolResults(
                timer_query_pool.get(), 2 * image, 2, 2 * sizeof(uint64_t), times, sizeof(uint64_t), vk::QueryResultFlagBits::eWait);
        const auto diff = ((times[1] - times[0]) / 1'000'000.0);
        log_i << diff << "ms => " << (1000 / diff) << "fps";
    }
    device->waitIdle();
}

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

void create_pipelines()
{
    vk::DescriptorSetLayoutBinding scene_matrix_binding;
    scene_matrix_binding.binding         = 0;
    scene_matrix_binding.descriptorCount = 1;
    scene_matrix_binding.descriptorType  = vk::DescriptorType::eUniformBuffer;
    scene_matrix_binding.stageFlags =
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
    vk::DescriptorSetLayoutCreateInfo scene_matrix_layout_info;
    scene_matrix_layout_info.bindingCount = 1;
    scene_matrix_layout_info.pBindings    = &scene_matrix_binding;
    descriptor_layouts.scene              = device->createDescriptorSetLayoutUnique(scene_matrix_layout_info);

    vk::DescriptorSetLayoutBinding models_binding;
    models_binding.binding         = 0;
    models_binding.descriptorCount = 1;
    models_binding.descriptorType  = vk::DescriptorType::eStorageBuffer;
    models_binding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
    vk::DescriptorSetLayoutCreateInfo models_layout_info;
    models_layout_info.bindingCount = 1;
    models_layout_info.pBindings    = &models_binding;
    descriptor_layouts.models       = device->createDescriptorSetLayoutUnique(models_layout_info);

    vk::SamplerCreateInfo shd_smp_info;
    shd_smp_info.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
    shd_smp_info.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
    shd_smp_info.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
    shd_smp_info.anisotropyEnable        = true;
    shd_smp_info.compareEnable           = true;
    shd_smp_info.compareOp               = vk::CompareOp::eGreaterOrEqual;
    shd_smp_info.magFilter               = vk::Filter::eLinear;
    shd_smp_info.minFilter               = vk::Filter::eLinear;
    shd_smp_info.maxAnisotropy           = 16.f;
    shd_smp_info.maxLod                  = 0;
    shd_smp_info.minLod                  = 0;
    shd_smp_info.mipmapMode              = vk::SamplerMipmapMode::eNearest;
    shd_smp_info.unnormalizedCoordinates = false;
    shadow_sampler                       = device->createSamplerUnique(shd_smp_info);

    vk::DescriptorSetLayoutBinding shadow_textures_binding;
    shadow_textures_binding.binding            = 0;
    shadow_textures_binding.descriptorCount    = 1;
    shadow_textures_binding.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
    shadow_textures_binding.pImmutableSamplers = &shadow_sampler.get();
    shadow_textures_binding.stageFlags         = vk::ShaderStageFlagBits::eFragment;
    vk::DescriptorSetLayoutCreateInfo shadow_desc_set_layout_info;
    shadow_desc_set_layout_info.bindingCount = 1;
    shadow_desc_set_layout_info.pBindings    = &shadow_textures_binding;
    descriptor_layouts.shadow_textures       = device->createDescriptorSetLayoutUnique(shadow_desc_set_layout_info);

    const auto layouts = {descriptor_layouts.scene.get(), descriptor_layouts.models.get(), descriptor_layouts.shadow_textures.get()};
    vk::PipelineLayoutCreateInfo layout_info;
    layout_info.setLayoutCount     = std::size(layouts);
    layout_info.pSetLayouts        = std::data(layouts);
    pipelines.layouts.default_mesh = device->createPipelineLayoutUnique(layout_info);

    vk::GraphicsPipelineCreateInfo pp_info;
    pp_info.renderPass = renderpasses.forward.get();
    pp_info.subpass    = 0;
    pp_info.layout     = pipelines.layouts.default_mesh.get();

    struct stage
    {
        enum
        {
            vertex   = 0,
            fragment = 1
        };
    };
    std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages;

    vk::UniqueShaderModule fragment_shader = create_shader(device, "../shd/fs.frag.spv");
    shader_stages[stage::fragment].module  = fragment_shader.get();
    shader_stages[stage::fragment].pName   = "main";
    shader_stages[stage::fragment].stage   = vk::ShaderStageFlagBits::eFragment;
    vk::UniqueShaderModule vertex_shader   = create_shader(device, "../shd/vs.vert.spv");
    shader_stages[stage::vertex].module    = vertex_shader.get();
    shader_stages[stage::vertex].pName     = "main";
    shader_stages[stage::vertex].stage     = vk::ShaderStageFlagBits::eVertex;

    pp_info.stageCount = std::size(shader_stages);
    pp_info.pStages    = std::data(shader_stages);

    struct attr
    {
        enum
        {
            position = 0,
            uv       = 1,
            normal   = 2
        };
    };
    std::array<vk::VertexInputAttributeDescription, 3> attributes;
    attributes[attr::position].binding  = 0;
    attributes[attr::position].location = attr::position;
    attributes[attr::position].format   = vk::Format::eR32G32B32Sfloat;
    attributes[attr::position].offset   = offsetof(gfx::vertex3d, position);
    attributes[attr::uv].binding  = 0;
    attributes[attr::uv].location = attr::uv;
    attributes[attr::uv].format   = vk::Format::eR32G32Sfloat;
    attributes[attr::uv].offset   = offsetof(gfx::vertex3d, uv);
    attributes[attr::normal].binding  = 0;
    attributes[attr::normal].location = attr::normal;
    attributes[attr::normal].format   = vk::Format::eR32G32B32Sfloat;
    attributes[attr::normal].offset   = offsetof(gfx::vertex3d, normal);

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

    pipelines.forward_shade = device->createGraphicsPipelineUnique(nullptr, pp_info);

    // Shadow map pipeline

    msaa.rasterizationSamples      = vk::SampleCountFlagBits::e1;
    msaa.sampleShadingEnable       = false;
    fragment_shader                = create_shader(device, "../shd/fs_shadow.frag.spv");
    shader_stages[stage::fragment].module = fragment_shader.get();
    vertex_shader                  = create_shader(device, "../shd/vs_shadow.vert.spv");
    shader_stages[stage::vertex].module   = vertex_shader.get();
    blend.attachmentCount          = 0;
    pp_info.renderPass             = renderpasses.shadow.get();
    dynamic.dynamicStateCount      = 0;
    vk::Rect2D scissor{{0, 0}, {256, 256}};
    viewport.pScissors = &scissor;
    vk::Viewport vp{0.f, 0.f, 256.f, 256.f, 0.f, 1.f};
    viewport.pViewports   = &vp;
    pp_info.pDynamicState = nullptr;
    pipelines.shadow_pass = device->createGraphicsPipelineUnique(nullptr, pp_info);

    // View frustum culling compute pipeline

    vk::ComputePipelineCreateInfo cull_pp_info;
    cull_pp_info.layout                = pipelines.layouts.default_mesh.get();
    vk::UniqueShaderModule cull_module = create_shader(device, "../shd/cull.comp.spv");
    cull_pp_info.stage.module          = cull_module.get();
    cull_pp_info.stage.pName           = "main";
    cull_pp_info.stage.stage           = vk::ShaderStageFlagBits::eCompute;
    pipelines.culling                  = device->createComputePipelineUnique(nullptr, cull_pp_info);
}

void create_renderpasses()
{
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
    msaa_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
    msaa_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    msaa_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription depth_attachment;
    depth_attachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth_attachment.initialLayout  = vk::ImageLayout::eUndefined;
    depth_attachment.format         = vk::Format::eD32Sfloat;
    depth_attachment.samples        = vk::SampleCountFlagBits::e8;
    depth_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
    depth_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
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
    renderpasses.forward                = device->createRenderPassUnique(fwd_renderpass_info);

    vk::SubpassDependency shadow_dep;
    shadow_dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;
    shadow_dep.srcAccessMask   = vk::AccessFlagBits(0);
    shadow_dep.srcStageMask    = vk::PipelineStageFlagBits::eAllGraphics;
    shadow_dep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    shadow_dep.dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    shadow_dep.dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    shadow_dep.dstSubpass      = 0;

    depth_attachment.samples = vk::SampleCountFlagBits::e1;

    vk::AttachmentReference shadow_depth_ref(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription  shadow_subpass;
    shadow_subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    shadow_subpass.colorAttachmentCount    = 0;
    shadow_subpass.pDepthStencilAttachment = &shadow_depth_ref;
    vk::RenderPassCreateInfo shadow_renderpass_info;
    shadow_renderpass_info.attachmentCount = 1;
    shadow_renderpass_info.pAttachments    = &depth_attachment;
    shadow_renderpass_info.dependencyCount = 1;
    shadow_renderpass_info.pDependencies   = &shadow_dep;
    shadow_renderpass_info.subpassCount    = 1;
    shadow_renderpass_info.pSubpasses      = &shadow_subpass;
    renderpasses.shadow                    = device->createRenderPassUnique(shadow_renderpass_info);
}