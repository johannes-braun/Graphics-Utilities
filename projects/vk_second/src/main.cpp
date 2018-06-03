#include "init.hpp"
#include <GLFW/glfw3.h>
#include <execution>
#include <fstream>
#include <gfx/camera.hpp>
#include <gfx/file.hpp>
#include <gfx/log.hpp>
#include <numeric>
#include <vulkan/vulkan.hpp>

#include "imgui_impl_glfw_vulkan.h"

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
    vk::UniqueDescriptorSetLayout lights;
} descriptor_layouts;

struct
{
    struct
    {
        vk::UniquePipelineLayout default_mesh;
        vk::UniquePipelineLayout reduced;
    } layouts;

    vk::UniquePipeline forward_shade;
    vk::UniquePipeline shadow_pass;
    vk::UniquePipeline culling;
} pipelines;

struct
{
    vk::UniqueRenderPass forward;
    vk::UniqueRenderPass shadow;
    vk::UniqueRenderPass overlay;
} renderpasses;

std::array<vk::UniqueCommandPool, 3> command_pools;
vk::UniqueDescriptorPool             main_descriptor_pool;

vk::UniqueSampler shadow_sampler;

struct
{
    vk::UniqueDescriptorSet scene_info;
    vk::UniqueDescriptorSet light_info;
    vk::UniqueDescriptorSet models_info;
    vk::UniqueDescriptorSet lights;
} descriptor_sets;

struct
{
    vk::UniqueBuffer indirect;
    vk::UniqueBuffer vertices;
    vk::UniqueBuffer indices;
} model_buffers;

struct tstamp
{
    enum
    {
        frame_begin,
        light_cull,
        light_sm,
        render_cull,
        render,
        frame_end,

