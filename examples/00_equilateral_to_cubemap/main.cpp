#include <gfx/gfx.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QThread>
#include <QVector>
#include <QtConcurrent>
#include <QStyleFactory>

std::array<std::string, 6> sides{"posx", "negx", "negy", "posy", "posz", "negz"};

glm::mat4 make_matrix(const glm::vec3 look_dir, const glm::vec3 up)
{
    return glm::mat4(glm::mat3{normalize(cross(look_dir, up)), up, -look_dir});
}

const std::array<glm::mat4, 6> matrices{
    make_matrix({1, 0, 0}, {0, 1, 0}),  make_matrix({-1, 0, 0}, {0, 1, 0}), make_matrix({0, 1, 0}, {0, 0, -1}),
    make_matrix({0, -1, 0}, {0, 0, 1}), make_matrix({0, 0, 1}, {0, 1, 0}),  make_matrix({0, 0, -1}, {0, 1, 0}),
};

const glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.01f, 100.f);

class ConverterWindow : public QMainWindow
{
public:
    constexpr static int  default_resolution    = 1024;
    constexpr static bool default_hdr           = true;
    constexpr static bool default_png           = false;
    constexpr static bool default_gen_subfolder = true;

    ConverterWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("Equilateral to Cube");
        setCentralWidget(&_content);
        {        // Add Paths group
            {    // Add Source path selector field
                auto* file_selector        = new QWidget;
                auto* file_selector_layout = new QHBoxLayout;
                file_selector->setLayout(file_selector_layout);
                auto* browse = new QPushButton;
                browse->setText("...");
                browse->setMaximumWidth(24);
                connect(browse, &QPushButton::clicked, this, &ConverterWindow::on_browse_src);
                file_selector_layout->addWidget(&_input_path_edit, 1);
                file_selector_layout->addWidget(browse, 0);
                file_selector_layout->setMargin(0);
                _paths_layout.addRow("Input HDR", file_selector);
            }

            {    // Add Destination path selector field
                auto* file_selector        = new QWidget;
                auto* file_selector_layout = new QHBoxLayout;
                file_selector->setLayout(file_selector_layout);
                auto* browse = new QPushButton;
                browse->setText("...");
                browse->setMaximumWidth(24);
                connect(browse, &QPushButton::clicked, this, &ConverterWindow::on_browse_dst);
                file_selector_layout->addWidget(&_output_path_edit, 1);
                file_selector_layout->addWidget(browse, 0);
                file_selector_layout->setMargin(0);
                _paths_layout.addRow("Output Folder", file_selector);

                _generate_subfolder_check = new QCheckBox("Generate Subfolder");
                _generate_subfolder_check->setChecked(default_gen_subfolder);
                _paths_layout.addRow("", _generate_subfolder_check);
            }

            auto* group = new QGroupBox("Paths");
            group->setLayout(&_paths_layout);
            _content_layout.addWidget(group, 0);
        }

        {        // Add options panel
            {    // Resolution
                _resolution_edit.setValidator(new QIntValidator(0, 2 << 13, this));
                _resolution_edit.setText(std::to_string(default_resolution).c_str());
                _options_layout.addRow("Output Resolution", &_resolution_edit);

                auto* vbox = new QVBoxLayout;
                vbox->addWidget(_generate_hdr_check = new QCheckBox("*.hdr"));
                vbox->addWidget(_generate_png_check = new QCheckBox("*.png"));

                _generate_hdr_check->setChecked(default_hdr);
                _generate_png_check->setChecked(default_png);

                _options_layout.addRow("Output Formats", vbox);
            }

            auto* group = new QGroupBox("Options");
            group->setLayout(&_options_layout);
            _content_layout.addWidget(group, 0);
        }

        auto* run = new QPushButton;
        run->setText("Generate");
        connect(run, &QPushButton::clicked, this, &ConverterWindow::on_generate);
        _content_layout.addWidget(run);
        centralWidget()->setLayout(&_content_layout);
    }

