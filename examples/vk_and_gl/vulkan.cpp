#define GLFW_EXPOSE_NATIVE_WIN32

#include "vulkan.hpp"
#include "camera.hpp"
#include "gfx.core/log.hpp"
#include "gfx.ecs.defaults2/movement.hpp"
#include "gfx.ecs.defaults2/prototype.hpp"
#include "gfx.ecs.defaults2/prototype_proxies.hpp"
#include "gfx.file/file.hpp"
#include "gfx.math/geometry.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "scene.hpp"
#include "shaders/def.hpp"
#include "shaders/shaders.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <input_glfw.hpp>
#include <random>
#include <thread>
#include <vulkan/vulkan.hpp>

namespace impl::vulkan {

struct att_id
{
    enum
    {
        color,
        msaa,
        depth,
        _count
    };
};

struct vulkan_state_t
{
    vk::UniqueRenderPass               render_pass;
    std::vector<vk::UniqueFramebuffer> framebuffers;
    std::vector<gfx::image>            msaa_attachments;
    std::vector<gfx::image>            depth_attachments;
    std::vector<vk::UniqueImageView>   color_att_views;
    std::vector<vk::UniqueImageView>   msaa_att_views;
    std::vector<vk::UniqueImageView>   depth_att_views;
    vk::UniqueDescriptorSetLayout      cam_buffer_layout;
    vk::UniqueDescriptorSetLayout      model_info_layout;

    std::uint32_t                 texture_count;
    vk::UniqueSampler             default_texture_sampler;
    vk::UniqueDescriptorSetLayout all_textures_layout;
};

std::unique_ptr<vulkan_state_t> vulkan_state;

void                     create_renderpass(gfx::device& gpu, gfx::swapchain& swapchain);
vk::UniquePipelineLayout create_pipeline_layout(gfx::device& gpu);
vk::UniquePipeline       create_pipeline(gfx::device& gpu, vk::PipelineLayout layout);
void                     create_framebuffer(gfx::device& gpu, gfx::swapchain& swapchain);
void generate_mipmaps(gfx::device& gpu, vk::Image image, std::uint32_t layers, std::uint32_t levels, vk::Extent3D extent,
                      const gfx::mapped<std::byte>& data);
}    // namespace impl::vulkan

