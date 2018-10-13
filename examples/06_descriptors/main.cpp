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
#include <QStatusBar>
#include <QSplitter>
#include <QStyleFactory>

class worker
{
public:
	template<typename Fun>
	worker(Fun&& fun) : _stopped(false), _worker_thread([&] { while (!_stopped && fun(*this)); })
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
	QMenuBar* menuBar = new QMenuBar();
	QMenu*    fileMenu = new QMenu("File");
	menuBar->addMenu(fileMenu);
	fileMenu->addAction("Open", [] { gfx::ilog << "Open..."; }, QKeySequence::Open);
	fileMenu->addAction("Save", [] {gfx::ilog << "Save..."; }, QKeySequence::Save);
	fileMenu->addAction("Save As", [] {gfx::ilog << "Save as..."; }, QKeySequence::SaveAs);
	fileMenu->addSeparator();
	fileMenu->addAction("Close", [&win] { win.close(); }, QKeySequence::Close);
	fileMenu->addAction("Quit", [&win] { QCoreApplication::quit(); }, QKeySequence::Quit);

	QSplitter* mainLayout = new  QSplitter;
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
	QWidget*     right_panel = new QWidget;
	mainLayout->addWidget(right_panel);
	right_panel->setLayout(right_panel_layout);
	
	QTabWidget* tabWidget = new QTabWidget;
	QWidget* general = new QWidget;
	QVBoxLayout* general_layout = new QVBoxLayout;
	general->setLayout(general_layout);
	QGroupBox* gfx_info = new QGroupBox("Graphics");
	QFormLayout* gfx_info_layout = new QFormLayout;
	gfx_info->setLayout(gfx_info_layout);
	general_layout->addWidget(gfx_info);
	gfx_info_layout->addRow("Renderer", new QLabel("Vulkan"));
	gfx_info_layout->addRow("Version", new QLabel("1.1"));
	gfx_info_layout->addRow("Device", new QLabel("NVidia GTX760"));
	general_layout->addWidget(new QWidget, 1);

	tabWidget->addTab(general, "General");
	tabWidget->addTab(new QWidget, "Permissions");
	right_panel_layout->addWidget(tabWidget);
	right_panel_layout->setContentsMargins(QMargins(0, 0, 0, 0));
	win.setStatusBar(new QStatusBar);
	mainLayout->addWidget(render_frame);

	QVBoxLayout* left_panel_layout = new QVBoxLayout;
	QWidget*     left_panel = new QWidget;
	mainLayout->addWidget(left_panel);
	left_panel->setLayout(left_panel_layout);
	left_panel_layout->addWidget(new QPushButton("Press me, I'm Qt"));
	left_panel_layout->setContentsMargins(QMargins(0, 0, 0, 0));

	QGroupBox*   inputs = new QGroupBox("Inputs");
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

	mainLayout->setSizes(QList<int>({ 250, 1280, 250 }));

	std::array<float, 4> clear_color = { 0.f, 0.f, 0.f, 1.f };
	win.connect(slider_r, &QSlider::valueChanged, [&](int val) { clear_color[0] = val / 255.f; });
	win.connect(slider_g, &QSlider::valueChanged, [&](int val) { clear_color[1] = val / 255.f; });
	win.connect(slider_b, &QSlider::valueChanged, [&](int val) { clear_color[2] = val / 255.f; });
	win.show();

	gfx::instance              my_app("Application", gfx::version_t(1, 0, 0), true, true);
	gfx::surface               surf1(my_app, render_surface);
	gfx::device                gpu(my_app, gfx::device_target::gpu, { 1.f, 0.5f }, 1.f, surf1);
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
		}
		gpu.wait_for({ cmd_fences[img] });
		gpu.reset_fences({ cmd_fences[img] });
		gpu_cmd[img].cmd().reset({});
		gpu_cmd[img].cmd().begin({ vk::CommandBufferUsageFlagBits::eSimultaneousUse });
		gpu_cmd[img].cmd().clearColorImage(chain.imgs()[img], vk::ImageLayout::eUndefined, vk::ClearColorValue{ clear_color },
			vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
		gpu_cmd[img].cmd().end();

		gpu.graphics_queue().submit({ gpu_cmd[img] }, { acquire_image_signal }, { render_finish_signal }, cmd_fences[img]);
		gpu.graphics_queue().wait();
		const auto present_error = gpu.present_queue().present({ {img, chain} }, { render_finish_signal });
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
