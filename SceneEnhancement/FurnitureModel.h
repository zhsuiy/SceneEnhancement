#ifndef FURNITURE_H
#define FURNITURE_H

#include "Model.h"

class DecorationModel;

#define FurnitureType QString 
#define FurnitureName QString

class FurnitureModel : public Model
{
public:
	FurnitureModel(FurnitureType type, FurnitureName name,QVector3D translate,
		QVector3D rotate, float scale);
	FurnitureType Type;

	QVector3D GetRelativePosition(DecorationModel* model);

	

private:

};

#endif