void vulkan_app::on_run()
{
    using namespace impl::vulkan;

    gfx::ecs::ecs ecs;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* vulkan_window = glfwCreateWindow(800, 800, "Vulkan", nullptr, nullptr);

    gfx::instance              vulkan("Vulkan", {1, 0, 0}, true, true);
    gfx::surface               surface(vulkan, glfwGetWin32Window(vulkan_window));
    gfx::device                gpu(vulkan, gfx::device_target::gpu, 1.f, {}, surface);
    gfx::swapchain             surface_swapchain(gpu, surface);
    gfx::semaphore             acquire_semaphore(gpu);
    gfx::semaphore             finish_semaphore(gpu);
    std::vector<gfx::commands> commands = gpu.allocate_graphics_commands(surface_swapchain.count());
    std::vector<gfx::fence>    command_fences;
    for (size_t i = 0; i < surface_swapchain.count(); ++i) command_fences.emplace_back(gpu, true);
    gfx::ecs::system_list graphics_list;

    vulkan_state = std::make_unique<vulkan_state_t>();

    vk::DescriptorPoolSize               sizes[3] {vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
                                     vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 8),
                                     vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 512)};
    const vk::DescriptorPoolCreateInfo   pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 16, gfx::u32(std::size(sizes)),
                                                 sizes);
    vk::UniqueDescriptorPool             descriptor_pool = gpu.get_device().createDescriptorPoolUnique(pool_info);
    const vk::DescriptorSetLayoutBinding cam_buf_binding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics);
    vulkan_state->cam_buffer_layout = gpu.get_device().createDescriptorSetLayoutUnique({{}, 1, &cam_buf_binding});

    std::vector<vk::DescriptorSetLayoutBinding> model_info_bindings {
        {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics},
        {def::buffer_binding_vertex, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics},
        {def::buffer_binding_element, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics}};
    if (DEF_use_rt_shadows)
        model_info_bindings.emplace_back(def::buffer_binding_bvh, vk::DescriptorType::eStorageBuffer, 1,
                                         vk::ShaderStageFlagBits::eAllGraphics);
    vulkan_state->model_info_layout = gpu.get_device().createDescriptorSetLayoutUnique(
        {{}, std::uint32_t(std::size(model_info_bindings)), std::data(model_info_bindings)});

    vk::DescriptorSetLayout              set_layouts[] = {vulkan_state->model_info_layout.get(), vulkan_state->model_info_layout.get()};
    std::vector<vk::UniqueDescriptorSet> model_info_sets =
        gpu.get_device().allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo(descriptor_pool.get(), 2, set_layouts));

    create_renderpass(gpu, surface_swapchain);
    create_framebuffer(gpu, surface_swapchain);

    gfx::impl::vk_proxy<def::mesh_info> vulkan_proxy(gpu);
    gfx::instance_system<def::mesh_info> instances(vulkan_proxy, DEF_use_rt_shadows ? gfx::mesh_allocator_flag::use_bvh : gfx::mesh_allocator_flag {});
    graphics_list.add(instances);

    const auto& scene = scene::current_scene();

    std::vector<gfx::ecs::unique_entity> mesh_entities;
    std::vector<def::mesh_info> mesh_infos;

    std::vector<gfx::image>          textures;
    std::vector<vk::UniqueImageView> texture_views;

    for (size_t i = 0; i < scene.materials.size(); ++i)
    {
        def::mesh_info& info = mesh_infos.emplace_back();
        info.diffuse.color      = 255 * clamp(scene.materials[i].color_diffuse, 0.f, 1.f);
        if (scene.materials[i].texture_diffuse.bytes())
        {
            info.diffuse.texture_id = textures.size();
            const auto& t   = scene.materials[i].texture_diffuse;

            vk::ImageCreateInfo create_info;
            create_info.arrayLayers   = 1;
            create_info.extent        = vk::Extent3D(t.width, t.height, 1);
            create_info.format        = vk::Format::eR8G8B8A8Unorm;
            create_info.imageType     = vk::ImageType::e2D;
            create_info.initialLayout = vk::ImageLayout::eUndefined;
            create_info.mipLevels     = std::uint32_t(1 + floor(log2(std::max(t.width, t.height))));
            create_info.sharingMode   = vk::SharingMode::eExclusive;
            create_info.usage =
                vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
            create_info.tiling = vk::ImageTiling::eOptimal;

            textures.emplace_back(gpu, create_info);

            gfx::mapped<std::byte> data(gpu, gsl::span<std::byte>(static_cast<std::byte*>(t.bytes()), t.width * t.height * t.channels));
            generate_mipmaps(gpu, textures.back().get_image(), create_info.arrayLayers, create_info.mipLevels, create_info.extent, data);

            vk::ImageViewCreateInfo ivi;
            ivi.format   = create_info.format;
            ivi.image    = textures.back().get_image();
            ivi.viewType = vk::ImageViewType::e2D;
            ivi.subresourceRange =
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, create_info.mipLevels, 0, create_info.arrayLayers);
            texture_views.emplace_back(gpu.get_device().createImageViewUnique(ivi));
        }
        if (scene.materials[i].texture_bump.bytes())
        {
            info.bump_map_texture_id = textures.size();
            const auto& t        = scene.materials[i].texture_bump;

            vk::ImageCreateInfo create_info;
            create_info.arrayLayers   = 1;
            create_info.extent        = vk::Extent3D(t.width, t.height, 1);
            create_info.format        = vk::Format::eR8Unorm;
            create_info.imageType     = vk::ImageType::e2D;
            create_info.initialLayout = vk::ImageLayout::eUndefined;
            create_info.mipLevels     = std::uint32_t(1 + floor(log2(std::max(t.width, t.height))));
            create_info.sharingMode   = vk::SharingMode::eExclusive;
            create_info.usage =
                vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
            create_info.tiling = vk::ImageTiling::eOptimal;

            textures.emplace_back(gpu, create_info);

            gfx::mapped<std::byte> data(gpu, gsl::span<std::byte>(static_cast<std::byte*>(t.bytes()), t.width * t.height * t.channels));
            generate_mipmaps(gpu, textures.back().get_image(), create_info.arrayLayers, create_info.mipLevels, create_info.extent, data);

            vk::ImageViewCreateInfo ivi;
            ivi.format   = create_info.format;
            ivi.image    = textures.back().get_image();
            ivi.viewType = vk::ImageViewType::e2D;
            ivi.subresourceRange =
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, create_info.mipLevels, 0, create_info.arrayLayers);
            ivi.components.r = vk::ComponentSwizzle::eR;
            ivi.components.g = vk::ComponentSwizzle::eR;
            ivi.components.b = vk::ComponentSwizzle::eR;
            ivi.components.a = vk::ComponentSwizzle::eR;
            texture_views.emplace_back(gpu.get_device().createImageViewUnique(ivi));
        }
    }
    vulkan_state->texture_count = textures.size();

    vk::SamplerCreateInfo sci;
    sci.addressModeU                      = vk::SamplerAddressMode::eRepeat;
    sci.addressModeV                      = vk::SamplerAddressMode::eRepeat;
    sci.addressModeW                      = vk::SamplerAddressMode::eRepeat;
    sci.anisotropyEnable                  = true;
    sci.magFilter                         = vk::Filter::eLinear;
    sci.minFilter                         = vk::Filter::eLinear;
    sci.maxAnisotropy                     = 16.f;
    sci.maxLod                            = 1000.f;
    sci.minLod                            = -1000.f;
    sci.mipmapMode                        = vk::SamplerMipmapMode::eLinear;
    vulkan_state->default_texture_sampler = gpu.get_device().createSamplerUnique(sci);

    std::vector<vk::Sampler> immutable_samplers(textures.size());
    std::fill(immutable_samplers.begin(), immutable_samplers.end(), vulkan_state->default_texture_sampler.get());
    const vk::DescriptorSetLayoutBinding all_textures_binding(def::texture_binding_all, vk::DescriptorType::eCombinedImageSampler,
                                                              vulkan_state->texture_count, vk::ShaderStageFlagBits::eFragment,
                                                              immutable_samplers.data());
    vulkan_state->all_textures_layout        = gpu.get_device().createDescriptorSetLayoutUnique({{}, 1, &all_textures_binding});
    vk::UniqueDescriptorSet all_textures_set = std::move(gpu.get_device().allocateDescriptorSetsUnique(
        vk::DescriptorSetAllocateInfo(descriptor_pool.get(), 1, &vulkan_state->all_textures_layout.get()))[0]);

    {
        std::vector<vk::DescriptorImageInfo> img_infos(textures.size());

        for (size_t i = 0; i < texture_views.size(); ++i)
        {
            img_infos[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            img_infos[i].imageView   = texture_views[i].get();
        }

        vk::WriteDescriptorSet atw;
        atw.descriptorCount = vulkan_state->texture_count;
        atw.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        atw.dstArrayElement = 0;
        atw.dstBinding      = 0;
        atw.dstSet          = all_textures_set.get();
        atw.pImageInfo      = img_infos.data();
        gpu.get_device().updateDescriptorSets(atw, nullptr);
    }

    vk::UniquePipelineLayout pipeline_layout = create_pipeline_layout(gpu);
    vk::UniquePipeline       pipeline        = create_pipeline(gpu, pipeline_layout.get());

    vk::UniqueDescriptorSet cam_buffer_set = std::move(gpu.get_device().allocateDescriptorSetsUnique(
        vk::DescriptorSetAllocateInfo(descriptor_pool.get(), 1, &vulkan_state->cam_buffer_layout.get()))[0]);

    gfx::buffer<gfx::camera_matrices> camera_buffer(gpu, {gfx::camera_matrices {}});
    const vk::DescriptorBufferInfo    cam_buf(camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
    const vk::WriteDescriptorSet      cam_buf_write(cam_buffer_set.get(), 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &cam_buf);
    gpu.get_device().updateDescriptorSets(cam_buf_write, nullptr);


    gfx::movement_system movement;
    graphics_list.add(movement);

    gfx::glfw_input_system  input(vulkan_window);
    gfx::user_camera_system cam_system(input);
    gfx::ecs::system_list   inputs_list;
    inputs_list.add(input);
    inputs_list.add(cam_system);
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;
    for (size_t i = 0; i < scene.mesh.geometries.size(); ++i)
    { 
        gfx::shared_mesh      mesh = instances.get_mesh_allocator().allocate_mesh(scene.mesh, scene.mesh.geometries[i]);
        gfx::shared_prototype proto = instances.get_instantiator().allocate_prototype("sponza_" + std::to_string(i), {&mesh, 1});

        gfx::instance_component<def::mesh_info> instance_component(std::move(proto), mesh_infos[scene.mesh_material_indices.at(i)]);
        gfx::transform_component                transform_component = scene.mesh.geometries[i].transformation.matrix();
        mesh_entities.emplace_back(ecs.create_entity_unique(instance_component, transform_component));
    }

    auto user_entity = ecs.create_entity_shared(gfx::transform_component {glm::vec3 {0, 0, 4}, glm::vec3(3)}, gfx::projection_component {},
                                                gfx::grabbed_cursor_component {}, gfx::camera_controls {});

    vk::Buffer curr_buffers[2] {nullptr, nullptr};
    size_t     curr_buffer_sizes[2] {0, 0};

    gfx::timed_while::duration vulkan_combined_delta = 0s;
    int                        vulkan_frames         = 0;
    gfx::worker                vulkan_graphics_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        vulkan_combined_delta += delta;
        ++vulkan_frames;
        if (vulkan_combined_delta > 1s)
        {
            gfx::ilog("VULKAN") << std::to_string(vulkan_frames / vulkan_combined_delta.count()) << "fps";
            vulkan_frames         = 0;
            vulkan_combined_delta = 0s;
        }

        ecs.update(delta, graphics_list);

        if (surface_swapchain.swap(acquire_semaphore))
        {
            if (!surface_swapchain.recreate()) return false;
            create_framebuffer(gpu, surface_swapchain);
            return true;
        }

        auto& current = commands[surface_swapchain.current_index()];

        gpu.wait_for({command_fences[surface_swapchain.current_index()]});
        gpu.reset_fences({command_fences[surface_swapchain.current_index()]});

        current.get_command_buffer().reset({});
        current.get_command_buffer().begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eSimultaneousUse});

        vulkan_proxy.swap(current);

        if (curr_buffers[vulkan_proxy.instance_buffer_index()] != vulkan_proxy.instances_buffer().get_buffer()
            || curr_buffer_sizes[vulkan_proxy.instance_buffer_index()] != vulkan_proxy.instances_buffer().size())
        {
            curr_buffers[vulkan_proxy.instance_buffer_index()] = vulkan_proxy.instances_buffer().get_buffer();
            curr_buffer_sizes[vulkan_proxy.instance_buffer_index()] = vulkan_proxy.instances_buffer().size();

            {
                vk::DescriptorBufferInfo info;
                info.buffer = curr_buffers[vulkan_proxy.instance_buffer_index()];
                info.offset = sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance);
                info.range  = vulkan_proxy.instances_buffer().size()
                             * sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance);
                const vk::WriteDescriptorSet updater(model_info_sets[vulkan_proxy.instance_buffer_index()].get(), 0, 0, 1,
                                                     vk::DescriptorType::eStorageBuffer, nullptr, &info);
                gpu.get_device().updateDescriptorSets(updater, nullptr);
            }
            {
                vk::DescriptorBufferInfo info;
                info.offset = sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance);
                vk::WriteDescriptorSet updater(model_info_sets[vulkan_proxy.instance_buffer_index()].get(),
                                               def::buffer_binding_vertex, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &info);
                info.offset = 0;

                if constexpr (DEF_use_rt_shadows)
                {
                    updater.dstBinding = def::buffer_binding_vertex;
                    info.buffer        = vulkan_proxy.vertex_buffer().get_buffer();
                    info.range         = vulkan_proxy.vertex_buffer().size() * sizeof(gfx::vertex3d);
                    gpu.get_device().updateDescriptorSets(updater, nullptr);

                    updater.dstBinding = def::buffer_binding_element;
                    info.buffer        = vulkan_proxy.index_buffer().get_buffer();
                    info.range         = vulkan_proxy.index_buffer().size() * sizeof(gfx::index32);
                    gpu.get_device().updateDescriptorSets(updater, nullptr);

                    updater.dstBinding = def::buffer_binding_bvh;
                    info.buffer        = vulkan_proxy.bvh_buffer().get_buffer();
                    info.range         = vulkan_proxy.bvh_buffer().size() * sizeof(gfx::bvh<3>::node);
                    gpu.get_device().updateDescriptorSets(updater, nullptr);
                }
            }
        }


        user_entity->get<gfx::projection_component>()->perspective().negative_y    = true;
        user_entity->get<gfx::projection_component>()->perspective().inverse_z     = false;
        user_entity->get<gfx::projection_component>()->perspective().screen_width  = surface_swapchain.extent().width;
        user_entity->get<gfx::projection_component>()->perspective().screen_height = surface_swapchain.extent().height;
        const gfx::camera_matrices cam                                             = *gfx::get_camera_info(*user_entity);
        current.get_command_buffer().updateBuffer(camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices), &cam);

        const vk::ClearValue clear_values[3] {
            {}, {vk::ClearColorValue(std::array<float, 4> {1.f, 0.3f, 0.f, 1.f})}, {vk::ClearDepthStencilValue(1.f, 0)}};
        current.get_command_buffer().beginRenderPass(
            vk::RenderPassBeginInfo(vulkan_state->render_pass.get(), vulkan_state->framebuffers[surface_swapchain.current_index()].get(),
                                    vk::Rect2D({0, 0}, surface_swapchain.extent()), gfx::u32(std::size(clear_values)),
                                    std::data(clear_values)),
            vk::SubpassContents::eInline);
        const vk::Viewport viewport(0, 0, surface_swapchain.extent().width, surface_swapchain.extent().height, 0.f, 1.f);
        const vk::Rect2D   scissor({0, 0}, surface_swapchain.extent());
        current.get_command_buffer().setViewport(0, viewport);
        current.get_command_buffer().setScissor(0, scissor);

        current.get_command_buffer().bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
        current.get_command_buffer().bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, pipeline_layout.get(), 0,
            {cam_buffer_set.get(), model_info_sets[vulkan_proxy.instance_buffer_index()].get(), all_textures_set.get()},
            {});
        current.get_command_buffer().bindVertexBuffers(0, vulkan_proxy.vertex_buffer().get_buffer(), {0ull});
        current.get_command_buffer().bindIndexBuffer(vulkan_proxy.index_buffer().get_buffer(), 0ull,
                                                     vk::IndexType::eUint32);

        current.get_command_buffer().drawIndexedIndirect(vulkan_proxy.instances_buffer().get_buffer(),
                                                         sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance),
            std::max(vulkan_proxy.instances_buffer().size(), 1ll) - 1,
                                                         sizeof(gfx::prototype_instantiator<def::mesh_info>::basic_instance));

        current.get_command_buffer().endRenderPass();
        current.get_command_buffer().end();
        gpu.graphics_queue().submit({current}, {acquire_semaphore}, {finish_semaphore}, command_fences[surface_swapchain.current_index()]);

        if (gpu.present_queue().present({{surface_swapchain.current_index(), surface_swapchain}}, {finish_semaphore}))
        {
            if (!surface_swapchain.recreate()) return false;
            create_framebuffer(gpu, surface_swapchain);
            return true;
        }

        return self.value_after(true, update_time_graphics);
    });

    gfx::timed_while::run([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        ecs.update(delta, inputs_list);
        glfwPollEvents();
        return self.value_after(!glfwWindowShouldClose(vulkan_window), update_time_inputs);
    });

    vulkan_graphics_worker.stop_and_wait();

    gpu.get_device().waitIdle();
    vulkan_state.reset();
}

