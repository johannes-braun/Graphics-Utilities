#include <GLFW/glfw3.h>
#include <thread>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "camera.hpp"
#include "gfx.core/log.hpp"
#include "gfx.ecs.defaults2/prototype.hpp"
#include "gfx.file/file.hpp"
#include "gfx.math/geometry.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "shaders/shaders.hpp"
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <input_glfw.hpp>
#include <vulkan/vulkan.hpp>

#include "shaders/def.hpp"

namespace impl::vulkan {
ecs_state_t* _current_state;
void         run();
}    // namespace impl::vulkan
std::thread run_vulkan(ecs_state_t& ecs_state)
{
    impl::vulkan::_current_state = &ecs_state;
    return std::thread([] { impl::vulkan::run(); });
}
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
};

std::unique_ptr<vulkan_state_t> vulkan_state;

struct mesh_info
{
    glm::u8vec4 color{255, 50, 0, 255};
};

void                     create_renderpass(gfx::device& gpu, gfx::swapchain& swapchain);
vk::UniquePipelineLayout create_pipeline_layout(gfx::device& gpu);
vk::UniquePipeline       create_pipeline(gfx::device& gpu, vk::PipelineLayout layout);
void                     create_framebuffer(gfx::device& gpu, gfx::swapchain& swapchain);

