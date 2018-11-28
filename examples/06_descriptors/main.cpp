#include "camera.hpp"
#include "cie.hpp"
#include "descriptors.hpp"
#include "input.hpp"
#include "input_qt.hpp"
#include "shaders/shaders.hpp"
#include <gfx.core/version.hpp>
#include <gfx.core/worker.hpp>
#include <gfx.graphics/graphics.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QRadioButton>
#include <QPushButton>
#include <QSlider>
#include <QMouseEvent>
#include <QCheckBox>
#include <QFrame>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleFactory>
#include <QWidget>
#include <QWindow>
#include <future>
#include <gfx.core/log.hpp>
#include <gfx.shaders/shaders.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <random>
#include <QTimer>

#include <gfx.ecs.defaults2/prototype.hpp>
#include <gfx.file/file.hpp>

enum class bsdf : uint
{
    opaque,
    transparent,
    emissive
};
struct material_info
{
    glm::u8vec4 color;
    float       roughness;
    float       strength;
    bsdf        type;
};

using namespace std::chrono_literals;

gfx::image load_cubemap(gfx::device& gpu, const std::filesystem::path& root);

int main(int argc, char** argv)
{
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;

    using gfx::u32;
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("fusion"));
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize() + 2);
    QApplication::setFont(defaultFont);
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    app.setPalette(darkPalette);
    app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    QMainWindow win;
    win.resize(1280, 720);

    QSplitter* mainLayout = new QSplitter;
    win.setCentralWidget(mainLayout);
    mainLayout->setContentsMargins(QMargins(8, 8, 8, 8));
    QFrame* render_frame = new QFrame;
    render_frame->setFrameStyle(QFrame::Shadow_Mask);
    render_frame->setFrameShadow(QFrame::Sunken);
    QVBoxLayout* render_frame_layout = new QVBoxLayout;
    render_frame->setLayout(render_frame_layout);
    render_frame->setStyleSheet("background-color: black");
    render_frame_layout->setContentsMargins(QMargins(1, 1, 1, 1));
    QWidget* render_surface = new QWidget;
    render_frame_layout->addWidget(render_surface);
    render_surface->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    QVBoxLayout* right_panel_layout = new QVBoxLayout;
    QWidget*     right_panel        = new QWidget;
    mainLayout->addWidget(right_panel);
    right_panel->setLayout(right_panel_layout);

    QTabWidget*  tabWidget      = new QTabWidget;
    QWidget*     general        = new QWidget;
    QVBoxLayout* general_layout = new QVBoxLayout;
    general->setLayout(general_layout);
    QGroupBox*   gfx_info        = new QGroupBox("Graphics");
    QFormLayout* gfx_info_layout = new QFormLayout;
    gfx_info->setLayout(gfx_info_layout);
    QGroupBox*   perf_info        = new QGroupBox("Performance");
    QFormLayout* perf_info_layout = new QFormLayout;
    perf_info->setLayout(perf_info_layout);
    general_layout->addWidget(gfx_info);
    general_layout->addWidget(perf_info);

    QWidget*     visuals        = new QWidget;
    QVBoxLayout* visuals_layout = new QVBoxLayout;
    visuals->setLayout(visuals_layout);

    tabWidget->addTab(general, "Info");
    tabWidget->addTab(visuals, "Visuals");
    right_panel_layout->addWidget(tabWidget);
    right_panel_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    win.setStatusBar(new QStatusBar);
    mainLayout->addWidget(render_frame);
    mainLayout->setSizes(QList<int>({250, 1280}));
    win.show();

    gfx::instance              my_app("Application", gfx::version_t(1, 0, 0), false, true);
    gfx::surface               surf1(my_app, render_surface->winId());
    gfx::device                gpu(my_app, gfx::device_target::gpu, {1.f, 0.5f}, 1.f, surf1);
    gfx::swapchain             chain(gpu, surf1);
    std::vector<gfx::commands> gpu_cmd = gpu.allocate_graphics_commands(chain.count());
    gfx::semaphore             acquire_image_signal(gpu);
    gfx::semaphore             render_finish_signal(gpu);
    std::vector<gfx::fence>    cmd_fences(gpu_cmd.size(), gfx::fence(gpu, true));

    const auto  props       = gpu.get_physical_device().getProperties2();
    const char* device_type = [&] {
        using dt = vk::PhysicalDeviceType;
        switch (props.properties.deviceType)
        {
        case dt::eCpu: return "CPU";
        case dt::eDiscreteGpu: return "Discrete GPU";
        case dt::eIntegratedGpu: return "iGPU";
        case dt::eOther: return "Unknown";
        case dt::eVirtualGpu: return "Virtual GPU";
        }
        return "";
    }();
    gfx_info_layout->addRow("Renderer", new QLabel("Vulkan"));
    gfx_info_layout->addRow("Version", new QLabel(QString::fromStdString(to_string(gfx::to_version(props.properties.apiVersion)))));
    gfx_info_layout->addRow("Device", new QLabel(props.properties.deviceName));
    gfx_info_layout->addRow("Type", new QLabel(device_type));
    gfx_info_layout->addRow("Driver", new QLabel(QString::fromStdString(to_string(gfx::to_version(props.properties.driverVersion)))));

    QLabel* fps_counter = new QLabel("waiting...");
    QLabel* ftm_counter = new QLabel("waiting...");
    perf_info_layout->addRow("Framerate", fps_counter);
    perf_info_layout->addRow("Frametime", ftm_counter);

    QGroupBox*    groupBox       = new QGroupBox("Output");
    QVBoxLayout*  vbox           = new QVBoxLayout;
    QButtonGroup* render_outputs = new QButtonGroup;
    QRadioButton* radio_default  = new QRadioButton("Default");
    QRadioButton* radio_samples  = new QRadioButton("Samples");
    render_outputs->addButton(radio_default, 0);
    render_outputs->addButton(radio_samples, 1);
    radio_default->setChecked(true);

    vbox->addStretch(1);
    vbox->addWidget(radio_default);
    vbox->addWidget(radio_samples);
    groupBox->setLayout(vbox);
    visuals_layout->addWidget(groupBox);

    visuals_layout->addWidget(new QWidget, 1);
    general_layout->addWidget(new QWidget, 1);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Output Images
    ////
    ////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<gfx::image> color_accum;
    std::unique_ptr<gfx::image> bounce_accum;
    std::unique_ptr<gfx::image> positions_bounce;
    std::unique_ptr<gfx::image> directions_sample;
    std::unique_ptr<gfx::image> randoms;
    std::unique_ptr<gfx::image> results;
    vk::UniqueImageView         color_accum_view;
    vk::UniqueImageView         bounce_accum_view;
    vk::UniqueImageView         positions_bounce_view;
    vk::UniqueImageView         directions_sample_view;
    vk::UniqueImageView         randoms_view;
    vk::UniqueImageView         results_view;

    const auto create_target_images = [&] {
        vk::ImageCreateInfo color_accum_create;
        color_accum_create.arrayLayers   = 1;
        color_accum_create.extent        = vk::Extent3D(1920, 1080, 1);
        color_accum_create.format        = vk::Format::eR32G32B32A32Sfloat;
        color_accum_create.imageType     = vk::ImageType::e2D;
        color_accum_create.initialLayout = vk::ImageLayout::eUndefined;
        color_accum_create.mipLevels     = 1;
        color_accum_create.samples       = vk::SampleCountFlagBits::e1;
        color_accum_create.sharingMode   = vk::SharingMode::eExclusive;
        color_accum_create.tiling        = vk::ImageTiling::eOptimal;
        color_accum_create.usage =
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
        color_accum              = std::make_unique<gfx::image>(gpu, color_accum_create);
        bounce_accum             = std::make_unique<gfx::image>(gpu, color_accum_create);
        positions_bounce         = std::make_unique<gfx::image>(gpu, color_accum_create);
        directions_sample        = std::make_unique<gfx::image>(gpu, color_accum_create);
        randoms                  = std::make_unique<gfx::image>(gpu, color_accum_create);
        color_accum_create.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment
                                   | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
        results = std::make_unique<gfx::image>(gpu, color_accum_create);

        vk::ImageViewCreateInfo color_accum_view_create;
        color_accum_view_create.format           = color_accum_create.format;
        color_accum_view_create.image            = color_accum->get_image();
        color_accum_view_create.viewType         = vk::ImageViewType::e2D;
        color_accum_view_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        color_accum_view                         = gpu.get_device().createImageViewUnique(color_accum_view_create);
        color_accum_view_create.image            = bounce_accum->get_image();
        bounce_accum_view                        = gpu.get_device().createImageViewUnique(color_accum_view_create);
        color_accum_view_create.image            = positions_bounce->get_image();
        positions_bounce_view                    = gpu.get_device().createImageViewUnique(color_accum_view_create);
        color_accum_view_create.image            = directions_sample->get_image();
        directions_sample_view                   = gpu.get_device().createImageViewUnique(color_accum_view_create);
        color_accum_view_create.image            = randoms->get_image();
        randoms_view                             = gpu.get_device().createImageViewUnique(color_accum_view_create);
        color_accum_view_create.image            = results->get_image();
        results_view                             = gpu.get_device().createImageViewUnique(color_accum_view_create);

        gfx::commands switch_layout = gpu.allocate_transfer_command();
        switch_layout.cmd().begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        {
            vk::ImageMemoryBarrier attachment_barrier;
            attachment_barrier.oldLayout                     = vk::ImageLayout::eUndefined;
            attachment_barrier.srcAccessMask                 = {};
            attachment_barrier.srcQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
            attachment_barrier.newLayout                     = vk::ImageLayout::eGeneral;
            attachment_barrier.dstAccessMask                 = {};
            attachment_barrier.dstQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
            attachment_barrier.image                         = color_accum->get_image();
            attachment_barrier.subresourceRange              = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            vk::ImageMemoryBarrier attachment_barrier_bounce = attachment_barrier;
            attachment_barrier_bounce.image                  = bounce_accum->get_image();
            vk::ImageMemoryBarrier positions_bounce_barrier  = attachment_barrier;
            positions_bounce_barrier.image                   = positions_bounce->get_image();
            vk::ImageMemoryBarrier directions_sample_barrier = attachment_barrier;
            directions_sample_barrier.image                  = directions_sample->get_image();
            vk::ImageMemoryBarrier randoms_barrier           = attachment_barrier;
            randoms_barrier.image                            = randoms->get_image();
            vk::ImageMemoryBarrier results_barrier           = attachment_barrier;
            results_barrier.image                            = results->get_image();
            switch_layout.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe,
                                                vk::DependencyFlagBits::eByRegion, {}, {},
                                                {attachment_barrier, attachment_barrier_bounce, positions_bounce_barrier,
                                                 directions_sample_barrier, randoms_barrier, results_barrier});
        }
        switch_layout.cmd().end();
        gpu.transfer_queue().submit({switch_layout}, {}, {});
        gpu.transfer_queue().wait();
    };

    const auto save_image = [&] {
        gfx::mapped<glm::vec4> pixels(gpu, chain.extent().width * chain.extent().height);
        gfx::commands          switch_layout = gpu.allocate_transfer_command();
        switch_layout.cmd().begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        {
            vk::ImageMemoryBarrier attachment_barrier;
            attachment_barrier.oldLayout           = vk::ImageLayout::eUndefined;
            attachment_barrier.srcAccessMask       = {};
            attachment_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            attachment_barrier.newLayout           = vk::ImageLayout::eTransferSrcOptimal;
            attachment_barrier.dstAccessMask       = vk::AccessFlagBits::eMemoryRead;
            attachment_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            attachment_barrier.image               = results->get_image();
            attachment_barrier.subresourceRange    = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            switch_layout.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                                vk::DependencyFlagBits::eByRegion, {}, {}, attachment_barrier);

            const vk::BufferImageCopy region(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), {0, 0, 0},
                                             {chain.extent().width, chain.extent().height, 0});
            switch_layout.cmd().copyImageToBuffer(results->get_image(), vk::ImageLayout::eTransferSrcOptimal, pixels.get_buffer(), region);

            attachment_barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
            attachment_barrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
            attachment_barrier.newLayout     = vk::ImageLayout::eGeneral;
            attachment_barrier.dstAccessMask = {};
            switch_layout.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands,
                                                vk::DependencyFlagBits::eByRegion, {}, {}, attachment_barrier);
        }
        switch_layout.cmd().end();
        gpu.transfer_queue().submit({switch_layout}, {}, {});
        gpu.transfer_queue().wait();

        if (const auto out = gfx::file::save_dialog("Save Image", "./"); out)
        {
            std::vector<glm::u8vec3> conv(pixels.size());
            int                      i = 0;
            for (auto& px : pixels) conv[i++] = clamp(px * 255.f, 0.f, 255.f);
            gfx::image_file::save_png(*out, chain.extent().width, chain.extent().height, 3, &conv.data()[0][0]);
        }
    };

    QMenuBar* menuBar  = new QMenuBar();
    QMenu*    fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);
    fileMenu->addAction("Open", [] { gfx::ilog << "Open..."; }, QKeySequence::Open);
    fileMenu->addAction("Save", save_image, QKeySequence::Save);
    fileMenu->addAction("Save As", [] { gfx::ilog << "Save as..."; }, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Close", [&win] { win.close(); }, QKeySequence::Close);
    fileMenu->addAction("Quit", [] { QCoreApplication::quit(); }, QKeySequence::Quit);
    win.setMenuBar(menuBar);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Framebuffer/Renderpass
    ////
    ////////////////////////////////////////////////////////////////////////////
    struct att_desc
    {
        enum
        {
            color,
            accum,
            bounce,
            pos_bounce,
            dir_sample,
            randoms,
            results,
            _count
        };
    };
    vk::AttachmentDescription attachment_descriptions[att_desc::_count];
    attachment_descriptions[att_desc::color].initialLayout  = vk::ImageLayout::eUndefined;
    attachment_descriptions[att_desc::color].finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    attachment_descriptions[att_desc::color].format         = chain.format();
    attachment_descriptions[att_desc::color].loadOp         = vk::AttachmentLoadOp::eClear;
    attachment_descriptions[att_desc::color].storeOp        = vk::AttachmentStoreOp::eStore;
    attachment_descriptions[att_desc::color].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachment_descriptions[att_desc::color].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachment_descriptions[att_desc::color].samples        = vk::SampleCountFlagBits::e1;

    attachment_descriptions[att_desc::accum].initialLayout  = vk::ImageLayout::eGeneral;
    attachment_descriptions[att_desc::accum].finalLayout    = vk::ImageLayout::eGeneral;
    attachment_descriptions[att_desc::accum].format         = vk::Format::eR32G32B32A32Sfloat;
    attachment_descriptions[att_desc::accum].loadOp         = vk::AttachmentLoadOp::eLoad;
    attachment_descriptions[att_desc::accum].storeOp        = vk::AttachmentStoreOp::eStore;
    attachment_descriptions[att_desc::accum].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachment_descriptions[att_desc::accum].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachment_descriptions[att_desc::accum].samples        = vk::SampleCountFlagBits::e1;

    attachment_descriptions[att_desc::bounce]     = attachment_descriptions[att_desc::accum];
    attachment_descriptions[att_desc::pos_bounce] = attachment_descriptions[att_desc::accum];
    attachment_descriptions[att_desc::dir_sample] = attachment_descriptions[att_desc::accum];
    attachment_descriptions[att_desc::randoms]    = attachment_descriptions[att_desc::accum];
    attachment_descriptions[att_desc::results]    = attachment_descriptions[att_desc::accum];

    const vk::AttachmentReference color_attachments[] = {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal),
                                                         vk::AttachmentReference(1, vk::ImageLayout::eGeneral),
                                                         vk::AttachmentReference(2, vk::ImageLayout::eGeneral),
                                                         vk::AttachmentReference(3, vk::ImageLayout::eGeneral),
                                                         vk::AttachmentReference(4, vk::ImageLayout::eGeneral),
                                                         vk::AttachmentReference(5, vk::ImageLayout::eGeneral),
                                                         vk::AttachmentReference(6, vk::ImageLayout::eGeneral)};

    vk::SubpassDescription main_subpass;
    main_subpass.colorAttachmentCount = u32(std::size(color_attachments));
    main_subpass.pColorAttachments    = std::data(color_attachments);
    main_subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;

    vk::SubpassDependency main_subpass_dep;
    main_subpass_dep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    main_subpass_dep.srcAccessMask   = {};
    main_subpass_dep.srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe;
    main_subpass_dep.dstSubpass      = 0;
    main_subpass_dep.dstAccessMask   = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    main_subpass_dep.dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    main_subpass_dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    vk::RenderPassCreateInfo rp_info;
    rp_info.attachmentCount = u32(std::size(attachment_descriptions));
    rp_info.pAttachments    = std::data(attachment_descriptions);
    rp_info.subpassCount    = 1;
    rp_info.pSubpasses      = &main_subpass;
    rp_info.dependencyCount = 1;
    rp_info.pDependencies   = &main_subpass_dep;

    const auto pass         = gpu.get_device().createRenderPassUnique(rp_info);

    std::vector<vk::UniqueImageView>   imvs;
    std::vector<vk::UniqueFramebuffer> fbos;

    create_target_images();
    const auto build_fbos = [&] {
        fbos.clear();
        imvs.clear();
        for (size_t i = 0; i < chain.count(); ++i)
        {
            vk::ImageViewCreateInfo imv_create;
            imv_create.format           = chain.format();
            imv_create.image            = chain.images()[i].get_image();
            imv_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            imv_create.viewType         = vk::ImageViewType::e2D;
            imvs.emplace_back(gpu.get_device().createImageViewUnique(imv_create));

            const auto attachments = {imvs[i].get(),
                                      color_accum_view.get(),
                                      bounce_accum_view.get(),
                                      positions_bounce_view.get(),
                                      directions_sample_view.get(),
                                      randoms_view.get(),
                                      results_view.get()};

            vk::FramebufferCreateInfo fbo_create;
            fbo_create.attachmentCount = u32(std::size(attachments));
            fbo_create.renderPass      = pass.get();
            fbo_create.width           = chain.extent().width;
            fbo_create.height          = chain.extent().height;
            fbo_create.layers          = 1;
            fbo_create.pAttachments    = std::data(attachments);
            fbos.emplace_back(gpu.get_device().createFramebufferUnique(fbo_create));
        }
    };
    build_fbos();
    vk::GraphicsPipelineCreateInfo pipe_info;
    pipe_info.subpass    = 0;
    pipe_info.renderPass = pass.get();

    const gfx::shader vert(gpu, gfx::spirv::core::screen_vert);
    const gfx::shader frag(gpu, gfx::spirv::spectral::shaders::spectral_frag);
    const auto        stages = {
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vert.get_module(), "main"),
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, frag.get_module(), "main"),
    };
    pipe_info.stageCount = gfx::u32(std::size(stages));
    pipe_info.pStages    = std::data(stages);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Descriptor Pool
    ////
    ////////////////////////////////////////////////////////////////////////////
    using dct                                  = vk::DescriptorType;
    vk::DescriptorPoolSize       dpool_sizes[] = {{dct::eUniformBuffer, 1}, {dct::eStorageBuffer, 8}};
    vk::DescriptorPoolCreateInfo dpool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 3, u32(std::size(dpool_sizes)),
                                            std::data(dpool_sizes));
    vk::UniqueDescriptorPool     dpool = gpu.get_device().createDescriptorPoolUnique(dpool_info);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		ECS
    ////
    ////////////////////////////////////////////////////////////////////////////
    gfx::ecs::ecs ecs;

    // Controls
    gfx::ecs::system_list   control_systems;
    gfx::qt_input_system    keys(render_surface);
    gfx::user_camera_system camera_system(keys);
    control_systems.add(keys);
    control_systems.add(camera_system);

    // Rendering
    gfx::ecs::system_list               render_systems;
    gfx::instance_system<material_info> instances(gpu, gfx::mesh_allocator_flag::use_bvh);
    render_systems.add(instances);

    // Create user entity
    gfx::ecs::unique_entity user_entity                      = ecs.create_entity_unique(gfx::projection_component(), gfx::camera_controls(),
                                                                   gfx::transform_component(), gfx::grabbed_cursor_component());
    user_entity->get<gfx::transform_component>()->position.z = -4.f;

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Meshes
    ////
    ////////////////////////////////////////////////////////////////////////////
    gfx::unique_prototype bunny  = instances.get_instantiator().allocate_prototype_unique("Bunny", gfx::scene_file("bunny.dae").mesh);
    gfx::unique_prototype sphere = instances.get_instantiator().allocate_prototype_unique("Sphere", gfx::scene_file("sphere.dae").mesh);
    gfx::unique_prototype floor  = instances.get_instantiator().allocate_prototype_unique("Floor", gfx::scene_file("floor.dae").mesh);
    
    ecs.create_entity(gfx::instance_component{floor.get(), material_info{glm::u8vec4(255, 255, 255, 255), 0.2f, 0.f, bsdf::opaque}},
                          gfx::transform_component{{0, -1.f, 0}, {2.f, 2.f, 2.f}, glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0))});
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            ecs.create_entity(
                gfx::instance_component{bunny.get(), material_info{glm::u8vec4(0, 50, 255, 255), 0.25f * i, 0.25f * j, bsdf::opaque}},
                gfx::transform_component{
                    {-5.f + 2.5f * i, 0, -5.f + 2.5f * j}, {1, 1, 1}, glm::angleAxis(glm::radians(0.f), glm::vec3(1, 0, 0))});
        }
        ecs.create_entity(
            gfx::instance_component{bunny.get(), material_info{glm::u8vec4(0, 50, 255, 255), 0.25f * i, 0.f, bsdf::transparent}},
            gfx::transform_component{{-5.f + 2.5f * i, 0, -7.5f}, {1, 1, 1}, glm::angleAxis(glm::radians(0.f), glm::vec3(1, 0, 0))});
    }
    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Environment
    ////
    ////////////////////////////////////////////////////////////////////////////
    gfx::image              cubemap = load_cubemap(gpu, "moulton_station_train_tunnel_west_16k.hdr/hdr");
    vk::ImageViewCreateInfo cubemap_view_create;
    cubemap_view_create.image            = cubemap.get_image();
    cubemap_view_create.format           = vk::Format::eR32G32B32A32Sfloat;
    cubemap_view_create.viewType         = vk::ImageViewType::eCube;
    cubemap_view_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 6);
    vk::UniqueImageView   cubemap_view   = gpu.get_device().createImageViewUnique(cubemap_view_create);
    vk::SamplerCreateInfo cubemap_sampler_create;
    cubemap_sampler_create.addressModeU     = vk::SamplerAddressMode::eClampToEdge;
    cubemap_sampler_create.addressModeV     = vk::SamplerAddressMode::eClampToEdge;
    cubemap_sampler_create.addressModeW     = vk::SamplerAddressMode::eClampToEdge;
    cubemap_sampler_create.anisotropyEnable = true;
    cubemap_sampler_create.maxAnisotropy    = 16.f;
    cubemap_sampler_create.compareEnable    = false;
    cubemap_sampler_create.magFilter        = vk::Filter::eLinear;
    cubemap_sampler_create.minFilter        = vk::Filter::eLinear;
    cubemap_sampler_create.mipmapMode       = vk::SamplerMipmapMode::eLinear;
    vk::UniqueSampler cubemap_sampler       = gpu.get_device().createSamplerUnique(cubemap_sampler_create);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		CIE
    ////
    ////////////////////////////////////////////////////////////////////////////
    vk::ImageCreateInfo cie_spectrum_create;
    cie_spectrum_create.arrayLayers   = 1;
    cie_spectrum_create.extent        = vk::Extent3D(u32(std::size(gfx::cie_curves)), 1, 1);
    cie_spectrum_create.format        = vk::Format::eR32G32B32A32Sfloat;
    cie_spectrum_create.imageType     = vk::ImageType::e1D;
    cie_spectrum_create.initialLayout = vk::ImageLayout::eUndefined;
    cie_spectrum_create.mipLevels     = 1;
    cie_spectrum_create.samples       = vk::SampleCountFlagBits::e1;
    cie_spectrum_create.sharingMode   = vk::SharingMode::eExclusive;
    cie_spectrum_create.tiling        = vk::ImageTiling::eOptimal;
    cie_spectrum_create.usage         = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    gfx::image cie_spectrum(gpu, cie_spectrum_create);

    gfx::image_file     bokeh_image("bokeh.jpeg", gfx::bits::b8, 1);
    vk::ImageCreateInfo bokeh_create;
    bokeh_create.arrayLayers   = 1;
    bokeh_create.extent        = vk::Extent3D(bokeh_image.width, bokeh_image.height, 1);
    bokeh_create.format        = vk::Format::eR8Unorm;
    bokeh_create.imageType     = vk::ImageType::e2D;
    bokeh_create.initialLayout = vk::ImageLayout::eUndefined;
    bokeh_create.mipLevels     = 1;
    bokeh_create.samples       = vk::SampleCountFlagBits::e1;
    bokeh_create.sharingMode   = vk::SharingMode::eExclusive;
    bokeh_create.tiling        = vk::ImageTiling::eOptimal;
    bokeh_create.usage         = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    gfx::image bokeh(gpu, bokeh_create);

    gfx::mapped<glm::vec4> cie_values(gpu, gfx::cie_curves);
    gfx::commands          transfer_cie = gpu.allocate_transfer_command();
     
    gfx::mapped<uint8_t> bokeh_values(gpu,
                                      gsl::make_span(static_cast<uint8_t*>(bokeh_image.bytes()), bokeh_image.width * bokeh_image.height));


    transfer_cie.cmd().begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    {
        vk::ImageMemoryBarrier cie_barrier;
        cie_barrier.oldLayout           = vk::ImageLayout::eUndefined;
        cie_barrier.srcAccessMask       = {};
        cie_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        cie_barrier.newLayout           = vk::ImageLayout::eTransferDstOptimal;
        cie_barrier.dstAccessMask       = vk::AccessFlagBits::eMemoryWrite;
        cie_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        cie_barrier.image               = cie_spectrum.get_image();
        cie_barrier.subresourceRange    = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        transfer_cie.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                           vk::DependencyFlagBits::eByRegion, {}, {}, cie_barrier);
        cie_barrier.image = bokeh.get_image();
        transfer_cie.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                           vk::DependencyFlagBits::eByRegion, {}, {}, cie_barrier);

        vk::BufferImageCopy cie_copy;
        cie_copy.imageExtent      = cie_spectrum_create.extent;
        cie_copy.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        transfer_cie.cmd().copyBufferToImage(cie_values.get_buffer(), cie_spectrum.get_image(), vk::ImageLayout::eTransferDstOptimal,
                                             cie_copy);
        vk::BufferImageCopy bokeh_copy;
        bokeh_copy.imageExtent      = bokeh_create.extent;
        bokeh_copy.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        transfer_cie.cmd().copyBufferToImage(bokeh_values.get_buffer(), bokeh.get_image(), vk::ImageLayout::eTransferDstOptimal,
                                             bokeh_copy);

        cie_barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
        cie_barrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
        cie_barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        cie_barrier.dstAccessMask = {};
        cie_barrier.image         = cie_spectrum.get_image();
        transfer_cie.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
                                           vk::DependencyFlagBits::eByRegion, {}, {}, cie_barrier);
        cie_barrier.image = bokeh.get_image();
        transfer_cie.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
                                           vk::DependencyFlagBits::eByRegion, {}, {}, cie_barrier);
    }
    transfer_cie.cmd().end();
    gpu.transfer_queue().submit({transfer_cie}, {}, {});

    vk::ImageViewCreateInfo cie_spectrum_view_create;
    cie_spectrum_view_create.format           = cie_spectrum_create.format;
    cie_spectrum_view_create.image            = cie_spectrum.get_image();
    cie_spectrum_view_create.viewType         = vk::ImageViewType::e1D;
    cie_spectrum_view_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    vk::UniqueImageView cie_spectrum_view     = gpu.get_device().createImageViewUnique(cie_spectrum_view_create);

    vk::ImageViewCreateInfo bokeh_view_create;
    bokeh_view_create.format           = bokeh_create.format;
    bokeh_view_create.image            = bokeh.get_image();
    bokeh_view_create.viewType         = vk::ImageViewType::e2D;
    bokeh_view_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    vk::UniqueImageView bokeh_view     = gpu.get_device().createImageViewUnique(bokeh_view_create);

    vk::SamplerCreateInfo cie_sampler_create;
    cie_sampler_create.addressModeU     = vk::SamplerAddressMode::eClampToEdge;
    cie_sampler_create.addressModeV     = vk::SamplerAddressMode::eClampToEdge;
    cie_sampler_create.addressModeW     = vk::SamplerAddressMode::eClampToEdge;
    cie_sampler_create.anisotropyEnable = false;
    cie_sampler_create.compareEnable    = false;
    cie_sampler_create.magFilter        = vk::Filter::eLinear;
    cie_sampler_create.minFilter        = vk::Filter::eLinear;
    cie_sampler_create.mipmapMode       = vk::SamplerMipmapMode::eNearest;
    vk::UniqueSampler cie_sampler       = gpu.get_device().createSamplerUnique(cie_sampler_create);

    ////////////////////////////////////////////////////////////////////////////
    ////
    ////		Descriptor Set Layouts
    ////
    ////////////////////////////////////////////////////////////////////////////
    vk::DescriptorSetLayoutBinding mat_bindings[] = {
        {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment},           // Camera Data
        {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Bokeh Shape
    };
    vk::DescriptorSetLayoutCreateInfo dset_create({}, u32(std::size(mat_bindings)), std::data(mat_bindings));
    vk::UniqueDescriptorSetLayout     mat_set_layout = gpu.get_device().createDescriptorSetLayoutUnique(dset_create);

    vk::DescriptorSetLayoutBinding mesh_bindings[] = {
        {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment},    // BVH Nodes
        {1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment},    // Vertices
        {2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment},    // Indices
        {3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment},    // Instances
    };
    vk::DescriptorSetLayoutCreateInfo mesh_set_create({}, u32(std::size(mesh_bindings)), std::data(mesh_bindings));
    vk::UniqueDescriptorSetLayout     mesh_set_layout = gpu.get_device().createDescriptorSetLayoutUnique(mesh_set_create);

    vk::DescriptorSetLayoutBinding globals_bindings[] = {
        {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment},           // Globals
        {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // CIE values
        {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Cubemap
        {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Accum
        {4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Bounce
        {5, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Pos + bounce
        {6, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Dir + sample
        {7, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},    // Randoms
    };
    vk::DescriptorSetLayoutCreateInfo globals_set_create({}, u32(std::size(globals_bindings)), std::data(globals_bindings));
    vk::UniqueDescriptorSetLayout     globals_set_layout = gpu.get_device().createDescriptorSetLayoutUnique(globals_set_create);

    const auto                   used_layouts = {mat_set_layout.get(), mesh_set_layout.get(), globals_set_layout.get()};
    vk::PipelineLayoutCreateInfo pipe_layout_info({}, u32(std::size(used_layouts)), std::data(used_layouts));

    vk::UniquePipelineLayout pipe_layout = gpu.get_device().createPipelineLayoutUnique(pipe_layout_info);
    pipe_info.layout                     = pipe_layout.get();

    vk::PipelineViewportStateCreateInfo vp_state({}, 1, nullptr, 1, nullptr);
    pipe_info.pViewportState = &vp_state;
    const auto dyn_states    = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dyn_state({}, gfx::u32(std::size(dyn_states)), std::data(dyn_states));
    pipe_info.pDynamicState = &dyn_state;

    vk::PipelineVertexInputStateCreateInfo vert_info;
    pipe_info.pVertexInputState = &vert_info;

    vk::PipelineRasterizationStateCreateInfo rst_state({}, 0, 0, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
                                                       vk::FrontFace::eCounterClockwise, false, 0.f, 0.f, 0.f, 1.f);
    pipe_info.pRasterizationState = &rst_state;

    vk::PipelineInputAssemblyStateCreateInfo inp_state({}, vk::PrimitiveTopology::eTriangleList, false);
    pipe_info.pInputAssemblyState = &inp_state;

    vk::PipelineDepthStencilStateCreateInfo dep_state;
    pipe_info.pDepthStencilState = &dep_state;

    vk::PipelineMultisampleStateCreateInfo msaa_state;
    pipe_info.pMultisampleState = &msaa_state;

    vk::PipelineColorBlendAttachmentState blend_col_atts[7];
    blend_col_atts[0].blendEnable = false;
    blend_col_atts[0].colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    blend_col_atts[1] = blend_col_atts[0];
    blend_col_atts[2] = blend_col_atts[0];
    blend_col_atts[3] = blend_col_atts[0];
    blend_col_atts[4] = blend_col_atts[0];
    blend_col_atts[5] = blend_col_atts[0];
    blend_col_atts[6] = blend_col_atts[0];
    vk::PipelineColorBlendStateCreateInfo bln_state({}, false, {}, u32(std::size(blend_col_atts)), std::data(blend_col_atts));
    pipe_info.pColorBlendState = &bln_state;

    vk::UniquePipeline                pipe = gpu.get_device().createGraphicsPipelineUnique(nullptr, pipe_info);
    gfx::buffer<gfx::camera_matrices> camera_info_buffer(gpu, {*gfx::get_camera_info(*user_entity)});

    vk::UniqueDescriptorSet mat_set = std::move(gpu.get_device().allocateDescriptorSetsUnique({dpool.get(), 1, &mat_set_layout.get()})[0]);
    vk::UniqueDescriptorSet mesh_set =
        std::move(gpu.get_device().allocateDescriptorSetsUnique({dpool.get(), 1, &mesh_set_layout.get()})[0]);
    vk::UniqueDescriptorSet globals_set =
        std::move(gpu.get_device().allocateDescriptorSetsUnique({dpool.get(), 1, &globals_set_layout.get()})[0]);

    gpu.get_device().updateDescriptorSets(
        {{mat_set.get(), 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &gfx::info_for(camera_info_buffer)},
         {mat_set.get(), 1, 0, 1, vk::DescriptorType::eCombinedImageSampler,
          &vk::DescriptorImageInfo(cie_sampler.get(), bokeh_view.get(), vk::ImageLayout::eGeneral)}},
        {});

    gpu.get_device().updateDescriptorSets({{mesh_set.get(), 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,
                                            &gfx::info_for(instances.get_mesh_allocator().bvh_buffer())},
                                           {mesh_set.get(), 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,
                                            &gfx::info_for(instances.get_mesh_allocator().vertex_buffer())},
                                           {mesh_set.get(), 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,
                                            &gfx::info_for(instances.get_mesh_allocator().index_buffer())}},
                                          {});

    if (!instances.get_instantiator().instances().empty())
    {
        gpu.get_device().updateDescriptorSets({{mesh_set.get(), 3, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,
                                                &gfx::info_for(instances.get_instantiator().instances())}},
                                              {});
    }

    enum class output_type : int
    {
        standard = 0,
        samples  = 1
    };

    struct globals
    {
        glm::ivec2  viewport;
        float       random;
        int         rendered_count;
        output_type render_output;
    } current_globals{};

    gfx::buffer<globals> globals_buffer(gpu, {current_globals});

    const auto update_target_inputs = [&] {
        gpu.get_device().updateDescriptorSets(
            {{globals_set.get(), 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &gfx::info_for(globals_buffer)},
             {globals_set.get(), 1, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), cie_spectrum_view.get(), vk::ImageLayout::eShaderReadOnlyOptimal)},
             {globals_set.get(), 2, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cubemap_sampler.get(), cubemap_view.get(), vk::ImageLayout::eGeneral)},
             {globals_set.get(), 3, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), color_accum_view.get(), vk::ImageLayout::eGeneral)},
             {globals_set.get(), 4, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), bounce_accum_view.get(), vk::ImageLayout::eGeneral)},
             {globals_set.get(), 5, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), positions_bounce_view.get(), vk::ImageLayout::eGeneral)},
             {globals_set.get(), 6, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), directions_sample_view.get(), vk::ImageLayout::eGeneral)},
             {globals_set.get(), 7, 0, 1, vk::DescriptorType::eCombinedImageSampler,
              &vk::DescriptorImageInfo(cie_sampler.get(), randoms_view.get(), vk::ImageLayout::eGeneral)}},
            {});
    };
    update_target_inputs();

    constexpr auto min_update_time_controls = 16.0ms;
    constexpr auto min_update_time_frame    = 8.0ms;

   gfx::worker control_thread([&](gfx::worker& self, const gfx::worker::duration& delta) {
        ecs.update(delta, control_systems);
        return self.value_after(true, min_update_time_controls);
    });

    size_t                        frame_count = 0;
    std::chrono::duration<double> delta_accum = std::chrono::duration<double>::zero();
    gfx::transform_component      last_transform;

    gfx::ecs::entity* current_view_entity = &*user_entity;
    
    current_globals.rendered_count = 0;
    gfx::worker render_thread([&](gfx::worker& self, const gfx::worker::duration& delta) {
        ++frame_count;
        delta_accum += delta;

        if (delta_accum > 1s)
        {
            fps_counter->setText(QString::fromStdString(std::to_string(frame_count / delta_accum.count())));
            ftm_counter->setText(QString::fromStdString(std::to_string(1'000.0 * delta_accum.count() / frame_count) + "ms"));
            frame_count = 0;
            delta_accum = std::chrono::duration<double>::zero();
        }

        const auto [img, acquire_error] = chain.next_image(acquire_image_signal);
        if (acquire_error && (*acquire_error == gfx::acquire_error::out_of_date || *acquire_error == gfx::acquire_error::suboptimal))
        {
            if (!chain.recreate())
            {
                gfx::ilog << "Could not recreate swapchain. Exiting.";
                return false;
            }
            build_fbos();
            current_globals.rendered_count = 0;
            return true;
        }

        ecs.update(delta, render_systems);
        gpu.get_device().updateDescriptorSets({{mesh_set.get(), 3, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,
                                                &gfx::info_for(instances.get_instantiator().instances())}},
                                              {});

        gpu.wait_for({cmd_fences[img]});
        gpu.reset_fences({cmd_fences[img]});
        gpu_cmd[img].cmd().reset({});
        gpu_cmd[img].cmd().begin({vk::CommandBufferUsageFlagBits::eSimultaneousUse});

        current_view_entity->get<gfx::projection_component>()->perspective().screen_width  = chain.extent().width;
        current_view_entity->get<gfx::projection_component>()->perspective().screen_height = chain.extent().height;
        gpu_cmd[img].cmd().updateBuffer(camera_info_buffer.get_buffer(), 0ull,
                                        vk::ArrayProxy<const gfx::camera_matrices>{*gfx::get_camera_info(*current_view_entity)});

        if (keys.key_down(gfx::key::kb_q) || last_transform != *current_view_entity->get<gfx::transform_component>())
        {
            last_transform = *current_view_entity->get<gfx::transform_component>();
            gpu_cmd[img].cmd().clearColorImage(color_accum->get_image(), vk::ImageLayout::eGeneral,
                                               vk::ClearColorValue(std::array{0.f, 0.f, 0.f, 0.f}),
                                               vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
            current_globals.rendered_count = 0;
        }

        current_globals.random        = dist(gen);
        current_globals.viewport[0]   = chain.extent().width;
        current_globals.viewport[1]   = chain.extent().height;
        current_globals.render_output = output_type(render_outputs->checkedId());
        current_globals.rendered_count++;
        gpu_cmd[img].cmd().updateBuffer(globals_buffer.get_buffer(), 0ull, 1 * sizeof(globals), &current_globals);

        vk::ClearValue clear_values[] = {{vk::ClearColorValue{std::array{0.f, 1.f, 0.f, 1.f}}}};
        gpu_cmd[img].cmd().beginRenderPass(
            {pass.get(), fbos[img].get(), {{0, 0}, chain.extent()}, u32(std::size(clear_values)), std::data(clear_values)},
            vk::SubpassContents::eInline);

        gpu_cmd[img].cmd().setViewport(0, vk::Viewport(0.f, 0.f, chain.extent().width, chain.extent().height, 0.f, 1.f));
        gpu_cmd[img].cmd().setScissor(0, vk::Rect2D({0, 0}, chain.extent()));
        gpu_cmd[img].cmd().bindPipeline(vk::PipelineBindPoint::eGraphics, pipe.get());

        gpu_cmd[img].cmd().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipe_layout.get(), 0,
                                              {mat_set.get(), mesh_set.get(), globals_set.get()}, nullptr);
        gpu_cmd[img].cmd().draw(3, 1, 0, 0);

        gpu_cmd[img].cmd().endRenderPass();
        gpu_cmd[img].cmd().end();

        gpu.graphics_queue().submit({gpu_cmd[img]}, {acquire_image_signal}, {render_finish_signal}, cmd_fences[img]);
        gpu.graphics_queue().wait();
        const auto present_error = gpu.present_queue().present({{img, chain}}, {render_finish_signal});
        if (present_error)
        {
            if (!chain.recreate())
            {
                gfx::ilog << "Could not recreate swapchain. Exiting.";
                return false;
            }
            build_fbos();
            current_globals.rendered_count = 0;
            return true;
        }
        return self.value_after(true, min_update_time_frame);
    });

    QApplication::exec();
}

gfx::image load_cubemap(gfx::device& gpu, const std::filesystem::path& root)
{
    std::future<gfx::image_file> posx_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "posx.hdr", gfx::bits::b32, 4); });
    std::future<gfx::image_file> negx_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "negx.hdr", gfx::bits::b32, 4); });
    std::future<gfx::image_file> posy_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "posy.hdr", gfx::bits::b32, 4); });
    std::future<gfx::image_file> negy_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "negy.hdr", gfx::bits::b32, 4); });
    std::future<gfx::image_file> posz_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "posz.hdr", gfx::bits::b32, 4); });
    std::future<gfx::image_file> negz_future =
        std::async(std::launch::async, [&root]() { return gfx::image_file(root / "negz.hdr", gfx::bits::b32, 4); });

    posx_future.wait();
    negx_future.wait();
    posy_future.wait();
    negy_future.wait();
    posz_future.wait();
    negz_future.wait();

    const auto posx = posx_future.get();
    const auto negx = negx_future.get();
    const auto posy = posy_future.get();
    const auto negy = negy_future.get();
    const auto posz = posz_future.get();
    const auto negz = negz_future.get();

    vk::ImageCreateInfo cube_create;
    cube_create.flags       = vk::ImageCreateFlagBits::eCubeCompatible;
    cube_create.arrayLayers = 6;
    cube_create.extent      = vk::Extent3D(posx.width, posx.height, 1);
    cube_create.format      = vk::Format::eR32G32B32A32Sfloat;
    cube_create.imageType   = vk::ImageType::e2D;
    cube_create.mipLevels   = 1;    // log2(std::max(posx.width, posx.height)) + 1;
    cube_create.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
    gfx::image cube(gpu, cube_create);

    const auto             ptr   = [](const gfx::image_file& f) { return static_cast<glm::vec4*>(f.bytes()); };
    const auto             count = posx.width * posx.height;
    gfx::mapped<glm::vec4> data(gpu);
    data.insert(data.end(), ptr(posx), ptr(posx) + count);
    data.insert(data.end(), ptr(negx), ptr(negx) + count);
    data.insert(data.end(), ptr(posy), ptr(posy) + count);
    data.insert(data.end(), ptr(negy), ptr(negy) + count);
    data.insert(data.end(), ptr(posz), ptr(posz) + count);
    data.insert(data.end(), ptr(negz), ptr(negz) + count);

    gfx::commands copy = gpu.allocate_graphics_command();

    copy.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::ImageMemoryBarrier img_barrier;
    img_barrier.oldLayout        = vk::ImageLayout::eUndefined;
    img_barrier.srcAccessMask    = {};
    img_barrier.newLayout        = vk::ImageLayout::eTransferDstOptimal;
    img_barrier.dstAccessMask    = vk::AccessFlagBits::eMemoryWrite;
    img_barrier.image            = cube.get_image();
    img_barrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 6);
    copy.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                               vk::DependencyFlagBits::eByRegion, {}, {}, img_barrier);

    vk::BufferImageCopy buf_copy;
    buf_copy.imageExtent      = cube_create.extent;
    buf_copy.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 6);
    copy.cmd().copyBufferToImage(data.get_buffer(), cube.get_image(), vk::ImageLayout::eTransferDstOptimal, buf_copy);

    // gen mipmaps.
    img_barrier.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
    img_barrier.srcAccessMask    = vk::AccessFlagBits::eMemoryWrite;
    img_barrier.newLayout        = vk::ImageLayout::eGeneral;
    img_barrier.dstAccessMask    = {};
    img_barrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 6);
    copy.cmd().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                               vk::DependencyFlagBits::eByRegion, {}, {}, img_barrier);

    copy.cmd().end();
    gpu.graphics_queue().submit({copy}, {}, {});
    gpu.graphics_queue().wait();
    return cube;
}