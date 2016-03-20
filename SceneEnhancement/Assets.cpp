#include "Assets.h"
#include "Parameter.h"
//#include <iostream>

Assets *Assets::m_assets;

Assets::Assets()
{
}

QVector<FurnitureModel*> Assets::GetFurnitureModels()
{
	if (m_funitureModels.size() == 0)
	{
		m_funitureModels = Utility::ParseFurnitureModels(Parameter::GetParameterInstance()->SceneTemplatePath);
	}
	return m_funitureModels;
}

Material* Assets::GetMaterial(const QString materialName)
{	
	if (m_materials.find(materialName) != m_materials.end()) // exist
	{
		return m_materials[materialName];
	}
	else
	{
		return nullptr;
	}	
}

void Assets::AddMaterial(QString materialName, Material* material)
{
	if (m_materials.find(materialName) != m_materials.end()) // already exist
	{
		std::cout << "Adding material Error: Material " << materialName.toStdString()
			<< " already exists.." << std::endl;
	}
	else
	{
		m_materials[materialName] = material;
	}
}