void run()
{
    GLFWwindow* vulkan_window = [] {
        std::unique_lock lock(_current_state->glfw_mutex);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        return glfwCreateWindow(800, 800, "Vulkan", nullptr, nullptr);
    }();

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

    vk::DescriptorPoolSize               sizes[2]{vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
                                    vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 8)};
    const vk::DescriptorPoolCreateInfo   pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 3, gfx::u32(std::size(sizes)),
                                                 sizes);
    vk::UniqueDescriptorPool             descriptor_pool = gpu.get_device().createDescriptorPoolUnique(pool_info);
    const vk::DescriptorSetLayoutBinding cam_buf_binding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics);
    vulkan_state->cam_buffer_layout = gpu.get_device().createDescriptorSetLayoutUnique({{}, 1, &cam_buf_binding});

    std::vector<vk::DescriptorSetLayoutBinding> model_info_bindings{
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
    vk::UniquePipelineLayout pipeline_layout = create_pipeline_layout(gpu);
    vk::UniquePipeline       pipeline        = create_pipeline(gpu, pipeline_layout.get());

    vk::UniqueDescriptorSet cam_buffer_set = std::move(gpu.get_device().allocateDescriptorSetsUnique(
        vk::DescriptorSetAllocateInfo(descriptor_pool.get(), 1, &vulkan_state->cam_buffer_layout.get()))[0]);

    gfx::buffer<gfx::camera_matrices> camera_buffer(gpu, {gfx::camera_matrices{}});
    const vk::DescriptorBufferInfo    cam_buf(camera_buffer.get_buffer(), 0, sizeof(gfx::camera_matrices));
    const vk::WriteDescriptorSet      cam_buf_write(cam_buffer_set.get(), 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &cam_buf);
    gpu.get_device().updateDescriptorSets(cam_buf_write, nullptr);

    gfx::instance_system<mesh_info> instances(gpu, DEF_use_rt_shadows ? gfx::mesh_allocator_flag::use_bvh : gfx::mesh_allocator_flag{});
    graphics_list.add(instances);

    gfx::glfw_input_system  input(vulkan_window);
    gfx::user_camera_system cam_system(input);
    gfx::ecs::system_list   inputs_list;
    inputs_list.add(input);
    inputs_list.add(cam_system);

    gfx::unique_prototype floor = instances.get_instantiator().allocate_prototype_unique("Floor", gfx::scene_file("models/floor.dae").mesh);
    gfx::unique_prototype bunny = instances.get_instantiator().allocate_prototype_unique("Bunny", gfx::scene_file("models/bunny.obj").mesh);
    gfx::unique_prototype dragon =
        instances.get_instantiator().allocate_prototype_unique("Dragon", gfx::scene_file("models/dragon.obj").mesh);

    auto bunny_entity_0 = _current_state->ecs.create_entity_unique(
        gfx::instance_component<mesh_info>(dragon.get(), {glm::u8vec4(130, 150, 12, 255)}), gfx::transform_component({0, 0, 0}, {3, 3, 3}));
    auto bunny_entity_1 = _current_state->ecs.create_entity_unique(
        gfx::instance_component<mesh_info>(dragon.get(), {glm::u8vec4(124, 88, 132, 255)}), gfx::transform_component({0, 0, 2}, {3, 3, 3}));
    auto bunny_entity_2 = _current_state->ecs.create_entity_unique(
        gfx::instance_component<mesh_info>(dragon.get(), {glm::u8vec4(24, 53, 222, 255)}), gfx::transform_component({2, 0, 2}, {3, 3, 3}));
    auto bunny_entity_3 = _current_state->ecs.create_entity_unique(
        gfx::instance_component<mesh_info>(bunny.get(), {glm::u8vec4(12, 221, 61, 255)}), gfx::transform_component({2, 0, 0}, {1, 1, 1}));
    auto floor_entity_3 = _current_state->ecs.create_entity_unique(
        gfx::instance_component<mesh_info>(floor.get(), {glm::u8vec4(12, 221, 61, 255)}),
        gfx::transform_component({0, -1, 0}, {4, 4, 1}, glm::angleAxis(glm::radians(-90.f), glm::vec3(1, 0, 0))));

    auto user_entity =
        _current_state->ecs.create_entity_shared(gfx::transform_component{glm::vec3{0, 0, 4}, glm::vec3(3)}, gfx::projection_component{},
                                                 gfx::grabbed_cursor_component{}, gfx::camera_controls{});

    vk::Buffer curr_buffers[2]{nullptr, nullptr};
    size_t     curr_buffer_sizes[2]{0, 0};

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

        bunny_entity_2->get<gfx::transform_component>()->rotation =
            glm::angleAxis(float(glfwGetTime() * 1.f), glm::normalize(glm::vec3(2, 1, 6)));

        _current_state->ecs.update(delta, graphics_list);

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
        current.get_command_buffer().begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eSimultaneousUse});

        instances.get_instantiator().swap_buffers(current);

        if (curr_buffers[instances.get_instantiator().instance_buffer_index()]
                != instances.get_instantiator().instances_buffer().get_buffer()
            || curr_buffer_sizes[instances.get_instantiator().instance_buffer_index()]
                   != instances.get_instantiator().instances_buffer().size())
        {
            curr_buffers[instances.get_instantiator().instance_buffer_index()] =
                instances.get_instantiator().instances_buffer().get_buffer();
            curr_buffer_sizes[instances.get_instantiator().instance_buffer_index()] =
                instances.get_instantiator().instances_buffer().size();

            {
                vk::DescriptorBufferInfo info;
                info.buffer = curr_buffers[instances.get_instantiator().instance_buffer_index()];
                info.offset = 0;
                info.range =
                    instances.get_instantiator().instances_buffer().size() * sizeof(gfx::prototype_instantiator<mesh_info>::basic_instance);
                const vk::WriteDescriptorSet updater(model_info_sets[instances.get_instantiator().instance_buffer_index()].get(), 0, 0, 1,
                                                     vk::DescriptorType::eStorageBuffer, nullptr, &info);
                gpu.get_device().updateDescriptorSets(updater, nullptr);
            }
            {
                vk::DescriptorBufferInfo info;
                info.offset = 0;
                vk::WriteDescriptorSet updater(model_info_sets[instances.get_instantiator().instance_buffer_index()].get(),
                                               def::buffer_binding_vertex, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &info);

                updater.dstBinding = def::buffer_binding_vertex;
                info.buffer        = instances.get_mesh_allocator().vertex_buffer().get_buffer();
                info.range         = instances.get_mesh_allocator().vertex_buffer().size() * sizeof(gfx::vertex3d);
                gpu.get_device().updateDescriptorSets(updater, nullptr);

                updater.dstBinding = def::buffer_binding_element;
                info.buffer        = instances.get_mesh_allocator().index_buffer().get_buffer();
                info.range         = instances.get_mesh_allocator().index_buffer().size() * sizeof(gfx::index32);
                gpu.get_device().updateDescriptorSets(updater, nullptr);

                if (DEF_use_rt_shadows)
                {
                    updater.dstBinding = def::buffer_binding_bvh;
                    info.buffer        = instances.get_mesh_allocator().bvh_buffer().get_buffer();
                    info.range         = instances.get_mesh_allocator().bvh_buffer().size() * sizeof(gfx::bvh<3>::node);
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

        const vk::ClearValue clear_values[3]{
            {}, {vk::ClearColorValue(std::array<float, 4>{1.f, 0.3f, 0.f, 1.f})}, {vk::ClearDepthStencilValue(1.f, 0)}};
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
            {cam_buffer_set.get(), model_info_sets[instances.get_instantiator().instance_buffer_index()].get()}, {});
        current.get_command_buffer().bindVertexBuffers(0, instances.get_mesh_allocator().vertex_buffer().get_buffer(), {0ull});
        current.get_command_buffer().bindIndexBuffer(instances.get_mesh_allocator().index_buffer().get_buffer(), 0ull,
                                                     vk::IndexType::eUint32);

        current.get_command_buffer().drawIndexedIndirect(instances.get_instantiator().instances_buffer().get_buffer(), 0,
                                                         instances.get_instantiator().instances_buffer().size(),
                                                         sizeof(gfx::prototype_instantiator<mesh_info>::basic_instance));

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
        _current_state->ecs.update(delta, inputs_list);
        glfwPollEvents();
        return self.value_after(!glfwWindowShouldClose(vulkan_window), update_time_inputs);
    });

    vulkan_graphics_worker.stop_and_wait();

    gpu.get_device().waitIdle();
    vulkan_state.reset();
}

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
    dependency.srcAccessMask   = vk::AccessFlagBits{};
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
    vk::DescriptorSetLayout set_layouts[]{vulkan_state->cam_buffer_layout.get(), vulkan_state->model_info_layout.get()};

    vk::PipelineLayoutCreateInfo info;
    info.pSetLayouts    = set_layouts;
    info.setLayoutCount = std::uint32_t(std::size(set_layouts));
    return gpu.get_device().createPipelineLayoutUnique(info);
}

vk::UniquePipeline create_pipeline(gfx::device& gpu, vk::PipelineLayout layout)
{
    vk::GraphicsPipelineCreateInfo info;

    gfx::shader                       vertex_shader(gpu, gfx::spirv::vkgl::shaders::vs_vert);
    gfx::shader                       fragment_shader(gpu, gfx::spirv::vkgl::shaders::fs_frag);
    vk::PipelineShaderStageCreateInfo stages[2];
    stages[0]       = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertex_shader.get_module(), "main");
    stages[1]       = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragment_shader.get_module(), "main");
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

    vk::DynamicState                   dynamic_states[]{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
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
}    // namespace impl::vulkan