#include "SupportRegion.h"
#include <QtCore/qglobal.h>
#include <QtGui/QVector3D>
#include "DecorationModel.h"
#include "Utility.h"
SupportRegion::SupportRegion()
{
}

SupportRegion::SupportRegion(float min_x, float max_x, float min_z, float max_z, float height, QMatrix4x4 modelMatrix)
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
	this->ModelNum = 0;
	this->m_area = Width * Depth;
	this->m_empty_area = m_area;
}

bool SupportRegion::IsSpaceEnough() const
{
	return m_empty_area > 0.35*m_area;	
}

bool SupportRegion::TryPutDecorationModel(DecorationModel* model)
{
	float cross_area = 0;
	for (size_t i = 0; i < m_decoration_models.size(); i++)
	{
		cross_area += Utility::GetCrossArea(m_decoration_models[i]->boundingBox->WorldLeftBottomBack(), m_decoration_models[i]->boundingBox->WorldRightUpFront(),
			model->boundingBox->WorldLeftBottomBack(), model->boundingBox->WorldRightUpFront());
	}
	if (cross_area == 0)
	{
		m_decoration_models.push_back(model);
		ModelNum++;
		updateRemainingArea();
		return true;
	}
	else
	{
		return false;
	}

	
}

void SupportRegion::Clear()
{
	this->m_decoration_models.clear();
	this->m_empty_area = m_area;
	ModelNum = 0;
}

void SupportRegion::updateRemainingArea()
{
	float occupied_area = 0;
	for (size_t i = 0; i < m_decoration_models.size(); i++)
	{
		occupied_area += Utility::GetCrossArea(m_decoration_models[i]->boundingBox->WorldLeftBottomBack(), m_decoration_models[i]->boundingBox->WorldRightUpFront(),
			QVector3D(MinX, Height, MinZ), QVector3D(MaxX, Height, MaxZ));
		//occupied_area += m_decoration_models[i]->boundingBox->WorldDepth() * m_decoration_models[i]->boundingBox->WorldWidth();
	}
	m_empty_area = m_area - occupied_area;
}
