#include "WallFloorModel.h"
#include "WallFloorMesh.h"
#include "Utility.h"
#include "Global.h"

WallFloorModel::WallFloorModel( QVector3D leftBottomBack, QVector3D rightUpFront):Model()
{	
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Ceiling,
		Utility::GetMaterialFromSingleTexture(G_FloorTexturePath)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, BackWall, 
		Utility::GetMaterialFromSingleTexture(G_FloorTexturePath)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, LeftWall,
		Utility::GetMaterialFromSingleTexture(G_FloorTexturePath)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, RightWall,
		Utility::GetMaterialFromSingleTexture(G_FloorTexturePath)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Floor,
		Utility::GetMaterialFromSingleTexture(G_FloorTexturePath)));

	// save materials ???	
}

WallFloorModel::~WallFloorModel()
{
}