        count
    };
};

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

    auto infos            = create_device(instance, gpu, surface);
    device                = std::move(infos.device);
    families              = std::move(infos.queue_families);
    queues[fam::graphics] = device->getQueue(families[fam::graphics], infos.queue_indices[fam::graphics]);
    queues[fam::compute]  = device->getQueue(families[fam::compute], infos.queue_indices[fam::compute]);
    queues[fam::transfer] = device->getQueue(families[fam::transfer], infos.queue_indices[fam::transfer]);
    queues[fam::present]  = device->getQueue(families[fam::present], infos.queue_indices[fam::present]);

    create_renderpasses();

    /********************************************** Swapchain & Main FBO setup *****************************************************/
    const vk::SurfaceCapabilitiesKHR capabilities = gpu.getSurfaceCapabilitiesKHR(surface.get());
    swapchain = create_swapchain(gpu, device, surface, families[fam::present], capabilities.currentExtent);
    std::vector<vk::Image>              swapchain_images = device->getSwapchainImagesKHR(swapchain.get());
    std::vector<vk::UniqueFramebuffer>  main_framebuffers(swapchain_images.size());
    std::vector<vk::UniqueFramebuffer>  gui_framebuffers(swapchain_images.size());
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

        fbc.pAttachments    = &swapchain_image_views[i].get();
        fbc.attachmentCount = 1;
        fbc.renderPass      = renderpasses.overlay.get();
        gui_framebuffers[i] = device->createFramebufferUnique(fbc);
    }

    /********************************************** Shadow map fbo setup *****************************************************/
    vk::ImageCreateInfo shadow_depth_image_create_info;
    shadow_depth_image_create_info.arrayLayers           = 1;
    shadow_depth_image_create_info.extent                = vk::Extent3D{512, 512, 1};
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
    shadow_map_fbo_info.height           = 512;
    shadow_map_fbo_info.width            = 512;
    shadow_map_fbo_info.layers           = 1;
    shadow_map_fbo_info.renderPass       = renderpasses.shadow.get();
    vk::UniqueFramebuffer shadow_map_fbo = device->createFramebufferUnique(shadow_map_fbo_info);

    /********************************************** Command pool creation *****************************************************/
    vk::CommandPoolCreateInfo pool_info;
    pool_info.flags              = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    pool_info.queueFamilyIndex   = families[fam::graphics];
    command_pools[fam::graphics] = device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex   = families[fam::compute];
    command_pools[fam::compute]  = device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex   = families[fam::transfer];
    command_pools[fam::transfer] = device->createCommandPoolUnique(pool_info);

    /********************************************** Model loading *****************************************************/
    gfx::scene_file scene("sponza.obj");
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
        vk::UniqueFence transfer_fence     = device->createFenceUnique({});
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
    scene_desc_alloc.pSetLayouts        = &descriptor_layouts.lights.get();
    descriptor_sets.lights     = std::move(device->allocateDescriptorSetsUnique(scene_desc_alloc)[0]);

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
    struct light_data
    {
        scene_data scene;
        glm::vec3  color{1, 1, 1};
        int shadow_map = -1;
    };
    vk::BufferCreateInfo light_buffer_create_info;
    light_buffer_create_info.pQueueFamilyIndices   = &families[fam::graphics];
    light_buffer_create_info.queueFamilyIndexCount = 1;
    light_buffer_create_info.sharingMode           = vk::SharingMode::eExclusive;
    light_buffer_create_info.usage                 = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eStorageBuffer;
    light_buffer_create_info.size                  = 1 * sizeof(light_data);
    vk::UniqueBuffer             light_buffer      = device->createBufferUnique(light_buffer_create_info);
    const vk::MemoryRequirements light_buf_req     = device->getBufferMemoryRequirements(light_buffer.get());
    vk::UniqueDeviceMemory       light_memory      = device->allocateMemoryUnique(
            {light_buf_req.size,
             memory_index(gpu, light_buf_req, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)});
    device->bindBufferMemory(light_buffer.get(), light_memory.get(), 0);

    gfx::camera light_camera;
    light_camera.projection                   = gfx::projection(glm::radians(70.f), 512, 512, 0.01f, 100.f, true, true);
    light_camera.transform                    = inverse(glm::lookAt(glm::vec3(1, 17, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    light_data* current_light_data            = static_cast<light_data*>(device->mapMemory(light_memory.get(), 0, sizeof(light_data), {}));
    current_light_data->scene.view            = inverse(light_camera.transform.matrix());
    current_light_data->scene.projection      = light_camera.projection;
    current_light_data->scene.camera_position = light_camera.transform.position;
    current_light_data->scene.object_count    = mesh_infos.size();
    current_light_data->color = glm::vec3(8, 7, 6);
    current_light_data->shadow_map = 0;
    device->flushMappedMemoryRanges(vk::MappedMemoryRange(light_memory.get(), 0, sizeof(light_data)));

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
    shadow_samplers_write.dstSet          = descriptor_sets.lights.get();
    shadow_samplers_write.pImageInfo      = &shadow_samplers_info;
    device->updateDescriptorSets(shadow_samplers_write, nullptr);

    vk::DescriptorBufferInfo lights_buffer_info;
    lights_buffer_info.buffer = light_buffer.get();
    lights_buffer_info.range  = 1 * sizeof(light_data);
    vk::WriteDescriptorSet lights_write;
    lights_write.descriptorCount = 1;
    lights_write.descriptorType  = vk::DescriptorType::eStorageBuffer;
    lights_write.dstArrayElement = 0;
    lights_write.dstBinding      = 1;
    lights_write.dstSet          = descriptor_sets.lights.get();
    lights_write.pBufferInfo     = &lights_buffer_info;
    device->updateDescriptorSets(lights_write, nullptr);

    /********************************************** Late setup *****************************************************/
    vk::UniqueSemaphore          swap_semaphore   = device->createSemaphoreUnique({});
    vk::UniqueSemaphore          render_semaphore = device->createSemaphoreUnique({});
    std::vector<vk::UniqueFence> render_fences(swapchain_images.size());
    std::generate(render_fences.begin(), render_fences.end(), [&]() {
        return device->createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    });

    vk::QueryPoolCreateInfo timer_pool_info;
    timer_pool_info.queryType            = vk::QueryType::eTimestamp;
    timer_pool_info.queryCount           = static_cast<uint32_t>(tstamp::count);
    vk::UniqueQueryPool timer_query_pool = device->createQueryPoolUnique(timer_pool_info);

    /********************************************** Record main cmd buffer *****************************************************/
    vk::CommandBufferAllocateInfo primary_buffers_allocation;
    primary_buffers_allocation.commandBufferCount         = static_cast<uint32_t>(swapchain_images.size());
    primary_buffers_allocation.level                      = vk::CommandBufferLevel::ePrimary;
    primary_buffers_allocation.commandPool                = command_pools[fam::graphics].get();
    std::vector<vk::UniqueCommandBuffer> primary_commands = device->allocateCommandBuffersUnique(primary_buffers_allocation);
    std::vector<vk::UniqueCommandBuffer> imgui_commands   = device->allocateCommandBuffersUnique(primary_buffers_allocation);
    for(int i = 0; i < swapchain_images.size(); ++i)
    {
        primary_commands[i]->reset({});
        primary_commands[i]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        primary_commands[i]->resetQueryPool(timer_query_pool.get(), 0, tstamp::count);
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::frame_begin);

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
        primary_commands[i]->bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                pipelines.layouts.reduced.get(),
                                                0,
                                                {descriptor_sets.light_info.get(), descriptor_sets.models_info.get()},
                                                nullptr);
        primary_commands[i]->dispatch((mesh_infos.size() + 16) / 32, 1, 1);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstQueueFamilyIndex = families[fam::graphics];
        info_barrier.srcQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eAllCommands, {}, {}, info_barrier, {});
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::light_cull);

        /************************************************* Shadow map rendering *****************************************************/
        vk::RenderPassBeginInfo shadow_pass_begin_info;
        shadow_pass_begin_info.clearValueCount = 1;
        shadow_pass_begin_info.framebuffer     = shadow_map_fbo.get();
        vk::ClearValue depth_clear(vk::ClearDepthStencilValue(0.f, 0));
        shadow_pass_begin_info.pClearValues = &depth_clear;
        shadow_pass_begin_info.renderArea   = vk::Rect2D{{0, 0}, {512, 512}};
        shadow_pass_begin_info.renderPass   = renderpasses.shadow.get();
        primary_commands[i]->beginRenderPass(shadow_pass_begin_info, vk::SubpassContents::eInline);
        primary_commands[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.shadow_pass.get());
        primary_commands[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                pipelines.layouts.reduced.get(),
                                                0,
                                                {descriptor_sets.light_info.get(), descriptor_sets.models_info.get()},
                                                nullptr);
        primary_commands[i]->bindVertexBuffers(0, model_buffers.vertices.get(), 0ull);
        primary_commands[i]->bindIndexBuffer(model_buffers.indices.get(), 0ull, vk::IndexType::eUint32);
        primary_commands[i]->drawIndexedIndirect(
                model_buffers.indirect.get(), offsetof(mesh_info, indirect), mesh_infos.size(), sizeof(mesh_info));
        primary_commands[i]->endRenderPass();
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::light_sm);

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
        primary_commands[i]->bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                pipelines.layouts.reduced.get(),
                                                0,
                                                {descriptor_sets.scene_info.get(), descriptor_sets.models_info.get()},
                                                nullptr);
        primary_commands[i]->dispatch((mesh_infos.size() + 16) / 32, 1, 1);
        info_barrier.srcAccessMask       = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstAccessMask       = vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eShaderRead;
        info_barrier.dstQueueFamilyIndex = families[fam::graphics];
        info_barrier.srcQueueFamilyIndex = families[fam::compute];
        primary_commands[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eAllCommands, {}, {}, info_barrier, {});
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::render_cull);

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
                {descriptor_sets.scene_info.get(), descriptor_sets.models_info.get(), descriptor_sets.lights.get()},
                nullptr);
        primary_commands[i]->setViewport(0, vk::Viewport(0, 0, 1280, 720, 0.f, 1.f));
        primary_commands[i]->setScissor(0, vk::Rect2D({0, 0}, {1280, 720}));
        primary_commands[i]->bindVertexBuffers(0, model_buffers.vertices.get(), 0ull);
        primary_commands[i]->bindIndexBuffer(model_buffers.indices.get(), 0ull, vk::IndexType::eUint32);
        primary_commands[i]->drawIndexedIndirect(
                model_buffers.indirect.get(), offsetof(mesh_info, indirect), mesh_infos.size(), sizeof(mesh_info));
        primary_commands[i]->endRenderPass();
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::render);
        primary_commands[i]->writeTimestamp(vk::PipelineStageFlagBits::eAllGraphics, timer_query_pool.get(), tstamp::frame_end);
        primary_commands[i]->end();
    }

    /**************************************************** ImGui init ********************************************************/
    ImGui_ImplGlfwVulkan_Init_Data imgui_init;
    imgui_init.gpu             = static_cast<VkPhysicalDevice>(gpu);
    imgui_init.device          = static_cast<VkDevice>(device.get());
    imgui_init.descriptor_pool = static_cast<VkDescriptorPool>(main_descriptor_pool.get());
    imgui_init.render_pass     = static_cast<VkRenderPass>(renderpasses.overlay.get());
    ImGuiContext* imctx        = ImGui::CreateContext();
    ImGui::SetCurrentContext(imctx);

    auto atlas = ImGui::GetIO().Fonts;
    // Default Font
    gfx::file font_file("ui/fonts/Ruda-Bold.ttf");
    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 12);
    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 10);
    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 14);
    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 18);

    atlas->Build();
    ImGui::GetIO().FontDefault = atlas->Fonts[0];
    ImGui_ImplGlfwVulkan_Init(*window, true, &imgui_init);

    {
        vk::CommandBufferAllocateInfo alloc;
        alloc.commandBufferCount           = 1;
        alloc.commandPool                  = command_pools[fam::transfer].get();
        alloc.level                        = vk::CommandBufferLevel::ePrimary;
        vk::UniqueCommandBuffer imfont_cmd = std::move(device->allocateCommandBuffersUnique(alloc)[0]);

        vk::UniqueFence imfence = device->createFenceUnique({});

        vk::CommandBufferBeginInfo begin;
        begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        imfont_cmd->begin(begin);
        ImGui_ImplGlfwVulkan_CreateFontsTexture(static_cast<VkCommandBuffer>(imfont_cmd.get()));
        imfont_cmd->end();

        vk::SubmitInfo submit;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers    = &imfont_cmd.get();
        queues[fam::transfer].submit(submit, imfence.get());
        device->waitForFences(imfence.get(), true, std::numeric_limits<uint64_t>::max());
    }

    /************************************************* Asynchronous time provider ******************************************************/
    struct time_printer
    {
        void operator()()
        {
            while(!stop)
            {
                using namespace std::chrono_literals;
                waitfor(500ms);

                std::unique_lock<std::mutex> lock(mtx);
                submits = 0;

                fixed_stamps = stamps;
                if(print.load())
                {
                    log_h << "--- Time stamps ---";
                    for(int i = tstamp::frame_begin + 1; i < tstamp::count; ++i)
                    {
                        log_i << "    " << stamp_names[i] << ": " << stamps[i] << "ms  ---  delta: " << (stamps[i] - stamps[i - 1]) << "ms";
                    }
                }
            }
            ended = true;
        }

        void end()
        {
            stop = true;
            while(!ended.load())
                ;
        }

        std::array<double, tstamp::count> get() const
        {
            std::unique_lock<std::mutex> lock(mtx);
            return fixed_stamps;
        }

        void waitfor(const std::chrono::high_resolution_clock::duration& time)
        {
            auto then = std::chrono::high_resolution_clock::now();
            while(std::chrono::high_resolution_clock::now() - then < time)
                ;
        }

        void submit(double ms, int stamp)
        {
            std::unique_lock<std::mutex> lock(mtx);
            if(submits == 0)
                stamps[stamp] = ms;
            else
                stamps[stamp] = glm::mix(stamps[stamp], ms, submits / static_cast<double>(submits + 1));
        }

        const std::array<const char*, tstamp::count> stamp_names = []() {
            std::array<const char*, tstamp::count> n;
            n[tstamp::frame_begin] = "frame_begin";
            n[tstamp::light_cull]  = "light_cull";
            n[tstamp::light_sm]    = "light_sm";
            n[tstamp::render_cull] = "render_cull";
            n[tstamp::render]      = "render";
            n[tstamp::frame_end]   = "frame_end";
            return n;
        }();
        std::atomic_bool                  print = false;
        std::atomic_bool                  stop  = false;
        mutable std::mutex                mtx;
        int                               submits = 0;
        std::array<double, tstamp::count> stamps;
        std::array<double, tstamp::count> fixed_stamps;

    private:
        std::atomic_bool ended = false;
    };
    time_printer printer;
    std::thread  printer_thread([&]() { printer(); });
    printer_thread.detach();

    using namespace std::chrono_literals;
    int max_framerate = 120;

    /**************************************************** Main loop ********************************************************/
    gfx::camera_controller ctrl(window);
    while(window->update())
    {
        ImGui_ImplGlfwVulkan_NewFrame();

        auto stamps = printer.get();
        ImGui::Begin("Window");

        ImGui::Text("Time stamps");
        for(int i = tstamp::frame_begin + 1; i < tstamp::count; ++i)
        {
            ImGui::Text("%s: %.4fms, delta: %.4fms", printer.stamp_names[i], stamps[i], (stamps[i] - stamps[i - 1]));
        }

        if(ImGui::DragInt("Max Framerate", &max_framerate, 0.1f, 25, 80000))
            window->set_max_framerate(max_framerate);

        ImGui::End();

        // TODO: Not "stable"! double buffer or something.
        ctrl.update(camera);
        current_scene_data->view            = inverse(camera.transform.matrix());
        current_scene_data->projection      = camera.projection;
        current_scene_data->camera_position = camera.transform.position;
        device->flushMappedMemoryRanges(vk::MappedMemoryRange(scene_memory.get(), 0, sizeof(scene_data)));

        uint32_t image = device->acquireNextImageKHR(*swapchain, std::numeric_limits<uint64_t>::max(), *swap_semaphore, nullptr).value;
        // Wait until last frame using this image has finished rendering
        device->waitForFences(*render_fences[image], true, std::numeric_limits<uint64_t>::max());
        device->resetFences(*render_fences[image]);

        // Render imgui
        imgui_commands[image]->reset({});
        imgui_commands[image]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        vk::RenderPassBeginInfo     begin;
        std::vector<vk::ClearValue> clear_values{vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.3f, 0.4f, 1.f}))};
        begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
        begin.pClearValues    = clear_values.data();
        begin.framebuffer     = gui_framebuffers[image].get();
        begin.renderArea      = vk::Rect2D({0, 0}, {1280, 720});
        begin.renderPass      = renderpasses.overlay.get();
        imgui_commands[image]->beginRenderPass(begin, vk::SubpassContents::eInline);
        ImGui_ImplGlfwVulkan_Render(imgui_commands[image].get().operator VkCommandBuffer());
        imgui_commands[image]->endRenderPass();
        imgui_commands[image]->end();

        std::array<vk::Semaphore, 1>          wait_semaphores{swap_semaphore.get()};
        std::array<vk::PipelineStageFlags, 1> wait_masks{vk::PipelineStageFlagBits::eColorAttachmentOutput};
        std::array<vk::CommandBuffer, 2>      command_buffers{primary_commands[image].get(), imgui_commands[image].get()};
        vk::SubmitInfo                        submit;
        submit.commandBufferCount   = std::size(command_buffers);
        submit.pCommandBuffers      = std::data(command_buffers);
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

        std::array<uint64_t, tstamp::count> time_stamps{0};
        device->getQueryPoolResults(timer_query_pool.get(),
                                    0,
                                    std::size(time_stamps),
                                    std::size(time_stamps) * sizeof(uint64_t),
                                    time_stamps.data(),
                                    sizeof(uint64_t),
                                    vk::QueryResultFlagBits::eWait);

        for(int i = tstamp::frame_begin + 1; i < tstamp::count; ++i)
        {
            const auto diff = (time_stamps[i] - time_stamps[tstamp::frame_begin]) / 1'000'000.0;
            printer.submit(diff, i);
        }
    }
    printer.end();
    device->waitIdle();
    ImGui::DestroyContext(imctx);
    ImGui_ImplGlfwVulkan_Shutdown();
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
    
    vk::DescriptorSetLayoutBinding lights_binding;
    lights_binding.binding            = 1;
    lights_binding.descriptorCount    = 1;
    lights_binding.descriptorType     = vk::DescriptorType::eStorageBuffer;
    lights_binding.stageFlags         = vk::ShaderStageFlagBits::eFragment;

    const std::array<vk::DescriptorSetLayoutBinding, 2> lights_bindings = {shadow_textures_binding, lights_binding};
    vk::DescriptorSetLayoutCreateInfo shadow_desc_set_layout_info;
    shadow_desc_set_layout_info.bindingCount = std::size(lights_bindings);
    shadow_desc_set_layout_info.pBindings    = std::data(lights_bindings);
    descriptor_layouts.lights       = device->createDescriptorSetLayoutUnique(shadow_desc_set_layout_info);

    vk::PipelineLayoutCreateInfo layout_info;
    const auto reduced_layouts = {descriptor_layouts.scene.get(), descriptor_layouts.models.get()};
    layout_info.setLayoutCount = std::size(reduced_layouts);
    layout_info.pSetLayouts    = std::data(reduced_layouts);
    pipelines.layouts.reduced  = device->createPipelineLayoutUnique(layout_info);

    const auto layouts = {descriptor_layouts.scene.get(), descriptor_layouts.models.get(), descriptor_layouts.lights.get()};
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
    attributes[attr::uv].binding        = 0;
    attributes[attr::uv].location       = attr::uv;
    attributes[attr::uv].format         = vk::Format::eR32G32Sfloat;
    attributes[attr::uv].offset         = offsetof(gfx::vertex3d, uv);
    attributes[attr::normal].binding    = 0;
    attributes[attr::normal].location   = attr::normal;
    attributes[attr::normal].format     = vk::Format::eR32G32B32Sfloat;
    attributes[attr::normal].offset     = offsetof(gfx::vertex3d, normal);

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

    msaa.rasterizationSamples             = vk::SampleCountFlagBits::e1;
    msaa.sampleShadingEnable              = false;
    fragment_shader                       = create_shader(device, "../shd/fs_shadow.frag.spv");
    shader_stages[stage::fragment].module = fragment_shader.get();
    vertex_shader                         = create_shader(device, "../shd/vs_shadow.vert.spv");
    shader_stages[stage::vertex].module   = vertex_shader.get();
    blend.attachmentCount                 = 0;
    pp_info.renderPass                    = renderpasses.shadow.get();
    dynamic.dynamicStateCount             = 0;
    vk::Rect2D scissor{{0, 0}, {512, 512}};
    viewport.pScissors = &scissor;
    vk::Viewport vp{0.f, 0.f, 512.f, 512.f, 0.f, 1.f};
    viewport.pViewports            = &vp;
    pp_info.pDynamicState          = nullptr;
    raster.depthBiasEnable         = true;
    raster.depthBiasConstantFactor = 1.5f;
    raster.depthBiasSlopeFactor    = -3.14159265359f;
    pp_info.layout                 = pipelines.layouts.reduced.get();
    pipelines.shadow_pass          = device->createGraphicsPipelineUnique(nullptr, pp_info);

    // View frustum culling compute pipeline

    vk::ComputePipelineCreateInfo cull_pp_info;
    cull_pp_info.layout                = pipelines.layouts.default_mesh.get();
    vk::UniqueShaderModule cull_module = create_shader(device, "../shd/cull.comp.spv");
    cull_pp_info.stage.module          = cull_module.get();
    cull_pp_info.stage.pName           = "main";
    cull_pp_info.stage.stage           = vk::ShaderStageFlagBits::eCompute;
    pipelines.culling                  = device->createComputePipelineUnique(nullptr, cull_pp_info);
}

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

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

    // Depth only renderpass (Shadow maps)

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

    // Overlay renderpass (GUI etc.)
    vk::SubpassDependency overlay_dep = shadow_dep;
    overlay_dep.dstAccessMask         = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    vk::SubpassDescription overlay_subpass;
    overlay_subpass.colorAttachmentCount    = 1;
    resolve_attachment_reference.attachment = 0;
    overlay_subpass.pColorAttachments       = &resolve_attachment_reference;
    overlay_subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    color_attachment.loadOp                 = vk::AttachmentLoadOp::eLoad;
    color_attachment.initialLayout          = vk::ImageLayout::ePresentSrcKHR;
    vk::RenderPassCreateInfo overlay_pass_info;
    overlay_pass_info.attachmentCount = 1;
    overlay_pass_info.pAttachments    = &color_attachment;
    overlay_pass_info.dependencyCount = 1;
    overlay_pass_info.pDependencies   = &overlay_dep;
    overlay_pass_info.subpassCount    = 1;
    overlay_pass_info.pSubpasses      = &overlay_subpass;
    renderpasses.overlay              = device->createRenderPassUnique(overlay_pass_info);
}