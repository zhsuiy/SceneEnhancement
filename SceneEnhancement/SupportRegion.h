#pragma once

class SupportRegion
{
public:
	SupportRegion()	{};
	SupportRegion(float min_x, float max_x, float min_z, float max_z, float height, QMatrix4x4 modelMatrix)		
	{
		QVector3D tmplb(min_x, height, min_z);
		QVector3D tmprf(max_x, height, max_z);
		QVector3D worldLb = modelMatrix * tmplb;
		QVector3D worldRf = modelMatrix * tmprf;

		this->MinX = qMin(worldLb.x(), worldRf.x());
		this->MaxX = qMax(worldLb.x(), worldRf.x());
		this->MinZ = qMin(worldLb.z(), worldRf.z());
		this->MaxZ = qMax(worldLb.z(), worldRf.z());
		this->Height = worldLb.y();		
		this->Width = MaxX - MinX;
		this->Depth = MaxZ - MinZ;
	}
	float MinX;
	float MaxX;
	float MinZ;
	float MaxZ;
	float Height;
	float Width;
	float Depth;


};