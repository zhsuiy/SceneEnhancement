#pragma once

#include "Model.h"

class WallFloorModel:public Model
{
public:
	WallFloorModel(QVector3D leftBottomBack, QVector3D rightUpFront);
	~WallFloorModel();

private:
	// attributes
	//float m_width;
	//float m_height;
	QVector3D m_left_bottom_back;
	QVector3D m_right_up_front;
};
