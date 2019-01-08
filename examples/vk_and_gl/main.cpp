#include "app.hpp"
#include "opengl.hpp"
#include "vulkan.hpp"
#include "scene.hpp"

#include <filesystem>

#include <gfx.ecs.defaults2/prototype.hpp>

#include <QtCharts/QtCharts>
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

class app_tab : public QDockWidget
{
public:
    app_tab(gfx::basic_app* app, const QString& title, QWidget* parent = nullptr) : QDockWidget(title, parent), _app(app)
    {
        setMinimumWidth(500);
        setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        _frame_times = new QLineSeries(this);
        _frame_times->setName("Frame Time");
        QChart*     chart = new QChart();
        QValueAxis* axisX = new QValueAxis();
        axisX->setTitleText("Elapsed Time (s)");
        chart->addAxis(axisX, Qt::AlignBottom);
        QValueAxis* axisY = new QValueAxis();
        axisY->setTitleText("FT (ms)");
        chart->addAxis(axisY, Qt::AlignLeft);

        constexpr const char* series_names[] {"Frame Begin", "Shadow Map", "Render"};

        for (int i = 1; i < std::size(_stamp_times); ++i)
        {
            _stamp_times[i] = new QLineSeries(this);
            _stamp_times[i]->setName(series_names[i]);
            chart->addSeries(_stamp_times[i]);
            _stamp_times[i]->attachAxis(axisX);
            _stamp_times[i]->attachAxis(axisY);
        }

        chart->addSeries(_frame_times);
        chart->setTitle("Frame Times");
        _frametime_chart_view = new QChartView(chart);
        _frametime_chart_view->setRenderHint(QPainter::Antialiasing);
        _frametime_chart_view->setFixedHeight(450);
        _frame_times->attachAxis(axisX);
        _frame_times->attachAxis(axisY);

        auto w = new QWidget(this);
        setWidget(w);
        auto gl = new QVBoxLayout(w);
        gl->addWidget(_frametime_chart_view, 0, Qt::AlignTop);
        auto save_btn = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), "Save Plot", this);
        connect(save_btn, &QPushButton::clicked, [this] {
            QPixmap p        = _frametime_chart_view->grab();
            QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "", tr("Images (*.png)"));
            if (!filename.isEmpty()) p.save(filename, "PNG");
        });

        auto below_graph = new QWidget(w);
        gl->addWidget(below_graph, 0, Qt::AlignTop);
        gl->addWidget(new QWidget(this), 1);
        QHBoxLayout* below_graph_layout = new QHBoxLayout(below_graph);
        auto         info_group         = new QGroupBox("Info", below_graph);
        auto         info_content       = new QVBoxLayout(info_group);
        below_graph_layout->addWidget(info_group, 0, Qt::AlignTop);

        QWidget* info_form        = new QWidget(info_group);
        auto     info_form_layout = new QFormLayout(info_form);

        info_form_layout->addRow("Current Frame Time", _framerate_label = new QLabel(info_form));

        for (int i = 1; i < std::size(_stamp_times); ++i)
        { info_form_layout->addRow(series_names[i], _timestamp_labels[i - 1] = new QLabel(info_form)); }

        info_content->addWidget(info_form);
        info_content->addWidget(save_btn);

        QTimer* t = new QTimer(this);
        connect(t, &QTimer::timeout, this, &app_tab::update_chart);
        t->start(time_step);

        QTimer* accum = new QTimer(this);
        connect(accum, &QTimer::timeout, [this] {
            _accum_frametimes.push_back(_app->current_frametime());
            std::shared_lock lock(_app->stamp_time_mutex());
            for (int i = 1; i < std::size(_accum_stamptimes); ++i)
            {
                const auto diff =
                    _app->stamp_times().at(i) > _app->stamp_times().at(0) ? _app->stamp_times().at(i) - _app->stamp_times().at(0) : 0;
                if (diff > 0) _accum_stamptimes[i].emplace_back(std::chrono::nanoseconds(diff));
            }
        });
        accum->start(25);

        below_graph_layout->addWidget(new QWidget(this), 1);
    }

    void update_chart()
    {
        _frametime_min += (time_step / 1000.0);
        _frametime_max += (time_step / 1000.0);

        if (_frame_times->count() + 1 > max_samples) { _frame_times->remove(0); }
        for (int i = 1; i < std::size(_stamp_times); ++i)
        {
            if (_stamp_times[i]->count() + 1 > max_samples) { _stamp_times[i]->remove(0); }
        }
        const auto acc = std::accumulate(_accum_frametimes.begin(), _accum_frametimes.end(), std::chrono::nanoseconds(0));
        auto       ft  = acc.count() / (1'000'000.0 * _accum_frametimes.size());
        *_frame_times << QPointF(_frametime_max, ft);

        double last = 0.0;
        for (int i = 1; i < std::size(_stamp_times); ++i)
        {
            const auto acc = std::accumulate(_accum_stamptimes[i].begin(), _accum_stamptimes[i].end(), std::chrono::nanoseconds(0));
            auto       ft  = acc.count() / (1'000'000.0 * _accum_stamptimes[i].size());
            *_stamp_times[i] << QPointF(_frametime_max, ft);
            _accum_stamptimes[i].clear();
            _timestamp_labels[i - 1]->setText(QString::fromStdString(std::to_string(ft - last) + "ms"));
            last = ft;
        }
        _frametime_chart_view->repaint();
        _framerate_label->setText(QString::fromStdString(std::to_string(ft) + "ms"));

        _frametime_chart_view->chart()->axisX()->setRange(_frametime_min, _frametime_max);
        _frametime_chart_view->chart()->axisY()->setRange(0, 16);
        _accum_frametimes.clear();
    }

