#include "WallModel.h"
#include "WallFloorMesh.h"
#include "Utility.h"
#include "Global.h"
#include "Assets.h"

WallModel::WallModel( QVector3D leftBottomBack, QVector3D rightUpFront):FurnitureModel()
{	
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;
	
	Material *material1 = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["Wall"][0]);
	material1->Name = "Wall1";
	material_assets["Wall1"] = material1;
	Material *material2 = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["Wall"][1]);
	material2->Name = "Wall2";
	material_assets["Wall2"] = material2;
	
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Ceiling, material1));
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, BackWall, material1));
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, LeftWall, material2));
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, RightWall, material1));

	this->Type = "Wall";
	init();
	
	this->OrderMaterialByMeshArea();
	this->UpdateMeshMaterials();
	
	// save materials???	
}

WallModel::~WallModel()
{
}

//void WallModel::UpdateMaterials()
//{
//	this->meshes[0]->MeshMaterial = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["Ceiling"][0]);
//	
//	this->meshes[1]->MeshMaterial =	Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["BackWall"][0]);
//
//	this->meshes[2]->MeshMaterial = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["LeftWall"][0]);
//
//	this->meshes[3]->MeshMaterial = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["RightWall"][0]);
//
//	this->meshes[4]->MeshMaterial = Utility::GetMaterialFromString(Assets::GetAssetsInstance()->MaterialMap["Floor"][0]);
//}
