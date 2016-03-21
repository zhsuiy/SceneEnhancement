#include "WallFloorModel.h"
#include "WallFloorMesh.h"
#include "Utility.h"
#include "Global.h"
#include "Assets.h"

WallFloorModel::WallFloorModel( QVector3D leftBottomBack, QVector3D rightUpFront):Model()
{	
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;
	QVector3D wallColor = Assets::GetAssetsInstance()->WallColor;
	QString floorTexture = Assets::GetAssetsInstance()->FloorTexture;
	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Ceiling,
		Utility::GetMaterialFromSingleColor(wallColor)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, BackWall, 
		Utility::GetMaterialFromSingleColor(wallColor)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, LeftWall,
		Utility::GetMaterialFromSingleColor(wallColor)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, RightWall,
		Utility::GetMaterialFromSingleColor(wallColor)));

	this->meshes.push_back(new WallFloorMesh(leftBottomBack, rightUpFront, Floor,
		Utility::GetMaterialFromSingleTexture(floorTexture)));

	// save materials???	
}

WallFloorModel::~WallFloorModel()
{
}