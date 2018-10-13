#include "graphics/graphics.hpp"

#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QWindow>
#include <gfx/log.hpp>
#include <QBoxLayout>
#include <QFrame>
#include <QMainWindow>
#include <QMenuBar>
#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>

class worker
{
public:
    template<typename Fun>
    worker(Fun&& fun) : _stopped(false), _worker_thread([&] { while(!_stopped && fun(*this)); })
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

    QMainWindow win;
    win.resize(1280, 720);
    QMenuBar* menuBar  = new QMenuBar();
    QMenu*    fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);
    fileMenu->addAction("Save");
    fileMenu->addAction("Exit");

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QWidget*     central    = new QWidget;
    win.setCentralWidget(central);
    central->setLayout(mainLayout);
    mainLayout->setMenuBar(menuBar);
    QFrame* render_frame = new QFrame;
    render_frame->setFrameStyle(QFrame::Shadow_Mask);
    render_frame->setFrameShadow(QFrame::Sunken);
    QVBoxLayout* render_frame_layout = new QVBoxLayout;
    render_frame->setLayout(render_frame_layout);
    QWidget* render_surface = new QWidget;
    render_frame_layout->addWidget(render_surface);
    mainLayout->addWidget(render_frame, 1);

    QVBoxLayout* left_panel_layout = new QVBoxLayout;
    QWidget*     left_panel        = new QWidget;
    mainLayout->addWidget(left_panel);
    left_panel->setLayout(left_panel_layout);
    left_panel_layout->addWidget(new QPushButton("Press me, I'm Qt"));

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
	std::vector<gfx::fence> cmd_fences;
	for (size_t i = 0; i < gpu_cmd.size(); ++i)
		cmd_fences.emplace_back(gpu, true);
	worker                     render_thread([&](worker& self) {
		const auto[img, aquire_error] = chain.next_image(acquire_image_signal);
		if (aquire_error && (*aquire_error == gfx::acquire_error::out_of_date || *aquire_error == gfx::acquire_error::suboptimal))
		{
			if (!chain.recreate())
			{
				gfx::ilog << "Could not recreate swapchain. Exiting.";
				return false;
			}
			return true;
		}
		gpu.wait_for({ cmd_fences[img] });
		gpu.reset_fences({ cmd_fences[img] });
        gpu_cmd[img].cmd().reset({});
        gpu_cmd[img].cmd().begin({vk::CommandBufferUsageFlagBits::eSimultaneousUse});
        gpu_cmd[img].cmd().clearColorImage(chain.imgs()[img], vk::ImageLayout::eUndefined, vk::ClearColorValue{clear_color},
                                           vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
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
			return true;
        }
        
		return true;
    });

    return app.exec();
}
