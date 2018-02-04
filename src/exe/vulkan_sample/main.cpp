#define IO_API_VULKAN
#define IO_EXPOSE_API_FUNCTIONS
#include <io/window.hpp>
#include <io/gui.hpp>
#include <vulkan/logical_device.hpp>
#include <vulkan/swapchain.hpp>
#include <vulkan/framebuffer.hpp>

jpu::ref_ptr<io::window> main_window;
vk::RenderPass msaa_renderpass{ nullptr };
jpu::ref_ptr<vkn::Texture> depth_attachment;
jpu::ref_ptr<vkn::Texture> color_attachment;
std::vector<jpu::ref_ptr<vkn::Framebuffer>> framebuffers;
auto sample_count = vk::SampleCountFlagBits::e4;

void createMultisampleRenderpass();
void createMultisampleFramebuffers();

void rebuildSwapchain()
{
    main_window->rebuild_swapchain();
    createMultisampleFramebuffers();
}

int main(int argc, const char** args)
{
    main_window = jpu::make_ref<io::window>(1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");

    createMultisampleRenderpass();
    createMultisampleFramebuffers();

    auto render_command_buffer = main_window->device()->allocateCommandBuffers(vk::CommandBufferAllocateInfo(
        main_window->device()->commandPool(vk::QueueFlagBits::eGraphics),
        vk::CommandBufferLevel::eSecondary, 1))[0];

    const auto clear_values = {
        vk::ClearValue{ vk::ClearColorValue(std::array<float, 4>{1, 1, 0, 1}) },
        vk::ClearValue{ vk::ClearColorValue() },
        vk::ClearValue{ vk::ClearDepthStencilValue() }
    };

    while(main_window->update())
    {
        ImGui::Begin("Test");
        ImGui::End();

        ImGui::Begin("iasdsads");
        ImGui::Button("Aua");
        ImGui::Text("aspiod");
        ImGui::End();

        // Update...
        vk::CommandBufferInheritanceInfo inheritance(msaa_renderpass);
        render_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse, &inheritance));
        render_command_buffer.end();

        main_window->current_primary_command_buffer().beginRenderPass(vk::RenderPassBeginInfo(msaa_renderpass,
            *framebuffers[main_window->swapchain()->currentImage()],
            vk::Rect2D({ 0, 0 }, main_window->swapchain()->extent()),
            static_cast<uint32_t>(std::size(clear_values)), std::data(clear_values)),
            vk::SubpassContents::eSecondaryCommandBuffers);
        main_window->current_primary_command_buffer().executeCommands({ render_command_buffer });
        main_window->current_primary_command_buffer().endRenderPass();
    }

    main_window->device()->waitIdle();
    main_window->device()->destroyRenderPass(msaa_renderpass);

    return 0;
}

void createMultisampleRenderpass()
{
    if (msaa_renderpass)
        main_window->device()->destroyRenderPass(msaa_renderpass);

    const auto color_attachment_sampled = vk::AttachmentDescription().setFormat(vk::Format::eB8G8R8A8Unorm)
        .setSamples(sample_count)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    const auto color_resolve_attachment = vk::AttachmentDescription().setFormat(vk::Format::eB8G8R8A8Unorm)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    const auto depth_attachment_sampled = vk::AttachmentDescription().setFormat(vk::Format::eD32SfloatS8Uint)
        .setSamples(sample_count)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference color_reference(sample_count == vk::SampleCountFlagBits::e1 ? 1 : 0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference resolve_reference(1, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depth_reference(2, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setPColorAttachments(&color_reference)
        .setPResolveAttachments(sample_count == vk::SampleCountFlagBits::e1 ? nullptr : &resolve_reference)
        .setPDepthStencilAttachment(&depth_reference);

    std::array<vk::SubpassDependency, 2> dependencies;
    dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
        .setDependencyFlags(vk::DependencyFlagBits::eByRegion);
    dependencies[1].setSrcSubpass(0)
        .setDstSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eTopOfPipe)
        .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
        .setDependencyFlags(vk::DependencyFlagBits::eByRegion);

    const auto renderpass_attachments = { color_attachment_sampled, color_resolve_attachment, depth_attachment_sampled };
    vk::RenderPassCreateInfo msaa_render_pass_info;
    msaa_render_pass_info.setAttachmentCount(static_cast<uint32_t>(std::size(renderpass_attachments)))
        .setPAttachments(std::data(renderpass_attachments))
        .setSubpassCount(1)
        .setPSubpasses(&subpass)
        .setDependencyCount(2)
        .setPDependencies(dependencies.data());
    msaa_renderpass = main_window->device()->createRenderPass(msaa_render_pass_info);
}

void createMultisampleFramebuffers()
{
    vk::ImageCreateInfo image_info;
    image_info.setImageType(vk::ImageType::e2D)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(sample_count)
        .setExtent(vk::Extent3D(main_window->swapchain()->extent().width, main_window->swapchain()->extent().height, 1))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    image_info.setFormat(vk::Format::eD32SfloatS8Uint)
        .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    depth_attachment = jpu::make_ref<vkn::Texture>(vkn::TextureCreateInfo(main_window->device()));
    depth_attachment->loadEmpty(image_info);

    image_info.setFormat(vk::Format::eB8G8R8A8Unorm)
        .setUsage(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
    color_attachment = jpu::make_ref<vkn::Texture>(vkn::TextureCreateInfo(main_window->device()));
    color_attachment->loadEmpty(image_info);



    const auto msaa_color_view = jpu::make_ref<vkn::TextureView>(vkn::TextureViewCreateInfo(color_attachment));
    const auto msaa_depth_view = jpu::make_ref<vkn::TextureView>(vkn::TextureViewCreateInfo(depth_attachment,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1)));

    framebuffers.resize(main_window->swapchain()->images().size(), nullptr);
    for (auto i = 0ui64; i < main_window->swapchain()->images().size(); ++i)
    {
        framebuffers[i] = jpu::make_ref<vkn::Framebuffer>(vkn::FramebufferCreateInfo(main_window->device(), msaa_renderpass, main_window->swapchain()->extent(), 1, {
            msaa_color_view, main_window->swapchain()->imageViews()[i], msaa_depth_view
            }));
    }
}