private:
    constexpr static auto                 max_samples = 100;
    constexpr static auto                 time_step   = 100.0;
    gfx::basic_app*                       _app;
    QLineSeries*                          _frame_times;
    QLineSeries*                          _stamp_times[gfx::basic_app::_stamp_id_count];
    QChartView*                           _frametime_chart_view;
    QLabel*                               _framerate_label;
    QLabel*                               _timestamp_labels[gfx::basic_app::_stamp_id_count - 1];
    double                                _frametime_min = -max_samples * (time_step / 1000.0);
    double                                _frametime_max = 0;
    std::vector<std::chrono::nanoseconds> _accum_frametimes;
    std::vector<std::chrono::nanoseconds> _accum_stamptimes[gfx::basic_app::_stamp_id_count];
};

class control_window : public QMainWindow
{
public:
    control_window()
    {
        auto dw = new QDockWidget("Settings");
        dw->setMaximumWidth(350);
        dw->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        setDockNestingEnabled(true);
        setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);
        addDockWidget(Qt::LeftDockWidgetArea, dw);
        dw->setWidget(_settings_tabs = new QTabWidget(this));

        init_general_tab();
        _settings_tabs->addTab(_general_tab.central, "General");
    }
    ~control_window()
    {
        for (auto& a : _running_apps)
        {
            a.first->hint_close(true);
            a.first->wait_until_closed();
        }
    }

