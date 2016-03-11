#include "mainwindow.h"
#include "DisplayGLWidget.h"
#include "DisplaySceneGLWidget.h"
#include <QtWidgets/QHBoxLayout>
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(1400,800);	
	centralWidget = new QWidget;
	setCentralWidget(centralWidget);


	QHBoxLayout *main_layout = new QHBoxLayout;
	//DisplayGLWidget *displaySceneWidget = new DisplayGLWidget();
	DisplaySceneGLWidget *displaySceneWidget = new DisplaySceneGLWidget();
	main_layout->addWidget(displaySceneWidget);
	//setLayout(main_layout);
	setWindowTitle("test triangle");
	centralWidget->setLayout(main_layout);

	displaySceneWidget->setFocusPolicy(Qt::StrongFocus);


	

	//ui.setupUi(this);
}

MainWindow::~MainWindow()
{

}
