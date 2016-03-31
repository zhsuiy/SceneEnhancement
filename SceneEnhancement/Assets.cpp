#include "Assets.h"
#include "Parameter.h"
//#include <iostream>

Assets *Assets::m_assets;

Assets::Assets()
{
	init();
}

void Assets::init()
{
	InitColorMap();
	InitMaterialMap();
}

QVector<FurnitureModel*> Assets::GetFurnitureModels()
{
	if (m_funitureModels.size() == 0)
	{
		m_funitureModels = Utility::ParseFurnitureModels(Parameter::GetParameterInstance()->SceneTemplatePath);
	}
	return m_funitureModels;
}

QVector<DecorationModel*> Assets::GetDecorationModels()
{
	if (m_decorationModels.size() == 0)
	{
		m_decorationModels = Utility::ParseDecorationModels(Parameter::GetParameterInstance()->DecorationModelsPath);
	}
	return m_decorationModels;
}

void Assets::InitColorMap()
{
	if (m_colors.size() == 0)
	{
		m_colors = Utility::ParseColorsFromFile(Parameter::GetParameterInstance()->ColorMapPath);
	}

}

void Assets::InitMaterialMap()
{
	if(MaterialMap.size() == 0)
	{
		MaterialMap = Utility::ParseMaterialMapFromFile(Parameter::GetParameterInstance()->MaterialMapPath);
	}
}

QVector3D& Assets::GetColorByName(QString& colorname)
{
	if (m_colors.contains(colorname))
	{
		return m_colors[colorname];
	}
	else
	{
		std::cout << "Invalid color: " <<  colorname.toStdString() << std::endl;
		return m_colors[m_colors.firstKey()];
	}
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

FurnitureModel* Assets::GetFurnitureModelByType(QString& type)
{
	if (type.compare("Floor",Qt::CaseInsensitive) == 0)
	{
		FurnitureModel *model = new FurnitureModel();		
		model->SetTranslation(QVector3D(RoomWidth / 2, 0, RoomDepth / 2));
		return model;
	}	

	for (size_t i = 0; i < m_funitureModels.size(); i++)
	{
		if (m_funitureModels[i]->Type.compare(type,Qt::CaseInsensitive) == 0)
		{
			return m_funitureModels[i];
		}
	}
	return nullptr;
}
