#include "FurnitureModel.h"
#include "Global.h"
#include "Parameter.h"

FurnitureModel::FurnitureModel(QString type, QString name, QVector3D translate, QVector3D rotate, float scale)
:Model(Utility::GetFurnitureModelPath(type,name),translate,rotate,scale)
{	
	this->Type = type;
}
