#pragma once
#include <QtGui/QVector3D>
#include "FurnitureModel.h"
#include "Light.h"

namespace Utility
{
	QVector3D Str2Vec3D(QString &str);
	QVector<FurnitureModel*> ParseFurnitureModels(QString &path);
	float QStr2Float(QString &str);
	bool QStrCmp(QString &str1, char* str2);

	QString GetModelPath(QString &type, QString &name);
	QVector<Light*> ParseLights();
	QVector<QString> ParseFurnitureTypes(QString types);
	

	
}
