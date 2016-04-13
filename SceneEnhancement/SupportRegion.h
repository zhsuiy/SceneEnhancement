#pragma once
#include <QtCore/QVector>

class DecorationModel;
class QMatrix4x4;


class SupportRegion
{
public:
	SupportRegion();
	SupportRegion(float min_x, float max_x, float min_z, float max_z, float height, QMatrix4x4 modelMatrix);
	bool IsSpaceEnough() const;	
	bool TryPutDecorationModel(DecorationModel *model);
	void Clear();
	float MinX;
	float MaxX;
	float MinZ;
	float MaxZ;
	float Height;
	float Width;
	float Depth;	
	int ModelNum;
private:
	QVector<DecorationModel *> m_decoration_models;
	void updateRemainingArea();
	
	float m_area;
	float m_empty_area;


};
