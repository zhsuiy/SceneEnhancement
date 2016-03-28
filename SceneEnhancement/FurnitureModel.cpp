#include "FurnitureModel.h"
#include "Global.h"
#include "Parameter.h"

FurnitureModel::FurnitureModel(QString type, QString name, QVector3D translate, QVector3D rotate,
							   QVector<FurnitureLocationType> locationTypes, float scale)
				:Model(Utility::GetFurnitureModelPath(type,name),translate,rotate,scale)
{	
	this->Type = type;
	this->LocationTypes = locationTypes;
	updateTranslation();
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