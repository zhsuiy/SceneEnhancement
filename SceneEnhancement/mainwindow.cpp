#include "mainwindow.h"
#include "DisplayGLWidget.h"
#include "DisplaySceneGLWidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenuBar>
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(1400,800);
	
	// widgets
	centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	QHBoxLayout *main_layout = new QHBoxLayout;

	//DisplayGLWidget *displaySceneWidget = new DisplayGLWidget();
	DisplaySceneGLWidget *displaySceneWidget = new DisplaySceneGLWidget();
	
	QWidget *left_control_widget = new QWidget;
	QVBoxLayout *left_control_layout = new QVBoxLayout;
	
	// button initialize
	ButtonUpdateMaterial = new QPushButton(tr("Update Material"));
	connect(ButtonUpdateMaterial, SIGNAL(clicked()), this, SLOT(OnButtonUpdateMaterialClicked()));	
	ButtonCancel = new QPushButton(tr("Cancel"));
	connect(ButtonCancel, SIGNAL(clicked()), this, SLOT(OnButtonCancelClicked()));
	
	// add button to left layout
	left_control_layout->addWidget(ButtonUpdateMaterial);
	left_control_layout->addWidget(ButtonCancel);
	
	// set left layout to left widget
	left_control_widget->setLayout(left_control_layout);

	//main_layout->addWidget(left_control_widget);
	main_layout->addWidget(displaySceneWidget);
	//main_layout->setStretchFactor(left_control_widget, 1);
	//main_layout->setStretchFactor(displaySceneWidget, 7);
		
	setWindowTitle("Scene Enhancement");
	centralWidget->setLayout(main_layout);

	// menu
	MenuUpdate = menuBar()->addMenu(tr("Update"));
	QAction *actionUpdateConfig = MenuUpdate->addAction(tr("Config"));
	connect(actionUpdateConfig, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateConfig);
	QAction *actionUpdateMaterial = MenuUpdate->addAction(tr("Material"));
	connect(actionUpdateMaterial, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateMaterials);
	QAction *actionUpdateDecoration = MenuUpdate->addAction(tr("Decorations"));
	connect(actionUpdateDecoration, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateDecorations);




	displaySceneWidget->setFocusPolicy(Qt::StrongFocus);
	




	//ui.setupUi(this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::OnButtonUpdateMaterialClicked()
{
}

void MainWindow::OnButtonCancelClicked()
{
}
//
//void MainWindow::ActionUpdateMaterialTriggered()
//{
//	int a = 1;
//}
//
//void MainWindow::ActionUpdateDecorationTriggered()
//{
//
//}
