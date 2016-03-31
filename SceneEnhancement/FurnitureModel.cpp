#include "FurnitureModel.h"
#include "Global.h"
#include "Parameter.h"

FurnitureModel::FurnitureModel()
{
}

FurnitureModel::FurnitureModel(QString type, QString name, QVector3D translate, QVector3D rotate,
							   QVector<FurnitureLocationType> locationTypes, float scale)
				:Model(Utility::GetFurnitureModelPath(type,name),translate,rotate,scale)
{	
	this->Type = type;
	this->LocationTypes = locationTypes;
	updateTranslation();

	this->OrderMaterialByMeshArea();
	this->UpdateMeshMaterials();
}

QVector3D FurnitureModel::GetRelativePosition(DecorationModel* model)
{
	QVector3D translate;




	return translate;
}

void FurnitureModel::updateTranslation()
{
	Assets *assets = Assets::GetAssetsInstance();
	for (size_t i = 0; i < LocationTypes.size(); i++)
	{		
		switch (LocationTypes[i])
		{
		case FTBottom:
			this->m_translate.setY(abs(boundingBox->LeftBottomBack().y())* m_scale + 0.01f);
			break;
		case FTUp:
			this->m_translate.setY(assets->RoomHeight - abs(boundingBox->RightUpFront().y()) * m_scale - 0.01f);
			break;
		case FTLeft:
			this->m_translate.setX(abs(boundingBox->LeftBottomBack().x()) * m_scale + 0.01f);
			break;
		case FTRight:
			this->m_translate.setX(assets->RoomWidth - abs(boundingBox->LeftBottomBack().x())* m_scale - 0.01f);
			break;
		case FTFront:
			this->m_translate.setZ(assets->RoomDepth - abs(boundingBox->RightUpFront().x())  * m_scale - 0.01f);
			break;
		case FTBack:
			this->m_translate.setZ(abs(boundingBox->RightUpFront().x()) * m_scale + 0.01f);
			break;
		default:
			break;
		}
	}	
}

int compare(const QPair<QString, float> &a1, const QPair<QString, float> &a2)
{
	return a1.second > a2.second;
}

void FurnitureModel::OrderMaterialByMeshArea()
{
	QMap<QString, float> tmp;
	QVector<QPair<QString, float>> materials;

	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		QString materialName = this->meshes[i]->MeshMaterial->Name;
		tmp[materialName] += this->meshes[i]->GetArea();
	}

	QMapIterator<QString, float> it(tmp);
	while (it.hasNext())
	{
		it.next();
		materials.push_back(QPair<QString, float>(it.key(), it.value()));
	}

	std::sort(materials.begin(), materials.end(), compare);
	for (size_t i = 0; i < materials.size(); i++)
	{
		ordered_materials.push_back(material_assets[materials[i].first]);
	}
}

void FurnitureModel::UpdateMeshMaterials()
{
	Assets *assets = Assets::GetAssetsInstance();
	if (assets->MaterialMap.contains(Type))
	{
		QVector<QString> colorNames = assets->MaterialMap[Type];
		int n = colorNames.size();
		for (size_t i = 0; i < this->ordered_materials.size(); i++)
		{
			ordered_materials[i]->Diffuse->UseMap = false;
			ordered_materials[i]->Diffuse->Color = assets->GetColorByName(colorNames[i%n]);
		}
	}
}
