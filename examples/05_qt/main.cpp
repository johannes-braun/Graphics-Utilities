#if __has_include(<QApplication>)

#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWindow>
#include <QLabel>
#include <QGridLayout>
#include <gfx/gfx.hpp>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>

class MyButton : public QMainWindow
{
public:
    MyButton(QWidget* parent = 0);

private slots:
    void on_quit() { gfx::ilog << "Quat."; }
};

MyButton::MyButton(QWidget* parent) : QMainWindow(parent)
{
	QWidget* content = new QWidget;
	QGridLayout *grid = new QGridLayout;
	content->setLayout(grid);
	setCentralWidget(content);

	QPushButton *plsBtn = new QPushButton("One", this);
	QPushButton *minBtn = new QPushButton("Two", this);
	QLabel* lbl = new QLabel("0", this);
	grid->addWidget(plsBtn, 0, 0);
	grid->addWidget(minBtn, 0, 1);
	grid->addWidget(lbl, 1, 1);

    connect(plsBtn, &QPushButton::clicked, this, [=]()
    {
		lbl->setText("Plussed!");
    });
	connect(minBtn, &QPushButton::clicked, this, [=]()
	{
		lbl->setText("Minussed!");
	});

	QAction *quit = new QAction("&Quit", this);

	QMenu *file;
	file = menuBar()->addMenu("&File");
	file->addAction(quit);

	connect(quit, &QAction::triggered, qApp, QApplication::quit);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MyButton quit;
    quit.resize(250, 150);
    quit.setWindowTitle("QPushButton");

    quit.show();
    return a.exec();
}

#else
int main() {}
#endif