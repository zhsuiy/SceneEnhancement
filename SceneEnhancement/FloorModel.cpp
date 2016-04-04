#include "FloorModel.h"
#include "Assets.h"
#include "WallFloorMesh.h"

FloorModel::FloorModel(QVector3D leftBottomBack, QVector3D rightUpFront):FurnitureModel()
{
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;
	
	Material *material = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["Floor"][0]);
	material->Name = "FloorMaterial";
	material_assets["FloorMaterial"] = material;
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Floor,material));
	this->Type = "Floor";	
	init();
	this->OrderMaterialByMeshArea();
	this->UpdateMeshMaterials();
}

FloorModel::~FloorModel()
{
}