namespace impl::vulkan {
void create_renderpass(gfx::device& gpu, gfx::swapchain& swapchain)
{
    vk::RenderPassCreateInfo info;

    vk::AttachmentDescription attachments[att_id::_count];
    attachments[att_id::color].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[att_id::color].finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    attachments[att_id::color].format         = swapchain.format();
    attachments[att_id::color].samples        = vk::SampleCountFlagBits::e1;
    attachments[att_id::color].loadOp         = vk::AttachmentLoadOp::eDontCare;
    attachments[att_id::color].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[att_id::color].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[att_id::color].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    attachments[att_id::msaa].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[att_id::msaa].finalLayout    = vk::ImageLayout::eColorAttachmentOptimal;
    attachments[att_id::msaa].format         = swapchain.format();
    attachments[att_id::msaa].samples        = vk::SampleCountFlagBits::e4;
    attachments[att_id::msaa].loadOp         = vk::AttachmentLoadOp::eClear;
    attachments[att_id::msaa].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[att_id::msaa].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[att_id::msaa].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    attachments[att_id::depth].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[att_id::depth].finalLayout    = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
    attachments[att_id::depth].format         = vk::Format::eD32Sfloat;
    attachments[att_id::depth].samples        = vk::SampleCountFlagBits::e4;
    attachments[att_id::depth].loadOp         = vk::AttachmentLoadOp::eClear;
    attachments[att_id::depth].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[att_id::depth].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[att_id::depth].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    info.attachmentCount = gfx::u32(std::size(attachments));
    info.pAttachments    = attachments;

    vk::SubpassDependency dependency;
    dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass      = 0;
    dependency.srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe;
    dependency.dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask   = vk::AccessFlagBits {};
    dependency.dstAccessMask   = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    info.dependencyCount = 1;
    info.pDependencies   = &dependency;

    vk::AttachmentReference msaa_attachment(att_id::msaa, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference color_attachment(att_id::color, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depth_attachment(att_id::depth, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &msaa_attachment;
    subpass.pResolveAttachments     = &color_attachment;
    subpass.pDepthStencilAttachment = &depth_attachment;
    subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;

    info.subpassCount = 1;
    info.pSubpasses   = &subpass;

    vulkan_state->render_pass = gpu.get_device().createRenderPassUnique(info);
}

vk::UniquePipelineLayout create_pipeline_layout(gfx::device& gpu)
{
    vk::DescriptorSetLayout set_layouts[] {vulkan_state->cam_buffer_layout.get(), vulkan_state->model_info_layout.get(),
                                           vulkan_state->all_textures_layout.get()};

    vk::PipelineLayoutCreateInfo info;
    info.pSetLayouts    = set_layouts;
    info.setLayoutCount = std::uint32_t(std::size(set_layouts));
    return gpu.get_device().createPipelineLayoutUnique(info);
}

vk::UniquePipeline create_pipeline(gfx::device& gpu, vk::PipelineLayout layout)
{
    vk::GraphicsPipelineCreateInfo info;

    struct
    {
        std::uint32_t texture_count;
    } shader_constants;

    shader_constants.texture_count = vulkan_state->texture_count;

    vk::SpecializationMapEntry spec_entries[1];
    spec_entries[0] = vk::SpecializationMapEntry {def::constant_id_texture_count, offsetof(decltype(shader_constants), texture_count),
                       sizeof(shader_constants.texture_count)};

    vk::SpecializationInfo specialization;
    specialization.mapEntryCount = std::uint32_t(std::size(spec_entries));
    specialization.pMapEntries   = spec_entries;
    specialization.dataSize      = sizeof(shader_constants);
    specialization.pData         = &shader_constants;

    gfx::shader                       vertex_shader(gpu, gfx::spirv::vkgl::shaders::vk_vs_vert);
    gfx::shader                       fragment_shader(gpu, gfx::spirv::vkgl::shaders::vk_fs_frag);
    vk::PipelineShaderStageCreateInfo stages[2];
    stages[0] =
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertex_shader.get_module(), "main", &specialization);
    stages[1] =
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragment_shader.get_module(), "main", &specialization);
    info.stageCount = gfx::u32(std::size(stages));
    info.pStages    = stages;

    info.renderPass = vulkan_state->render_pass.get();
    info.subpass    = 0;

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    vk::PipelineColorBlendAttachmentState def;
    def.blendEnable = false;
    def.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    blend.attachmentCount = 1;
    blend.pAttachments    = &def;
    info.pColorBlendState = &blend;

    vk::PipelineDepthStencilStateCreateInfo depth;
    depth.depthTestEnable   = true;
    depth.depthCompareOp    = vk::CompareOp::eLessOrEqual;
    depth.depthWriteEnable  = true;
    info.pDepthStencilState = &depth;

    vk::DynamicState                   dynamic_states[] {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic;
    dynamic.dynamicStateCount = gfx::u32(std::size(dynamic_states));
    dynamic.pDynamicStates    = dynamic_states;
    info.pDynamicState        = &dynamic;

    vk::PipelineInputAssemblyStateCreateInfo assembly;
    assembly.topology        = vk::PrimitiveTopology::eTriangleList;
    info.pInputAssemblyState = &assembly;

    vk::PipelineMultisampleStateCreateInfo msaa;
    msaa.rasterizationSamples = vk::SampleCountFlagBits::e4;
    info.pMultisampleState    = &msaa;

    vk::PipelineRasterizationStateCreateInfo raster;
    raster.cullMode          = vk::CullModeFlagBits::eBack;
    raster.polygonMode       = vk::PolygonMode::eFill;
    raster.lineWidth         = 1.f;
    info.pRasterizationState = &raster;

    vk::VertexInputAttributeDescription attrs[3];
    attrs[0] = vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(gfx::vertex3d, position));
    attrs[1] = vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(gfx::vertex3d, normal));
    attrs[2] = vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(gfx::vertex3d, uv));
    vk::VertexInputBindingDescription bindings[1];
    bindings[0] = vk::VertexInputBindingDescription(0, sizeof(gfx::vertex3d), vk::VertexInputRate::eVertex);
    vk::PipelineVertexInputStateCreateInfo vertex;
    vertex.vertexAttributeDescriptionCount = gfx::u32(std::size(attrs));
    vertex.vertexBindingDescriptionCount   = gfx::u32(std::size(bindings));
    vertex.pVertexAttributeDescriptions    = attrs;
    vertex.pVertexBindingDescriptions      = bindings;
    info.pVertexInputState                 = &vertex;

