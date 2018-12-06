#include <GLFW/glfw3.h>
#include <thread>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "gfx.core/log.hpp"
#include "gfx.math/geometry.hpp"
#include "globals.hpp"
#include "shaders/shaders.hpp"
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <vulkan/vulkan.hpp>

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
vk::UniqueRenderPass  render_pass;
vk::UniqueFramebuffer framebuffer;

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

std::vector<gfx::image>          msaa_attachments;
std::vector<gfx::image>          depth_attachments;
std::vector<vk::UniqueImageView> color_att_views;
std::vector<vk::UniqueImageView> msaa_att_views;
std::vector<vk::UniqueImageView> depth_att_views;

std::vector<vk::UniqueDescriptorSetLayout> mesh_set_layouts;

void               create_renderpass(gfx::device& gpu, gfx::swapchain& swapchain);
vk::UniquePipelineLayout create_pipeline_layout(gfx::device& gpu);
vk::UniquePipeline       create_pipeline(gfx::device& gpu, vk::PipelineLayout layout);

void run()
{
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

    create_renderpass(gpu, surface_swapchain);
    vk::UniquePipelineLayout pipeline_layout = create_pipeline_layout(gpu);
    vk::UniquePipeline       pipeline        = create_pipeline(gpu, pipeline_layout.get());

    gfx::worker::duration vulkan_combined_delta = 0s;
    int                   vulkan_frames         = 0;
    gfx::worker           vulkan_graphics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        vulkan_combined_delta += delta;
        ++vulkan_frames;
        if (vulkan_combined_delta > 1s)
        {
            gfx::ilog("VULKAN") << std::to_string(vulkan_frames / vulkan_combined_delta.count()) << "fps";
            vulkan_frames         = 0;
            vulkan_combined_delta = 0s;
        }

        _current_state->ecs.update(delta, graphics_list);

        if (surface_swapchain.swap(acquire_semaphore))
        {
            if (!surface_swapchain.recreate()) return false;
            return true;
        }

        const auto& current = commands[surface_swapchain.current_index()];

        gpu.wait_for({command_fences[surface_swapchain.current_index()]});
        gpu.reset_fences({command_fences[surface_swapchain.current_index()]});

        current.get_command_buffer().reset({});
        current.get_command_buffer().begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eSimultaneousUse});
        vk::ImageMemoryBarrier make_presentable;
        make_presentable.image         = surface_swapchain.current_image().get_image();
        make_presentable.oldLayout     = vk::ImageLayout::eUndefined;
        make_presentable.newLayout     = vk::ImageLayout::eTransferDstOptimal;
        make_presentable.srcAccessMask = {};
        make_presentable.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
        current.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, make_presentable);
        current.get_command_buffer().clearColorImage(surface_swapchain.current_image().get_image(), vk::ImageLayout::eTransferDstOptimal,
                                                     vk::ClearColorValue(std::array{0.8f, 0.1f, 0.f, 1.f}),
                                                     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        make_presentable.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
        make_presentable.newLayout     = vk::ImageLayout::ePresentSrcKHR;
        make_presentable.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
        make_presentable.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
        current.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, make_presentable);
        current.get_command_buffer().end();
        gpu.graphics_queue().submit({current}, {acquire_semaphore}, {finish_semaphore}, command_fences[surface_swapchain.current_index()]);

        if (gpu.present_queue().present({{surface_swapchain.current_index(), surface_swapchain}}, {finish_semaphore}))
        {
            if (!surface_swapchain.recreate()) return false;
            return true;
        }

        return self.value_after(true, update_time_graphics);
    });

    while (!glfwWindowShouldClose(vulkan_window)) glfwPollEvents();
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
    attachments[att_id::msaa].samples        = vk::SampleCountFlagBits::e8;
    attachments[att_id::msaa].loadOp         = vk::AttachmentLoadOp::eClear;
    attachments[att_id::msaa].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[att_id::msaa].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[att_id::msaa].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    attachments[att_id::depth].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[att_id::depth].finalLayout    = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
    attachments[att_id::depth].format         = swapchain.format();
    attachments[att_id::depth].samples        = vk::SampleCountFlagBits::e8;
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

    render_pass = gpu.get_device().createRenderPassUnique(info);
}

vk::UniquePipelineLayout create_pipeline_layout(gfx::device& gpu)
{
    vk::PipelineLayoutCreateInfo info;

    info.pSetLayouts;
    info.setLayoutCount;

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

    info.renderPass = render_pass.get();
    info.subpass    = 0;

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    vk::PipelineColorBlendAttachmentState def;
    def.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    blend.attachmentCount = 1;
    blend.pAttachments    = &def;
    info.pColorBlendState = &blend;

    vk::PipelineDepthStencilStateCreateInfo depth;
    depth.depthTestEnable   = true;
    depth.depthCompareOp    = vk::CompareOp::eLess;
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
    msaa.rasterizationSamples = vk::SampleCountFlagBits::e8;
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
    vk::VertexInputBindingDescription      bindings[1];
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
}    // namespace impl::vulkan