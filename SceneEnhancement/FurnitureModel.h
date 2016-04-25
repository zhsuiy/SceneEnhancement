#ifndef FURNITURE_H
#define FURNITURE_H

#include "Model.h"
#include "SupportRegion.h"

class ColorPalette;
class DecorationModel;

#define FurnitureType QString 
#define FurnitureName QString

enum FurnitureLocationType
{	
	FTBottom,
	FTUp,
	FTLeft,
	FTRight,
	FTFront,
	FTBack
};

enum FurnitureFrontDirection // 前方向
{
	Invalid,
	XPos,
	XNeg,
	ZPos,
	ZNeg
};

class FurnitureModel : public Model
{
public:
	FurnitureModel();
	FurnitureModel(FurnitureType type, FurnitureName name,QVector3D translate,
		QVector3D rotate, QVector<FurnitureLocationType> locationTypes, float scale);
	FurnitureType Type;
	QVector<FurnitureLocationType> LocationTypes;
	QVector3D GetRelativePosition(DecorationModel* model);
	void UpdateMeshMaterials();
	void UpdateMeshMaterials(ColorPalette *color_palette);
	void UpdateDecorationLayout();
	void ClearDecorationLayout();
	void AddDecorationModel(DecorationModel *model);
protected:
	QVector<Material*> ordered_materials;
	void OrderMaterialByMeshArea();

	void updateTranslation(); // 根据BoundingBox调整位置
	void SetModelMatrix();

	QVector<DecorationModel*> decoration_models;
	FurnitureFrontDirection furniture_front_direction;
	void updateFrontDirection(QVector3D &rotate);

	// detect support region
	QVector<SupportRegion*> support_regions;
	void DetectSupportRegions();
	void AdaptDecorationLocationType(DecorationModel *model) const;
	void AdaptTranslateAccord2FrontDirection(float &tx, float &tz);
	
private:
	QVector3D& getTranslate(float x, float y, float z);
	
	
	
	
	

};

#endif