private slots:
    void on_browse_src()
    {
        if (const auto src = gfx::file::open_dialog("Open HDRI", "./", {"*.hdr"}, {"HDRI Images"}))
            _input_path_edit.setText(src.value().path.string().c_str());
    }
    void on_browse_dst()
    {
        const auto p = absolute(std::filesystem::path(".")).make_preferred();
        if (const auto dst = gfx::file::folder_dialog("Select Target Folder", p))
        { _output_path_edit.setText(dst.value().string().c_str()); } }

    void on_generate()
    {
        std::filesystem::path path = _input_path_edit.text().toStdString();

        if(!exists(path))
        {
			QMessageBox::critical(this, "Error", "The input path is empty or pointing to a non-existent file.", QMessageBox::Ok);
			return;
        }
        if(_output_path_edit.text().isEmpty())
        {
			QMessageBox::critical(this, "Error", "The output path is empty.", QMessageBox::Ok);
			return;
        }

        if (gfx::image_file hdri = gfx::image_file(path, gfx::bits::b32, 3); hdri.bytes())
        {
            // Prepare the vector.
            QVector<int> vector(6);
            std::iota(vector.begin(), vector.end(), 0);

            // Create a progress dialog.
            QProgressDialog dialog;
            dialog.setLabelText(QString("Generating..."));

            // Create a QFutureWatcher and connect signals and slots.
            QFutureWatcher<void> futureWatcher;
            connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
            connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
            connect(&futureWatcher, &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
            connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged, &dialog, &QProgressDialog::setValue);

            std::filesystem::path dest = _output_path_edit.text().toStdString();
            dest.replace_extension("");

			if (_generate_subfolder_check->isChecked())
				dest /= path.filename();

            std::filesystem::path ppng = dest / "png";
            std::filesystem::path phdr = dest / "hdr";
            if (_generate_hdr_check->isChecked()) create_directories(phdr);
            if (_generate_png_check->isChecked()) create_directories(ppng);

           // gfx::ilog << "Successfully loaded " << path;
            const auto getter = [&](glm::vec3 dir) {
                dir                 = normalize(dir);
                const float angle_a = std::atan2(dir.x, dir.z) / (2.0f * glm::pi<float>());
                const float angle_b = 1 - (0.5f - asin(dir.y) / glm::pi<float>());

                const auto idx = (int(angle_b * hdri.height) + 10000 * hdri.height) % hdri.height * hdri.width
                                 + (int(angle_a * hdri.width) + 10000 * hdri.width) % hdri.width;
                return reinterpret_cast<glm::vec3*>(hdri.bytes())[idx];
            };
            const int resolution = _resolution_edit.text().toInt();

            futureWatcher.setFuture(QtConcurrent::map(vector, [&](int& side) {
				const auto             matrix = inverse(projection * matrices[side]);
				std::vector<glm::vec3> new_image(resolution * resolution);
				for (int y = 0; y < resolution; ++y)
				{
					for (int x = 0; x < resolution; ++x)
					{
						glm::vec4 dir =
							matrix
							* glm::vec4(static_cast<float>(x) / resolution * 2 - 1, static_cast<float>(y) / resolution * 2 - 1, 0, 1);
						new_image[y * resolution + x] = getter(dir / dir[3]);
					}
				}

				if (_generate_hdr_check->isChecked())
				{
					const auto target = phdr.string() + "/" + sides[side] + ".hdr";
					//gfx::ilog << "Writing " << target << " ...";
					gfx::image_file::save_hdr(target, resolution, resolution, 3, reinterpret_cast<float*>(new_image.data()));
				}

				if (_generate_png_check->isChecked())
				{
					std::vector<glm::u8vec3> png(resolution * resolution);
					for (int i = 0; i < png.size(); ++i)
						png[i] = static_cast<glm::u8vec3>(
							glm::clamp(glm::pow(new_image[i], glm::vec3(1.f / 1.75f)) * glm::vec3(255), glm::vec3(0), glm::vec3(255)));

					const auto target = ppng.string() + "/" + sides[side] + ".png";
					//gfx::ilog << "Writing " << target << " ...";
					gfx::image_file::save_png(target, resolution, resolution, 3, reinterpret_cast<const uint8_t*>(png.data()));
				}
			}));
            dialog.exec();

            futureWatcher.waitForFinished();
        }
    }

private:
    QWidget     _content;
    QVBoxLayout _content_layout;
    QLineEdit   _input_path_edit;
    QLineEdit   _output_path_edit;
    QLineEdit   _resolution_edit;
    QCheckBox*  _generate_subfolder_check;
    QCheckBox*  _generate_hdr_check;
    QCheckBox*  _generate_png_check;
    QFormLayout _paths_layout;
    QFormLayout _options_layout;
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

    ConverterWindow converter;
    converter.show();

    return app.exec();
}