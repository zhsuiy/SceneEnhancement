#pragma once

#include <QOpenGLShaderProgram>
#include "Vertex.h"
#include "Texture.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include "Mesh.h"

class BoundingBox:public Mesh
{
public:
	BoundingBox(QVector3D leftBottomBack, QVector3D rightUpFront);
	~BoundingBox();

private:
	// attributes
	float m_width;
	float m_height;
	QVector3D m_left_bottom_back;
	QVector3D m_right_up_front;
	
	
	// functions
	void setupData();
	void updateVertices();
	void updateTextures();

	
	
	

};
