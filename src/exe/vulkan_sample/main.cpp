#define IO_API_VULKAN
#define IO_EXPOSE_API_FUNCTIONS
#include <io/window.hpp>
#include <vulkan/device.hpp>
#include <vulkan/framebuffer.hpp>
#include "vulkan/pipeline.hpp"
#include "vulkan/pipeline.hpp"
#include "res/vertex.hpp"
#include "res/image.hpp"
#include "io/camera.hpp"
#include "res/presets.hpp"
#include "res/geometry.hpp"
#include "vulkan/buffer.hpp"

jpu::ref_ptr<io::window> main_window;
vk::UniqueRenderPass msaa_renderpass{ nullptr };
jpu::ref_ptr<vkn::texture> depth_attachment;
jpu::ref_ptr<vkn::texture> color_attachment;
std::vector<jpu::ref_ptr<vkn::framebuffer>> framebuffers;
glm::ivec2 full_resolution;
bool is_fullscreen;
auto sample_count = vk::SampleCountFlagBits::e4;
float max_framerate = 60.0;

void createMultisampleRenderpass();
void createMultisampleFramebuffers();

void rebuildSwapchain()
{
    main_window->device()->waitIdle();
    if (is_fullscreen)
    {
        main_window->set_monitor(io::monitor());
    }
    else
    {
        main_window->set_monitor({});
    }
    glfwSetWindowSize(*main_window, full_resolution.x, full_resolution.y);
    main_window->rebuild_swapchain();
    createMultisampleFramebuffers();
    main_window->device()->waitIdle();
}

