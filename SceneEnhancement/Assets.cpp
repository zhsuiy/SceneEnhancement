#include "Assets.h"
#include "Parameter.h"
#include "WallModel.h"
#include "FloorModel.h"
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
	InitTextureColors();
	InitDecorationZOrders();
	InitDecorationMedialOrders();
	InitDecorationScales();
}

QVector<FurnitureModel*> Assets::GetFurnitureModels()
{
	if (m_funitureModels.size() == 0)
	{
		m_funitureModels = Utility::ParseFurnitureModels(Parameter::GetParameterInstance()->SceneTemplatePath);
		WallModel *wall_model = new WallModel(QVector3D(-m_assets->RoomWidth / 2, -m_assets->RoomHeight / 2, -m_assets->RoomDepth / 2), QVector3D(m_assets->RoomWidth / 2, m_assets->RoomHeight / 2, m_assets->RoomDepth / 2));
		FloorModel *floor_model = new FloorModel(QVector3D(-m_assets->RoomWidth / 2, -m_assets->RoomHeight / 2, -m_assets->RoomDepth / 2), QVector3D(m_assets->RoomWidth / 2, m_assets->RoomHeight / 2, m_assets->RoomDepth / 2));
		m_funitureModels.push_back(wall_model);
		m_funitureModels.push_back(floor_model);
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

QVector<DecorationModel*> Assets::GetUpdatedDecorationModels()
{
	m_decorationModels = Utility::ParseDecorationModels(Parameter::GetParameterInstance()->DecorationModelsPath);
	return m_decorationModels;
}

QMap<QString, QVector3D> Assets::GetColors()
{
	return m_colors;
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

void Assets::InitTextureColors()
{
	if (FurnitureTextureColors.size() == 0)
	{
		FurnitureTextureColors = Utility::ParseFurnitureTextureColors(Parameter::GetParameterInstance()->TexturePath);
	}
}

void Assets::InitDecorationZOrders()
{
	if (DecorationZOrders.size() == 0)
	{
		DecorationZOrders = Utility::ParseQStrNameAndFloatValue(Parameter::GetParameterInstance()->DecorationZOrdersPath);
	}
}

void Assets::InitDecorationMedialOrders()
{
	if (DecorationMedialOrders.size() == 0)
	{
		DecorationMedialOrders = Utility::ParseQStrNameAndFloatValue(Parameter::GetParameterInstance()->DecorationMedialOrderPath);
	}
}

void Assets::InitDecorationScales()
{
	if (DecorationScales.size() == 0)
	{
		DecorationScales = Utility::ParseQStrNameAndFloatValue(Parameter::GetParameterInstance()->DecorationScalePath);
	}
}

void Assets::UpdateMaterialMap()
{
	MaterialMap.clear();
	MaterialMap = Utility::ParseMaterialMapFromFile(Parameter::GetParameterInstance()->MaterialMapPath);
}

Texture* Assets::GetTexture(QString& path)
{
	if (!QFile::exists(path))
	{
		qWarning("%s does not exist.", path.toStdString().c_str());
		return nullptr;
	}
	// if not exist, add to assets
	if (!m_textures.contains(path))
	{
		Texture *texture = new Texture();
		QOpenGLTexture *opgltexture;
		opgltexture = new QOpenGLTexture(QImage(path).mirrored());
		//texture->setAutoMipMapGenerationEnabled(true);
		opgltexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		opgltexture->setMagnificationFilter(QOpenGLTexture::Linear);
		opgltexture->setWrapMode(QOpenGLTexture::Repeat);
		texture->texture = opgltexture;
		texture->type = DiffuseTexture;
		m_textures[path] = texture;
	}	
	return m_textures[path];
}

DecorationModel* Assets::GetDecorationModel(DecorationType &decorationtype)
{
	if (!m_decorations.contains(decorationtype))
	{
		// read all decorations of the same category
		QList<DecorationModel*> models = Utility::ParseDecorationModelsByType(decorationtype);
		m_decorations[decorationtype] = models;
	}
	
	// 返回第一个Decoration IsAssigned为false的model
	for (size_t i = 0; i < m_decorations[decorationtype].size(); i++)
	{
		if (m_decorations[decorationtype][i]->IsAssigned == false)
		{			
			return m_decorations[decorationtype][i];
		}
	}

	// 当前所有的都被占用，则扩展当前的model库
	QList<DecorationModel*> models = Utility::ParseDecorationModelsByType(decorationtype);
	for (size_t i = 0; i < models.size(); i++)
	{
		m_decorations[decorationtype].push_back(models[i]);
	}	
	if (models.size() > 0)	
		return models[0]; // 返回新的model的第一个	
	else
		return nullptr;
	 
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
	/*if (type.compare("Floor",Qt::CaseInsensitive) == 0)
	{
		FurnitureModel *model = new FurnitureModel();		
		model->SetTranslation(QVector3D(RoomWidth / 2, 0, RoomDepth / 2));
		return model;
	}	*/

	for (size_t i = 0; i < m_funitureModels.size(); i++)
	{
		if (m_funitureModels[i]->Type.compare(type,Qt::CaseInsensitive) == 0)
		{
			return m_funitureModels[i];
		}
	}
	// only for test, for those who does not have support model, set it to floor model
	//return m_funitureModels.last(); 
	return nullptr;
}