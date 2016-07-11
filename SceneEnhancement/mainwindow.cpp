#include "mainwindow.h"
#include "DisplayGLWidget.h"
#include "DisplaySceneGLWidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenuBar>
#include "ProbLearning.h"
#include "floatingwidget.h"
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(1400,900);
	
	// widgets
	centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	main_layout = new QHBoxLayout;

	ProbLearning *problearner = new ProbLearning();
	MenuLearn = menuBar()->addMenu(tr("Learning"));
	QAction *actionTrainF1 = MenuLearn->addAction(tr("TrainF1"));
//	connect(actionTrainF1, &QAction::triggered, problearner, SLOT(&ProbLearning::Learn,F1));
	connect(actionTrainF1, &QAction::triggered, problearner, [problearner]
	{
		problearner->Learn(F1);
	});
	QAction *actionTrainF2 = MenuLearn->addAction(tr("TrainF2"));
	connect(actionTrainF2, &QAction::triggered, problearner, [problearner]
	{
		problearner->Learn(F2);
	});
	QAction *actionTrainF1F2 = MenuLearn->addAction(tr("TrainF1F2"));
	connect(actionTrainF1F2, &QAction::triggered, problearner, [problearner]
	{
		problearner->Learn(F1F2);
	});


	//DisplayGLWidget *displaySceneWidget = new DisplayGLWidget();
	displaySceneWidget = new DisplaySceneGLWidget(problearner);
	displaySceneWidget->setFixedWidth(1400);
	displaySceneWidget->setFixedHeight(800);
	//FloatingWidget * fw_widget = new FloatingWidget;
	//main_layout->addWidget(fw_widget);
	//QWidget *left_control_widget = new QWidget;
	//QVBoxLayout *left_control_layout = new QVBoxLayout;
	
	// button initialize
	//ButtonUpdateMaterial = new QPushButton(tr("Update Material"));
	//connect(ButtonUpdateMaterial, SIGNAL(clicked()), this, SLOT(OnButtonUpdateMaterialClicked()));	
	//ButtonCancel = new QPushButton(tr("Cancel"));
	//connect(ButtonCancel, SIGNAL(clicked()), this, SLOT(OnButtonCancelClicked()));
	
	// add button to left layout
	//left_control_layout->addWidget(ButtonUpdateMaterial);
	//left_control_layout->addWidget(ButtonCancel);
	
	// set left layout to left widget
	//left_control_widget->setLayout(left_control_layout);

	//main_layout->addWidget(left_control_widget);
	//main_layout->addWidget(displaySceneWidget);
	//main_layout->setStretchFactor(left_control_widget, 1);
	//main_layout->setStretchFactor(displaySceneWidget, 7);
		
	setWindowTitle("Scene Enhancement");
	centralWidget->setLayout(main_layout);

	// menu
	MenuScene = menuBar()->addMenu(tr("Scene"));
	QAction *actionDisplay= MenuScene->addAction(tr("Display"));
	connect(actionDisplay, &QAction::triggered, this, &MainWindow::OnDisplayScene);

	MenuUpdate = menuBar()->addMenu(tr("Update"));	
	QAction *actionUpdateConfig = MenuUpdate->addAction(tr("Config  (Ctrl+U)"));
	connect(actionUpdateConfig, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateConfig);
	QAction *actionUpdateMaterial = MenuUpdate->addAction(tr("Material  (Ctrl+M)"));
	connect(actionUpdateMaterial, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateMaterials);
	QAction *actionUpdateDecoration = MenuUpdate->addAction(tr("Decorations  (Ctrl+D)"));
	connect(actionUpdateDecoration, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateDecorations);
	QAction *actionUpdateMaterialByLearner = MenuUpdate->addAction(tr("Material By Learner"));
	connect(actionUpdateMaterialByLearner, &QAction::triggered, displaySceneWidget, &DisplaySceneGLWidget::UpdateMaterialsByLearner);


	displaySceneWidget->setFocusPolicy(Qt::StrongFocus);
	
	//

	QToolBar *toolbar = new QToolBar("toolBar", this);	
	toolbar->addAction(QIcon("./Resources/icon/texture.png"), "toggle texture (T)", displaySceneWidget, &DisplaySceneGLWidget::ToggleTexture);
	toolbar->addAction(QIcon("./Resources/icon/decoration.png"), "toggle display decorations", displaySceneWidget, &DisplaySceneGLWidget::ToggleDisplayDecorations);
	toolbar->addAction(QIcon("./Resources/icon/F1.png"),"train color using unary term", problearner, [problearner]
	{
		problearner->Learn(F1);
	});
	toolbar->addAction(QIcon("./Resources/icon/F2.png"), "train color using binary term", problearner, [problearner]
	{
		problearner->Learn(F2);
	});
	toolbar->addAction(QIcon("./Resources/icon/F1F2.png"), "train color using both unary and binary terms",
		problearner, [problearner]
	{
		problearner->Learn(F1F2);
	});
	toolbar->addAction(QIcon("./Resources/icon/MIF1.png"), "train color with unary term using MI", problearner, [problearner]
	{
		problearner->LearnMI();
	});
	toolbar->addAction(QIcon("./Resources/icon/prevelance.png"),"Use prevalence",problearner,[problearner]
	{
		problearner->LearnPU(Prevalence);
	});
	toolbar->addAction(QIcon("./Resources/icon/uniqueness.png"), "Use Uniqueness", problearner, [problearner]
	{
		problearner->LearnPU(Uniqueness);
	});
	toolbar->addAction(QIcon("./Resources/icon/pu.png"), "Use both prevalence and uniqueness", problearner, [problearner]
	{
		problearner->LearnPU(PU);
	});
	toolbar->addAction(QIcon("./Resources/icon/cluster.png"),"save cluster result",problearner,[problearner]
	{
		problearner->SaveFurnitureClusterResult();
	});
	

	addToolBar(toolbar);
	

	



	//ui.setupUi(this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::OnDisplayScene()
{
	
	main_layout->addWidget(displaySceneWidget); 
	
}

void MainWindow::OnButtonUpdateMaterialClicked()
{
}

void MainWindow::OnButtonCancelClicked()
{
}

