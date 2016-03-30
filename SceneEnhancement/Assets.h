#pragma once
#include <QtCore/qmap.h>
#include "Material.h"
#include "Utility.h"
#include "FurnitureModel.h"
#include <iostream>
#include "DecorationModel.h"

class Assets
{
public:
	static Assets* GetAssetsInstance()
	{
		if (!m_assets)
		{
			m_assets = new Assets();
		}
		return m_assets;
	}
	float RoomWidth;
	float RoomHeight;
	float RoomDepth;
	QVector<FurnitureModel*> GetFurnitureModels();
	QVector<DecorationModel*> GetDecorationModels();	
	void InitColorMap();
	QVector3D& GetColorByName(QString &colorname);
	Material* GetMaterial(const QString materialName);	
	void AddMaterial(QString materialName, Material* material);
	QVector3D WallColor;
	QString FloorTexture;
	FurnitureModel* GetFurnitureModelByType(QString &type);
	
private:
	QMap<QString, Material*> m_materials;
	QVector<FurnitureModel*> m_funitureModels;
	QVector<DecorationModel*> m_decorationModels;
	QMap<QString, QVector3D> m_colors;
	Assets();	
	void init();
	static Assets *m_assets;
};