int main(int argc, const char** args)
{
    full_resolution = { 1280, 720 };
    main_window = jpu::make_ref<io::window>(full_resolution.x, full_resolution.y, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    main_window->limit_framerate(max_framerate);

    glfwSetFramebufferSizeCallback(*main_window, [](GLFWwindow* w, int x, int y)
    {
        full_resolution = { x, y };
        rebuildSwapchain();
    });
    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;
    });

    glfwSetScrollCallback(*main_window, [](GLFWwindow*, double x, double y) {
        main_window->gui()->scrolled(y);
    });

    glfwSetCharCallback(*main_window, [](GLFWwindow*, uint32_t ch) {
        if (main_window->gui()->char_input(static_cast<wchar_t>(ch)))
            return;
    });

    glfwSetMouseButtonCallback(*main_window, [](GLFWwindow*, int btn, int action, int mods) {
        if (main_window->gui()->mouse_button_action(btn, action, mods))
            return;
    });

    createMultisampleRenderpass();
    rebuildSwapchain();

    const auto clear_values = {
        vk::ClearValue{ vk::ClearColorValue(std::array<float, 4>{ 0.7f, 0.8f, 1.f, 1 }) },
        vk::ClearValue{ vk::ClearColorValue(std::array<float, 4>{ 0.7f, 0.8f, 1.f, 1 }) },
        vk::ClearValue{ vk::ClearDepthStencilValue() }
    };

    auto default_sampler_unique = main_window->device()->createSamplerUnique(vk::SamplerCreateInfo({},
        vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eMirroredRepeat, vk::SamplerAddressMode::eMirroredRepeat,
        vk::SamplerAddressMode::eMirroredRepeat, 0, true, 16.f
    ));

    const auto bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
    };
    const auto mesh_push_set_layout = jpu::make_ref<vkn::descriptor_set_layout>(main_window->device(), bindings,
        vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR);

    const auto pipeline_layout = jpu::make_ref<vkn::pipeline_layout>(main_window->device());
    pipeline_layout->add_push_constant_range(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
    pipeline_layout->add_push_constant_range(vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4), sizeof(glm::mat4));
    pipeline_layout->add_descriptor_set_layout(mesh_push_set_layout);
    pipeline_layout->finalize();

    const auto pipeline = jpu::make_ref<vkn::graphics_pipeline>(main_window->device(), pipeline_layout, msaa_renderpass.get(), 0);
    pipeline->use_stages(
        jpu::make_ref<vkn::shader>(main_window->device(), vkn::shader_root / "simple_vk/simple.vert"),
        jpu::make_ref<vkn::shader>(main_window->device(), vkn::shader_root / "simple_vk/simple.frag")
    );
    pipeline->set_vertex_attributes({
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(res::vertex, position)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(res::vertex, uv)),
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(res::vertex, normal))
    });
    pipeline->set_vertex_bindings({ vk::VertexInputBindingDescription(0, sizeof(res::vertex)) });
    pipeline->multisample.setRasterizationSamples(sample_count);
    pipeline->finalize();
    
    auto tex = jpu::make_ref<vkn::texture>(main_window->device());
    int w, h; stbi_ldr_to_hdr_gamma(1.f);
    const auto dat = res::stbi_data(stbi_loadf("../res/grid.jpg", &w, &h, nullptr, 4));
    tex->assign_2d(w, h, vk::Format::eR32G32B32A32Sfloat, w*h * 4 * sizeof(float), dat.get());
    tex->generate_mipmaps();

    auto logo = jpu::make_ref<vkn::texture>(main_window->device());
    const auto logo_dat = res::stbi_data(stbi_load("../res/ui/logo.png", &w, &h, nullptr, 4));
    logo->assign_2d(w, h, vk::Format::eR8G8B8A8Unorm, w*h * 4 * sizeof(uint8_t), logo_dat.get());
    logo->generate_mipmaps();
    const auto logo_view = jpu::make_ref<vkn::texture_view>(logo);

    const auto tex_view = jpu::make_ref<vkn::texture_view>(tex);
    const auto bunny = res::load_geometry("../res/bunny.dae");
    auto&& idx = bunny.meshes.get_by_index(0).indices;
    auto&& vtx = bunny.meshes.get_by_index(0).vertices;

    auto vtx_stagebuffer = jpu::make_ref<vkn::buffer>(main_window->device(), vtx, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
    auto idx_stagebuffer = jpu::make_ref<vkn::buffer>(main_window->device(), idx, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
    auto vtxbuffer = jpu::make_ref<vkn::buffer>(main_window->device(), vtx_stagebuffer->size(),
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
    auto idxbuffer = jpu::make_ref<vkn::buffer>(main_window->device(), idx_stagebuffer->size(),
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vtx_stagebuffer->copy_to(vtxbuffer);
    idx_stagebuffer->copy_to(idxbuffer);

    vk::DescriptorImageInfo tex_info(default_sampler_unique.get(), *tex_view, vk::ImageLayout::eShaderReadOnlyOptimal);
    vk::WriteDescriptorSet tex_set(nullptr, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &tex_info);

    auto render_command_buffer = main_window->device()->allocateCommandBuffers(vk::CommandBufferAllocateInfo(
        main_window->device()->command_pool(vk::QueueFlagBits::eGraphics),
        vk::CommandBufferLevel::eSecondary, 1))[0];

    io::camera cam;
    cam.inverse_z = true;
    cam.negative_y = true;
    io::default_cam_controller ctrl;

    while (main_window->update())
    {
        ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<VkImageView>(static_cast<vk::ImageView>(*logo_view))), ImVec2(32, 32));
        ImGui::SameLine();
        const auto cur_pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cur_pos.x, cur_pos.y + 6));
        ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[3]);
        ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
        ImGui::Text("Graphics Framework");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
        ImGui::Text("V1.0.0");
        ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
        ImGui::Text("%.1f fps at %.3f ms/f", 1.f / static_cast<float>(main_window->delta_time()), static_cast<float>(main_window->delta_time()));
        ImGui::PopStyleColor();

        ImGui::Spacing();

        ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
        if (bool b = ImGui::CollapsingHeader("Application"); ImGui::PopFont(), b)
        {
            ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
            ImGui::Text("Screen");
            ImGui::PopStyleColor();
            ImGui::PopFont();
            ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
            ImGui::TextWrapped("Settings concerning the screen resolution and framebuffer attachments.");
            ImGui::PopStyleColor();

            static glm::ivec2 res(full_resolution);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth()*0.6f);
            ImGui::DragInt2("##scale_window", &res[0], 1, 0, 8192);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            static bool fscr = is_fullscreen;
            ImGui::Checkbox("Fullscreen", &fscr);

            static int curr_msaa = 2;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::Combo("MSAA", &curr_msaa, "  1x MSAA (OFF) \0 2x MSAA \0 4x MSAA \0 8x MSAA \0 16x MSAA \0"))
            {
            }
            ImGui::PopItemWidth();

            if (fscr != is_fullscreen || res != full_resolution || 1<< curr_msaa != static_cast<uint32_t>(sample_count))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
                ImGui::TextWrapped("Click \"Apply\" for your changes to take effect.");
                ImGui::PopStyleColor();

                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                {
                    full_resolution = res;
                    is_fullscreen = fscr;
                    sample_count = vk::SampleCountFlagBits(1 << curr_msaa);
                    createMultisampleRenderpass();
                    rebuildSwapchain();
                    pipeline->renderpass = msaa_renderpass.get();
                    pipeline->multisample.setRasterizationSamples(sample_count);
                    pipeline->finalize();
                }
            }

            static bool en_lim = true;
            ImGui::Checkbox("Limit FPS", &en_lim);
            if (en_lim)
            {
                ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::DragFloat("##LimitFPS", &max_framerate, 0.1f, 20.f, 9999999.f, "%.1f fps");
                ImGui::PopItemWidth();
                main_window->limit_framerate(max_framerate);
            }
            else
                main_window->limit_framerate(std::numeric_limits<float>::max());

            ImGui::Spacing();
            ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff87f100);
            ImGui::Text("General");
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, 0x88ffffff);
            ImGui::TextWrapped("General application controls.");
            ImGui::PopStyleColor();
            ImGui::PopFont();
            if (ImGui::Button("Quit Application", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                main_window->close();
        }

        ImGui::PushFont(ImGui::GetIO().Fonts[0].Fonts[2]);
        if (bool b = ImGui::CollapsingHeader("Shaders"); ImGui::PopFont(), b)
        {
            if (ImGui::Button("Reload All", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                pipeline->reload_stages();
        }

        ImGui::End();

        ctrl.update(cam, *main_window, main_window->delta_time());

        // Render to secondary command buffer
        vk::CommandBufferInheritanceInfo inheritance(msaa_renderpass.get());
        render_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue |
            vk::CommandBufferUsageFlagBits::eSimultaneousUse, &inheritance));

        // Setup render environment
        render_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
        render_command_buffer.setViewport(0, vk::Viewport(0, 0, main_window->swapchain()->extent().width, main_window->swapchain()->extent().height, 0, 1.f));
        render_command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(), main_window->swapchain()->extent()));

        // Push information
        render_command_buffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, tex_set);
        render_command_buffer.pushConstants(*pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, vk::ArrayProxy<const glm::mat4>{
            (cam.projection(main_window->swapchain()->extent().width, main_window->swapchain()->extent().height)) * cam.view(),
                inverse(cam.view())
        });

        // Draw mesh
        render_command_buffer.bindVertexBuffers(0, static_cast<vk::Buffer>(*vtxbuffer), 0ui64);
        render_command_buffer.bindIndexBuffer(*idxbuffer, 0, vk::IndexType::eUint32);
        render_command_buffer.drawIndexed(idx.size(), 1, 0, 0, 0);
        render_command_buffer.end();

        main_window->current_primary_command_buffer().beginRenderPass(vk::RenderPassBeginInfo(msaa_renderpass.get(),
            *framebuffers[main_window->swapchain()->current_image()],
            vk::Rect2D({ 0, 0 }, main_window->swapchain()->extent()),
            static_cast<uint32_t>(std::size(clear_values)), std::data(clear_values)),
            vk::SubpassContents::eSecondaryCommandBuffers);
        main_window->current_primary_command_buffer().executeCommands({ render_command_buffer });
        main_window->current_primary_command_buffer().endRenderPass();
    }

    return 0;
}

