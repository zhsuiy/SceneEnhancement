#pragma once

#include "Mesh.h"

class BoundingBox:public Mesh
{
public:
	BoundingBox(QVector3D leftBottomBack, QVector3D rightUpFront);
	~BoundingBox();
	float Width() {	return m_width;	};
	float Height() { return m_height; };
	float Depth() { return m_depth; };
	virtual void Draw(QOpenGLShaderProgram *program);

private:
	float m_width;
	float m_height;
	float m_depth;
	QVector3D m_left_bottom_back;
	QVector3D m_right_up_front;
	void setupData();
	void updateVertices();
	void updateTextures();
};



