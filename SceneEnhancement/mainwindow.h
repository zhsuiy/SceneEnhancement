#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>


class MainWindow : public QMainWindow
{
	

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Q_OBJECT
	QWidget *centralWidget;

	QMenu *MenuUpdate;

	

	QPushButton *ButtonUpdateMaterial;
	QPushButton *ButtonCancel;
	private slots:
	void OnButtonUpdateMaterialClicked();
	void OnButtonCancelClicked();
	//void ActionUpdateMaterialTriggered();
	//void ActionUpdateDecorationTriggered();

	
};

#endif // DISPLAYSCENE_H
