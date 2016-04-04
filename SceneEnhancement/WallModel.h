#pragma once


#include "FurnitureModel.h"

class WallModel:public FurnitureModel
{
public:
	WallModel(QVector3D leftBottomBack, QVector3D rightUpFront);
	~WallModel();
	//void UpdateMaterials();

private:
	// attributes
	//float m_width;
	//float m_height;
	QVector3D m_left_bottom_back;
	QVector3D m_right_up_front;
};