void createMultisampleRenderpass()
{
    if (msaa_renderpass)
        msaa_renderpass.reset();

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
        .setLoadOp(vk::AttachmentLoadOp::eClear)
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

    vk::AttachmentReference color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference resolve_reference(1, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depth_reference(2, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setPColorAttachments(sample_count == vk::SampleCountFlagBits::e1 ? &resolve_reference : &color_reference)
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
    msaa_renderpass = main_window->device()->createRenderPassUnique(msaa_render_pass_info);
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
    depth_attachment = jpu::make_ref<vkn::texture>(main_window->device());
    depth_attachment->create_empty(image_info);

    image_info.setFormat(vk::Format::eB8G8R8A8Unorm)
        .setUsage(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
    color_attachment = jpu::make_ref<vkn::texture>(main_window->device());
    color_attachment->create_empty(image_info);

    const auto msaa_color_view = jpu::make_ref<vkn::texture_view>(color_attachment);
    const auto msaa_depth_view = jpu::make_ref<vkn::texture_view>(depth_attachment,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1));

    framebuffers.resize(main_window->swapchain()->images().size(), nullptr);
    for (auto i = 0ui64; i < main_window->swapchain()->images().size(); ++i)
    {
        framebuffers[i] = jpu::make_ref<vkn::framebuffer>(main_window->device(), msaa_renderpass.get(), main_window->swapchain()->extent());
        framebuffers[i]->set_attachments({ msaa_color_view, main_window->swapchain()->image_views()[i], msaa_depth_view });
        framebuffers[i]->finalize();
    }
}
