#ifndef FURNITURE_H
#define FURNITURE_H

#include "Model.h"

class DecorationModel;

#define FurnitureType QString 
#define FurnitureName QString

enum FurnitureLocationType
{	
	FTBottom,
	FTUp,
	FTLeft,
	FTRight,
	FTFront,
	FTBack
};

class FurnitureModel : public Model
{
public:
	FurnitureModel();
	FurnitureModel(FurnitureType type, FurnitureName name,QVector3D translate,
		QVector3D rotate, QVector<FurnitureLocationType> locationTypes, float scale);
	FurnitureType Type;
	QVector<FurnitureLocationType> LocationTypes;
	QVector3D GetRelativePosition(DecorationModel* model);
	
	

private:
	void updateTranslation(); // 根据BoundingBox调整位置

};

#endif
