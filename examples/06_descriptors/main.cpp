#include "graphics/graphics.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleFactory>
#include <QWidget>
#include <QWindow>
#include <gfx/log.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class worker
{
public:
    template<typename Fun>
    worker(Fun&& fun)
          : _stopped(false), _worker_thread([&] {
              while (!_stopped && fun(*this))
                  ;
          })
    {}
    ~worker()
    {
        _stopped = true;
        _worker_thread.join();
    }

    const std::atomic_bool& has_stopped() const noexcept { return _stopped; }
    void                    trigger_stop() noexcept { _stopped = true; }

private:
    std::atomic_bool _stopped;
    std::thread      _worker_thread;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("fusion"));
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize() + 2);
    app.setFont(defaultFont);
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
    QMenuBar* menuBar  = new QMenuBar();
    QMenu*    fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);
    fileMenu->addAction("Open", [] { gfx::ilog << "Open..."; }, QKeySequence::Open);
    fileMenu->addAction("Save", [] { gfx::ilog << "Save..."; }, QKeySequence::Save);
    fileMenu->addAction("Save As", [] { gfx::ilog << "Save as..."; }, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Close", [&win] { win.close(); }, QKeySequence::Close);
    fileMenu->addAction("Quit", [&win] { QCoreApplication::quit(); }, QKeySequence::Quit);

    QSplitter* mainLayout = new QSplitter;
    win.setCentralWidget(mainLayout);
    mainLayout->setContentsMargins(QMargins(8, 8, 8, 8));
    win.setMenuBar(menuBar);
    QFrame* render_frame = new QFrame;
    render_frame->setFrameStyle(QFrame::Shadow_Mask);
    render_frame->setFrameShadow(QFrame::Sunken);
    QVBoxLayout* render_frame_layout = new QVBoxLayout;
    render_frame->setLayout(render_frame_layout);
    render_frame->setStyleSheet("background-color: black");
    render_frame_layout->setContentsMargins(QMargins(1, 1, 1, 1));
    QWidget* render_surface = new QWidget;
    render_frame_layout->addWidget(render_surface);

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
    general_layout->addWidget(new QWidget, 1);

    tabWidget->addTab(general, "General");
    tabWidget->addTab(new QWidget, "Permissions");
    right_panel_layout->addWidget(tabWidget);
    right_panel_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    win.setStatusBar(new QStatusBar);
    mainLayout->addWidget(render_frame);

    QVBoxLayout* left_panel_layout = new QVBoxLayout;
    QWidget*     left_panel        = new QWidget;
    mainLayout->addWidget(left_panel);
    left_panel->setLayout(left_panel_layout);
    left_panel_layout->addWidget(new QPushButton("Press me, I'm Qt"));
    left_panel_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    QGroupBox*   inputs        = new QGroupBox("Inputs");
    QFormLayout* inputs_layout = new QFormLayout;
    inputs->setLayout(inputs_layout);
    left_panel_layout->addWidget(inputs);
    QSlider* slider_r = new QSlider(Qt::Horizontal);
    QSlider* slider_g = new QSlider(Qt::Horizontal);
    QSlider* slider_b = new QSlider(Qt::Horizontal);
    slider_r->setRange(0, 255);
    slider_g->setRange(0, 255);
    slider_b->setRange(0, 255);
    inputs_layout->addRow(new QLabel("Red"), slider_r);
    inputs_layout->addRow(new QLabel("Green"), slider_g);
    inputs_layout->addRow(new QLabel("Blue"), slider_b);

    mainLayout->setSizes(QList<int>({250, 1280, 250}));

    std::array<float, 4> clear_color = {0.f, 0.f, 0.f, 1.f};
    win.connect(slider_r, &QSlider::valueChanged, [&](int val) { clear_color[0] = val / 255.f; });
    win.connect(slider_g, &QSlider::valueChanged, [&](int val) { clear_color[1] = val / 255.f; });
    win.connect(slider_b, &QSlider::valueChanged, [&](int val) { clear_color[2] = val / 255.f; });
    win.show();

















    gfx::instance              my_app("Application", gfx::version_t(1, 0, 0), true, true);
    gfx::surface               surf1(my_app, render_surface);
    gfx::device                gpu(my_app, gfx::device_target::gpu, {1.f, 0.5f}, 1.f, surf1);
    gfx::swapchain             chain(gpu, surf1);
    std::vector<gfx::commands> gpu_cmd = gpu.allocate_graphics_commands(chain.count());
    gfx::semaphore             acquire_image_signal(gpu);
    gfx::semaphore             render_finish_signal(gpu);
    std::vector<gfx::fence>    cmd_fences(gpu_cmd.size(), gfx::fence(gpu, true));

    gfx::mapped<float> my_floats(gpu);
    for (int i = 0; i < 10; ++i) my_floats.emplace_back(i);
	my_floats.insert(my_floats.begin(), { 0.f, 1.f, 0.3f });
	my_floats.erase(my_floats.begin() + 4);

	gfx::buffer<float> tbuf(gpu, my_floats);
	gfx::buffer<float> tbuf2 = tbuf;

	const auto props = gpu.get_physical_device().getProperties2();
    const char*                   device_type = [&] {
        using dt = vk::PhysicalDeviceType;
        switch (props.properties.deviceType)
        {
        case dt::eCpu: return "CPU";
        case dt::eDiscreteGpu: return "Discrete GPU";
        case dt::eIntegratedGpu: return "iGPU";
        case dt::eOther: return "Unknown";
        case dt::eVirtualGpu: return "Virtual GPU";
        }
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

    vk::AttachmentDescription color_attachment_desc;
    color_attachment_desc.initialLayout  = vk::ImageLayout::eUndefined;
    color_attachment_desc.finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    color_attachment_desc.format         = chain.format();
    color_attachment_desc.loadOp         = vk::AttachmentLoadOp::eClear;
    color_attachment_desc.storeOp        = vk::AttachmentStoreOp::eStore;
    color_attachment_desc.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    color_attachment_desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    color_attachment_desc.samples        = vk::SampleCountFlagBits::e1;

    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription main_subpass;
    main_subpass.colorAttachmentCount = 1;
    main_subpass.pColorAttachments    = &color_attachment_reference;
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
    rp_info.attachmentCount = 1;
    rp_info.pAttachments    = &color_attachment_desc;
    rp_info.subpassCount    = 1;
    rp_info.pSubpasses      = &main_subpass;
    rp_info.dependencyCount = 1;
    rp_info.pDependencies   = &main_subpass_dep;
    const auto pass         = gpu.get_device().createRenderPassUnique(rp_info);

    std::vector<vk::UniqueImageView>   imvs;
    std::vector<vk::UniqueFramebuffer> fbos;

    const auto build_fbos = [&] {
        fbos.clear();
        imvs.clear();
        for (size_t i = 0; i < chain.count(); ++i)
        {
            vk::ImageViewCreateInfo imv_create;
            imv_create.format           = chain.format();
            imv_create.image            = chain.imgs()[i];
            imv_create.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            imv_create.viewType         = vk::ImageViewType::e2D;
            imvs.emplace_back(gpu.get_device().createImageViewUnique(imv_create));

            vk::FramebufferCreateInfo fbo_create;
            fbo_create.attachmentCount = 1;
            fbo_create.renderPass      = pass.get();
            fbo_create.width           = chain.extent().width;
            fbo_create.height          = chain.extent().height;
            fbo_create.layers          = 1;
            fbo_create.pAttachments    = &imvs[i].get();
            fbos.emplace_back(gpu.get_device().createFramebufferUnique(fbo_create));
        }
    };
    build_fbos();

    const gfx::shader vert(gpu, "06_descriptors/do.vert.vk.spv");
    const gfx::shader frag(gpu, "06_descriptors/do.frag.vk.spv");

    vk::GraphicsPipelineCreateInfo pipe_info;
    pipe_info.subpass    = 0;
    pipe_info.renderPass = pass.get();
    const auto stages    = {
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vert.get_module(), "main"),
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, frag.get_module(), "main"),
    };
    pipe_info.stageCount = gfx::u32(std::size(stages));
    pipe_info.pStages    = std::data(stages);

    vk::DescriptorPoolSize       dpool_size(vk::DescriptorType::eUniformBuffer, 256);
    vk::DescriptorPoolCreateInfo dpool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 2048, 1, &dpool_size);
    vk::UniqueDescriptorPool     dpool = gpu.get_device().createDescriptorPoolUnique(dpool_info);

    vk::DescriptorSetLayoutBinding    mat_binding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll);
    vk::DescriptorSetLayoutCreateInfo dset_create({}, 1, &mat_binding);
    vk::UniqueDescriptorSetLayout     mat_set_layout = gpu.get_device().createDescriptorSetLayoutUnique(dset_create);

    vk::PipelineLayoutCreateInfo pipe_layout_info({}, 1, &mat_set_layout.get());

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

    vk::PipelineColorBlendAttachmentState blend_col_att0;
    blend_col_att0.blendEnable = false;
    blend_col_att0.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo bln_state({}, false, {}, 1, &blend_col_att0);
    pipe_info.pColorBlendState = &bln_state;

    vk::UniquePipeline pipe = gpu.get_device().createGraphicsPipelineUnique(nullptr, pipe_info);
	gfx::mapped<glm::mat4> buf(gpu, 1);
    vk::DescriptorSetAllocateInfo mat_set_alloc(dpool.get(), 1, &mat_set_layout.get());
    vk::UniqueDescriptorSet       mat_set = std::move(gpu.get_device().allocateDescriptorSetsUnique(mat_set_alloc)[0]);

    vk::DescriptorBufferInfo mat_buf_info(buf.get_buffer(), 0, sizeof(glm::mat4));
    vk::WriteDescriptorSet   mat_write(mat_set.get(), 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &mat_buf_info);
    gpu.get_device().updateDescriptorSets(mat_write, {});

    std::chrono::duration<double> time_sec    = std::chrono::duration<double>::zero();
    std::chrono::duration<double> delta       = std::chrono::duration<double>::zero();
    size_t                        frame_count = 0;
    std::chrono::time_point       begin       = std::chrono::steady_clock::now();
    std::chrono::time_point       frame_begin = std::chrono::steady_clock::now();
    worker                        render_thread([&](worker& self) {
        using namespace std::chrono_literals;
        time_sec = std::chrono::steady_clock::now() - begin;
        ++frame_count;
        delta = std::chrono::steady_clock::now() - frame_begin;
        if (delta > 1s)
        {
            fps_counter->setText(QString::fromStdString(std::to_string(frame_count / delta.count())));
            ftm_counter->setText(QString::fromStdString(std::to_string(1'000.0 * delta.count() / frame_count) + "ms"));
            frame_begin = std::chrono::steady_clock::now();
            frame_count = 0;
            delta       = std::chrono::duration<double>::zero();
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
        }

        gpu.wait_for({cmd_fences[img]});
        gpu.reset_fences({cmd_fences[img]});
        gpu_cmd[img].cmd().reset({});
        gpu_cmd[img].cmd().begin({vk::CommandBufferUsageFlagBits::eSimultaneousUse});
		const auto data = { glm::rotate(glm::radians<float>(time_sec.count() * 180.f), glm::vec3(0, 0, 1)) };
		gpu_cmd[img].cmd().updateBuffer(buf.get_buffer(), 0ull, std::size(data) * sizeof(glm::mat4), std::data(data));

        vk::ClearValue          clr(vk::ClearColorValue{clear_color});
        vk::RenderPassBeginInfo beg;
        beg.clearValueCount = 1;
        beg.pClearValues    = &clr;
        beg.framebuffer     = fbos[img].get();
        beg.renderPass      = pass.get();
        beg.renderArea      = vk::Rect2D({0, 0}, chain.extent());
        gpu_cmd[img].cmd().beginRenderPass(beg, vk::SubpassContents::eInline);

        gpu_cmd[img].cmd().setViewport(0, vk::Viewport(0.f, 0.f, chain.extent().width, chain.extent().height, 0.f, 1.f));
        gpu_cmd[img].cmd().setScissor(0, vk::Rect2D({0, 0}, chain.extent()));
        gpu_cmd[img].cmd().bindPipeline(vk::PipelineBindPoint::eGraphics, pipe.get());

        gpu_cmd[img].cmd().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipe_layout.get(), 0, mat_set.get(), nullptr);
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
        }

        return true;
    });

    app.exec();
}
