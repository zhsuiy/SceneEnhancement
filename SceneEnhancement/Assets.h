#pragma once
#include <QtCore/qmap.h>
#include "Material.h"

class Assets
{
public:
	Material* GetMaterial(const QString materialName);
	void AddMaterial(QString materialName, Material* material);
private:
	QMap<QString, Material*> m_materials;
};

