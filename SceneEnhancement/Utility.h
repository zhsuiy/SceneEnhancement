#pragma once
#include <QtGui/QVector3D>
#include "FurnitureModel.h"
#include "Light.h"
#include "DecorationModel.h"

enum AdjType;

namespace Utility
{
	// useful converters
	QVector3D Str2Vec3D(QString &str);
	float QStr2Float(QString &str);
	float QStr2Int(QString &str);
	bool QStrCmp(QString &str1, char* str2);
	bool QStr2Bool(QString &str);
	bool QStrIsImagePath(QString &str);
	QVector<QString> QStr2StrVector(QString types);	

	QStringList GetFileNames(QString &path);

	/* file process */
	QVector<FurnitureModel*> ParseFurnitureModels(QString &path);
	QVector<DecorationModel*> ParseDecorationModels(QString &path);
	QVector<Light*> ParseLights();
	
	QMap<QString, QVector3D> ParseColorsFromFile(QString &path);
	QMap<FurnitureType, QVector<QString>> ParseMaterialMapFromFile(QString &path);
	

	/* path join */
	QString GetFurnitureModelPath(QString &type, QString &name);
	QString GetDecorationModelPath(QString &type, QString &name);
	QString GetDecorationModelPath(QString &type);
	

	Material* GetMaterialFromString(QString &material);
	Material* GetMaterialFromSingleTexture(QString path);
	Material* GetMaterialFromSingleColor(QVector3D &diffuse_color);

	//DecorationLocationType GetLocationTypeFromString(QString type);
	QVector<FurnitureLocationType> ParseLocationTypes(QString types);
	QVector<DecorationLocationType> ParseDecorationLocationTypes(QString types);
	
	// rectangle
	float GetCrossArea(QVector3D &rec1_v1, QVector3D &rec1_v2, QVector3D &rec2_v1, QVector3D &rec2_v2);

	// learning
	QMap<FurnitureType, QVector<QColor>> ReadImageFurnitureInfo(QString &path);
	QList<QPair<DecorationType, QPair<FurnitureType, QVector<DecorationLocationType>>>> ReadImageDecorationInfo(QString &path);


	
	
}
