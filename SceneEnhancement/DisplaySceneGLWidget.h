#pragma once

#include <QGLWidget>

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include "Camera.h"
#include "Model.h"
#include "BoundingBox.h"

class DisplaySceneGLWidget :public QGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	DisplaySceneGLWidget(QWidget *parent = 0);
	~DisplaySceneGLWidget();

	void teardownGL();
	void keyPressEvent(QKeyEvent *event);
	public slots:
	//void CompileAndLinkVertexShader(const QString& shaderText);
	//void CompileAndLinkFragmentShader(const QString& shaderText);

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);


	//void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	// wheel event
	void wheelEvent(QWheelEvent *event);


private:
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ebo;
	QOpenGLShaderProgram *m_program;

	QOpenGLVertexArrayObject light_vao;
	QOpenGLShaderProgram *light_program;


	QMatrix4x4 modelMatrix;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 projection;
	QMatrix4x4 model2world;

	Qt::MouseButton mouseButton;
	QVector3D getArcBallVector(int x, int y);
	QVector2D mouseCurPos, mouseLastPos;

	QVector2D mousePressPosition;
	

	// zoom 
	Camera *camera;
	int scrollDelta = 0;

	// lights
	void initLights();	
	void paintLight();

	Model *model;
	void printVersionInformation();

	BoundingBox *bb;





};




