#pragma once

#include "Model.h"
#include "FurnitureModel.h"

#define DecorationType QString
#define DecorationName QString
#define DecorationLocType QString

enum DecorationLocationType
{
	NotSet,
	Center,
	Left,
	Right,
	Back,
	Front
};

class DecorationModel : public Model
{
public:
	DecorationModel(FurnitureType furnitureType, DecorationType decType, DecorationLocType locType, float scale = 1.0f);
	virtual void Draw(QOpenGLShaderProgram *program);
	DecorationLocationType LocationType;
private:
	FurnitureType support_model_type;
	FurnitureModel* m_support_model;
	QVector3D m_relative_translate;

};