public:
    void tab_gc()
    {
        for (auto it = _running_apps.begin(); it != _running_apps.end();)
        {
            if ((*it).first->finished())
            {
                removeDockWidget(it->second.get());
                it = _running_apps.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    void init_general_tab()
    {
        _general_tab.central        = new QWidget(_settings_tabs);
        _general_tab.central_layout = new QVBoxLayout(_general_tab.central);

        _general_tab.select_app_combo = new QComboBox(_general_tab.central);
        _general_tab.select_app_combo->addItem("Vulkan");
        _general_tab.select_app_combo->addItem("OpenGL");

        connect(_general_tab.select_app_combo, QOverload<int>::of(&QComboBox::activated),
                [this](int index) { _general_tab.run_props.current_selection = index; });

        _general_tab.run_load_progress = new QProgressBar(_general_tab.central);
        _general_tab.run_load_progress->setRange(0, 1000);
        _general_tab.run_load_progress->setValue(0);
        _general_tab.run_load_progress->setVisible(false);
        _general_tab.run_app = new QPushButton("+ Run", _general_tab.central);

        QTimer* progress_updater = new QTimer(this);
        connect(_general_tab.run_app, &QPushButton::clicked, [this, progress_updater] {
            switch (_general_tab.run_props.current_selection)
            {
            case 0:
            {
                auto app = std::make_unique<vulkan_app>();
                auto p   = app.get();
                addDockWidget(
                    Qt::RightDockWidgetArea,
                    _running_apps
                        .emplace_back(
                            std::move(app),
                            std::make_unique<app_tab>(
                                p, QString::fromStdString(
                                       "Vulkan (ID: " + std::to_string(++_tab_counters[_general_tab.run_props.current_selection]) + ")")))
                        .second.get());
            }
            break;
            case 1:
            {
                auto app = std::make_unique<opengl_app>();
                auto p   = app.get();
                addDockWidget(
                    Qt::RightDockWidgetArea,
                    _running_apps
                        .emplace_back(
                            std::move(app),
                            std::make_unique<app_tab>(
                                p, QString::fromStdString(
                                       "Opengl (ID: " + std::to_string(++_tab_counters[_general_tab.run_props.current_selection]) + ")")))
                        .second.get());
            }
            break;
            default: break;
            }
            if (_running_apps.size() >= 2)
            {
                tabifyDockWidget(_running_apps[_running_apps.size() - 2].second.get(),
                                 _running_apps[_running_apps.size() - 1].second.get());
            }
            // set current scene
            const float                 scale      = static_cast<float>(_general_tab.scene_scale->value());
            const std::filesystem::path scene_path = _general_tab.scene_select->text().toStdString();
            scene::scene_manager.load(scene_path, scale, true, [this](float p) -> bool {
                gfx::ilog << "Progress: " << p;
                _general_tab.current_load_progress = 1000 * p;
                return true;
            });
            _running_apps.back().first->run();
            progress_updater->start(100);
            _general_tab.run_load_progress->setVisible(true);
            _general_tab.run_app->setVisible(false);
        });
        connect(progress_updater, &QTimer::timeout, [this, progress_updater] {
            _general_tab.run_load_progress->setValue(_general_tab.current_load_progress);
            if (_general_tab.current_load_progress == 1000)
            {
                progress_updater->stop();
                _general_tab.run_load_progress->setVisible(false);
                _general_tab.run_app->setVisible(true);
            }
        });

        _general_tab.run_group = new QGroupBox("Run App", _general_tab.central);
        _general_tab.central_layout->addWidget(_general_tab.run_group);
        _general_tab.run_group_layout = new QVBoxLayout(_general_tab.run_group);
        _general_tab.run_settings     = new QWidget(_general_tab.run_group);
        _general_tab.run_group_layout->addWidget(_general_tab.run_settings, 1);
        _general_tab.run_settings_layout = new QFormLayout(_general_tab.run_settings);

        _general_tab.scene_scale = new QDoubleSpinBox(_general_tab.run_group);
        _general_tab.scene_scale->setDecimals(2);
        _general_tab.scene_scale->setMinimum(0.01);
        _general_tab.scene_scale->setMaximum(100.0);
        _general_tab.scene_scale->setValue(1.0);

        QWidget* scene_select_line = new QWidget(_general_tab.run_group);
        scene_select_line->setContentsMargins(0, 0, 0, 0);
        _general_tab.scene_select = new QLineEdit(scene_select_line);
        _general_tab.scene_select->setText("sponza/sponza.obj");

        QPushButton* scene_select_browse =
            new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon), "Browse", scene_select_line);

        connect(scene_select_browse, &QPushButton::clicked, [this] {
            QString fileName =
                QFileDialog::getOpenFileName(this, tr("Load Scene Book"), "", tr("Geometry File (*.obj *.dae *.fbx *.blend)"));
            if (!fileName.isEmpty()) _general_tab.scene_select->setText(fileName);
        });

        QHBoxLayout* scene_select_line_layout = new QHBoxLayout(scene_select_line);
        scene_select_line_layout->setMargin(0);
        scene_select_line_layout->setContentsMargins(0, 0, 0, 0);
        scene_select_line_layout->addWidget(_general_tab.scene_select, 1);
        scene_select_line_layout->addWidget(scene_select_browse, 0);

        _general_tab.run_settings_layout->addRow("API", _general_tab.select_app_combo);
        _general_tab.run_settings_layout->addRow("Scene File", scene_select_line);
        _general_tab.run_settings_layout->addRow("Scene Scale", _general_tab.scene_scale);
        _general_tab.run_group_layout->addWidget(_general_tab.run_app);
        _general_tab.run_group_layout->addWidget(_general_tab.run_load_progress);


        // Check for finished apps every 500ms
        _general_tab.gc_timer = new QTimer(this);
        this->moveToThread(thread());
        _general_tab.gc_timer->moveToThread(thread());
        _general_tab.gc_timer->setTimerType(Qt::PreciseTimer);
        connect(_general_tab.gc_timer, &QTimer::timeout, this, &control_window::tab_gc);
        _general_tab.gc_timer->start(100);
    }

    QTabWidget* _settings_tabs;

    struct
    {
        QTimer* gc_timer;

        QWidget*     central;
        QVBoxLayout* central_layout;

        QGroupBox*   run_group;
        QVBoxLayout* run_group_layout;
        QWidget*     run_settings;
        QFormLayout* run_settings_layout;

        QComboBox*      select_app_combo;
        QLineEdit*      scene_select;
        QDoubleSpinBox* scene_scale;
        QPushButton*    run_app;
        QProgressBar*   run_load_progress;
        std::atomic_int current_load_progress = 0;

        struct
        {
            int current_selection = 0;
        } run_props;

    } _general_tab {};

    std::vector<std::pair<std::unique_ptr<gfx::basic_app>, std::unique_ptr<app_tab>>> _running_apps;
    std::unordered_map<int, size_t>                                                   _tab_counters;
};

int main(int argc, char** argv)
{
    using namespace std::string_view_literals;

    QApplication app(argc, argv);
    // set style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // increase font size for better reading
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize() + 2);
    QApplication::setFont(defaultFont);
    // modify palette to dark
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
    QApplication::setPalette(darkPalette);

    control_window controls;
    controls.show();
    QApplication::exec();
}