    vk::PipelineViewportStateCreateInfo vport;
    vport.scissorCount  = 1;
    vport.viewportCount = 1;
    info.pViewportState = &vport;

    info.layout = layout;

    return gpu.get_device().createGraphicsPipelineUnique(nullptr, info);
}

void create_framebuffer(gfx::device& gpu, gfx::swapchain& swapchain)
{
    gpu.get_device().waitIdle();
    if (vulkan_state->framebuffers.empty())
    {
        const gfx::u32 queue_indices[] = {gpu.graphics_family()};

        vk::ImageCreateInfo depth_info;
        depth_info.arrayLayers           = 1;
        depth_info.extent                = vk::Extent3D(1920, 1080, 1);
        depth_info.format                = vk::Format::eD32Sfloat;
        depth_info.imageType             = vk::ImageType::e2D;
        depth_info.initialLayout         = vk::ImageLayout::eUndefined;
        depth_info.mipLevels             = 1;
        depth_info.samples               = vk::SampleCountFlagBits::e4;
        depth_info.sharingMode           = vk::SharingMode::eExclusive;
        depth_info.tiling                = vk::ImageTiling::eOptimal;
        depth_info.usage                 = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        depth_info.queueFamilyIndexCount = gfx::u32(std::size(queue_indices));
        depth_info.pQueueFamilyIndices   = std::data(queue_indices);

        vk::ImageCreateInfo msaa_info;
        msaa_info.arrayLayers           = 1;
        msaa_info.extent                = vk::Extent3D(1920, 1080, 1);
        msaa_info.format                = swapchain.format();
        msaa_info.imageType             = vk::ImageType::e2D;
        msaa_info.initialLayout         = vk::ImageLayout::eUndefined;
        msaa_info.mipLevels             = 1;
        msaa_info.samples               = vk::SampleCountFlagBits::e4;
        msaa_info.sharingMode           = vk::SharingMode::eExclusive;
        msaa_info.tiling                = vk::ImageTiling::eOptimal;
        msaa_info.usage                 = vk::ImageUsageFlagBits::eColorAttachment;
        msaa_info.queueFamilyIndexCount = gfx::u32(std::size(queue_indices));
        msaa_info.pQueueFamilyIndices   = std::data(queue_indices);

        for (auto i = 0; i < swapchain.count(); ++i)
        {
            const auto& da = vulkan_state->depth_attachments.emplace_back(gpu, depth_info);

            vk::ImageViewCreateInfo dc({}, da.get_image(), vk::ImageViewType::e2D, depth_info.format, {},
                                       vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));
            const auto&             dv = vulkan_state->depth_att_views.emplace_back(gpu.get_device().createImageViewUnique(dc));

            const auto& ma = vulkan_state->msaa_attachments.emplace_back(gpu, msaa_info);

            vk::ImageViewCreateInfo mc({}, ma.get_image(), vk::ImageViewType::e2D, msaa_info.format, {},
                                       vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
            const auto&             mv = vulkan_state->msaa_att_views.emplace_back(gpu.get_device().createImageViewUnique(mc));
        }
    }

    vulkan_state->framebuffers.clear();
    for (auto i = 0; i < swapchain.count(); ++i)
    {
        const auto&               res = swapchain.images()[i].get_image();
        vk::ImageViewCreateInfo   rc({}, res, vk::ImageViewType::e2D, swapchain.format(), {},
                                   vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        const auto&               rv = vulkan_state->color_att_views.emplace_back(gpu.get_device().createImageViewUnique(rc));
        vk::FramebufferCreateInfo info;
        info.renderPass        = vulkan_state->render_pass.get();
        const auto attachments = {rv.get(), vulkan_state->msaa_att_views[i].get(), vulkan_state->depth_att_views[i].get()};
        info.attachmentCount   = gfx::u32(std::size(attachments));
        info.pAttachments      = std::data(attachments);
        info.layers            = 1;
        info.height            = swapchain.extent().height;
        info.width             = swapchain.extent().width;
        vulkan_state->framebuffers.emplace_back(gpu.get_device().createFramebufferUnique(info));
    }
}

void generate_mipmaps(gfx::device& gpu, vk::Image image, std::uint32_t layers, std::uint32_t levels, vk::Extent3D extent,
                      const gfx::mapped<std::byte>& data)
{
    gfx::commands cmd = gpu.allocate_graphics_command();
    gfx::fence    fence(gpu);

    cmd.get_command_buffer().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    {
        vk::ImageMemoryBarrier initial;
        initial.srcAccessMask                   = {};
        initial.dstAccessMask                   = vk::AccessFlagBits::eTransferWrite;
        initial.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        initial.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        initial.image                           = image;
        initial.oldLayout                       = vk::ImageLayout::eUndefined;
        initial.newLayout                       = vk::ImageLayout::eTransferDstOptimal;
        initial.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        initial.subresourceRange.baseArrayLayer = 0;
        initial.subresourceRange.layerCount     = layers;
        initial.subresourceRange.baseMipLevel   = 0;
        initial.subresourceRange.levelCount     = 1;
        cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer,
                                                 vk::DependencyFlagBits::eByRegion, {}, {}, initial);

        vk::BufferImageCopy bic(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), {0, 0, 0}, extent);
        cmd.get_command_buffer().copyBufferToImage(data.get_buffer(), image, vk::ImageLayout::eTransferDstOptimal, bic);
    }


    vk::ImageMemoryBarrier initial;
    initial.srcAccessMask                   = vk::AccessFlagBits::eTransferWrite;
    initial.oldLayout                       = vk::ImageLayout::eTransferDstOptimal;
    initial.dstAccessMask                   = vk::AccessFlagBits::eTransferRead;
    initial.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    initial.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    initial.image                           = image;
    initial.newLayout                       = vk::ImageLayout::eTransferSrcOptimal;
    initial.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    initial.subresourceRange.baseArrayLayer = 0;
    initial.subresourceRange.layerCount     = layers;
    initial.subresourceRange.baseMipLevel   = 0;
    initial.subresourceRange.levelCount     = 1;
    cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer,
                                             vk::DependencyFlagBits::eByRegion, {}, {}, initial);
    initial.srcAccessMask = {};
    initial.oldLayout     = vk::ImageLayout::eUndefined;

    vk::ImageMemoryBarrier ending      = initial;
    ending.subresourceRange.layerCount = 1;
    ending.srcAccessMask               = vk::AccessFlagBits::eTransferWrite;
    ending.oldLayout                   = vk::ImageLayout::eTransferDstOptimal;

    initial.subresourceRange.layerCount = 1;
    initial.srcAccessMask               = {};
    initial.oldLayout                   = vk::ImageLayout::eUndefined;
    initial.dstAccessMask               = vk::AccessFlagBits::eTransferWrite;
    initial.newLayout                   = vk::ImageLayout::eTransferDstOptimal;
    for (std::uint32_t layer = 0; layer < layers; ++layer)
    {
        for (std::uint32_t i = 1; i < levels; ++i)
        {
            initial.subresourceRange.baseArrayLayer = layer;
            initial.subresourceRange.baseMipLevel   = i;
            cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, initial);

            const auto    clm = [](uint32_t i) -> std::int32_t { return static_cast<std::int32_t>(std::max(i, 1u)); };
            vk::ImageBlit blit;
            blit.srcOffsets[0] = VkOffset3D {0, 0, 0};
            blit.srcOffsets[1] = VkOffset3D {clm(extent.width >> (i - 1)), clm(extent.height >> (i - 1)), clm(extent.depth >> (i - 1))};
            blit.dstOffsets[0] = VkOffset3D {0, 0, 0};
            blit.dstOffsets[1] = VkOffset3D {clm(extent.width >> (i)), clm(extent.height >> (i)), clm(extent.depth >> (i))};
            blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            blit.srcSubresource.baseArrayLayer = layer;
            blit.srcSubresource.layerCount     = 1;
            blit.srcSubresource.mipLevel       = i - 1;
            blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            blit.dstSubresource.baseArrayLayer = layer;
            blit.dstSubresource.layerCount     = 1;
            blit.dstSubresource.mipLevel       = i;

            cmd.get_command_buffer().blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal,
                                               blit, vk::Filter::eLinear);

            ending.subresourceRange.baseArrayLayer = layer;
            ending.subresourceRange.baseMipLevel   = i;
            cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, ending);
        }
    }

    initial.srcAccessMask                   = vk::AccessFlagBits::eTransferRead;
    initial.dstAccessMask                   = vk::AccessFlagBits::eShaderRead;
    initial.oldLayout                       = vk::ImageLayout::eTransferSrcOptimal;
    initial.newLayout                       = vk::ImageLayout::eShaderReadOnlyOptimal;
    initial.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    initial.subresourceRange.baseArrayLayer = 0;
    initial.subresourceRange.layerCount     = layers;
    initial.subresourceRange.baseMipLevel   = 0;
    initial.subresourceRange.levelCount     = levels;

    cmd.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands,
                                             vk::DependencyFlagBits::eByRegion, {}, {}, initial);
    cmd.get_command_buffer().end();

    gpu.graphics_queue().submit({cmd}, {}, {}, fence);
    gpu.wait_for({fence});
}

}    // namespace impl::vulkan