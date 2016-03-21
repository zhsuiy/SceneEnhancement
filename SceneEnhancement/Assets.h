#pragma once
#include <QtCore/qmap.h>
#include "Material.h"
#include "Utility.h"
#include "FurnitureModel.h"
#include <iostream>

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
	
	QVector<FurnitureModel*> GetFurnitureModels();
	Material* GetMaterial(const QString materialName);	
	void AddMaterial(QString materialName, Material* material);
	QVector3D WallColor;
	QString FloorTexture;
	
private:
	QMap<QString, Material*> m_materials;
	QVector<FurnitureModel*> m_funitureModels;
	Assets();	
	static Assets *m_assets;
};

