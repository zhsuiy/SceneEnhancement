#include "SupportRegion.h"
#include <QtCore/qglobal.h>
#include <QtGui/QVector3D>
#include "DecorationModel.h"
#include "Utility.h"
#include "Assets.h"
#include <ctime>

SupportRegion::SupportRegion()
{
}

SupportRegion::SupportRegion(float min_x, float max_x, float min_z, float max_z, float height, QMatrix4x4 modelMatrix)
{
	srand(time(NULL));
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

#define XRatio double
#define ZRatio double
void SupportRegion::ArrangeDecorationModels(FurnitureModel* support, QVector<DecorationModel*> models)
{
	furniture = support;
	
	srand(time(NULL));
	QList<QPair<QMap<DecorationType, QPair<XRatio, ZRatio>>, double>> all_results;
	QMap<DecorationType,QPair<XRatio, ZRatio>> decoration_pos_ratio;
	QMap<DecorationType, QPair<XRatio, ZRatio>> old_decoration_pos_ratio;
	// 初始化
	for (size_t i = 0; i < models.size(); i++)
	{
		if (!decoration_pos_ratio.contains(models[i]->Type))
		{
			double xratio = (static_cast<double>(rand()) / (RAND_MAX));
			double zratio = (static_cast<double>(rand()) / (RAND_MAX));
			decoration_pos_ratio[models[i]->Type] = QPair<XRatio, ZRatio>(xratio, zratio);
		}
	}
	updateDecorationModelCoords(models, decoration_pos_ratio);
	
	double F = getCost(models, decoration_pos_ratio);
	double Fold = F;
	

	int total_iteration = 10000;
	int n = 0;
	double beta = 1.0;
	while (n++ < total_iteration)
	{
		// 记录更改前的状态
		old_decoration_pos_ratio = QMap<DecorationType, QPair<XRatio, ZRatio>>(decoration_pos_ratio);
		
		// proposal
		applyProposalMoves(decoration_pos_ratio);
		// 根据ratio更新decoration的位置
		updateDecorationModelCoords(models, decoration_pos_ratio);

		F = getCost(models, decoration_pos_ratio);

		double accept_rate = qMin(1.0, exp(-beta*F) / exp(-beta*Fold));
		if ((static_cast<double>(rand()) / (RAND_MAX)) < accept_rate) // 接受
		{
			Fold = F;
			// 保存当前结果
			all_results.push_back(QPair<QMap<DecorationType, QPair<XRatio, ZRatio>>, double>
				(QMap<DecorationType, QPair<XRatio, ZRatio>>(decoration_pos_ratio), F));
		}
		else
		{
			// 回退到更改前
			decoration_pos_ratio = old_decoration_pos_ratio;
		}
	}
	qSort(all_results.begin(), all_results.end(), Utility::QPairSecondComparerAscending());

	// 根据结果更新decoration model的位置
	decoration_pos_ratio = all_results[0].first;
	updateDecorationModelCoords(models, decoration_pos_ratio);
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

void SupportRegion::updateDecorationModelCoords(QVector<DecorationModel*> models, QMap<QString, QPair<double, double>> decoration_XZ)
{
	for (size_t i = 0; i < models.size(); i++)
	{
		float centerx = furniture->boundingBox->WorldLeftBottomBack().x() + furniture->boundingBox->WorldWidth() / 2.0;
		float centerz = furniture->boundingBox->WorldLeftBottomBack().z() + furniture->boundingBox->Depth() / 2.0;
		float xratio = decoration_XZ[models[i]->Type].first;
		float zratio = decoration_XZ[models[i]->Type].second;
		
		float tx = MinX + xratio*(MaxX - MinX) - centerx;
		float tz = MinZ + zratio*(MaxZ - MinZ) - centerz;

		furniture->AdaptTranslateAccord2FrontDirection(tx, tz); // 根据家具的朝向调整translate
		float ty = this->Height - furniture->GetTranslate().y() + abs(models[i]->boundingBox->LeftBottomBack().y())*models[i]->GetScale();
		models[i]->SetRelativeTranslate(tx, ty, tz);
	}
}

void SupportRegion::applyProposalMoves(QMap<QString, QPair<double, double>> &decoration_XZ)
{
	int move = rand() % 3;
	auto keys = decoration_XZ.keys();
	double step = 0.1;
	int di = 0, dj = 0;
	double newx = 0, newz = 0;
	switch (move)
	{
	case 0: // perturb x z ratio
		di = rand() % keys.size();
		// 可以加上高斯分布
		newx = decoration_XZ[keys[di]].first + step *(2*(static_cast<double>(rand()) / (RAND_MAX)) -1);
		newx = newx > 1 ? 1 : newx;
		newx = newx < 0 ? 0 : newx;
		decoration_XZ[keys[di]].first = newx;
		newz = decoration_XZ[keys[di]].second + step *(2 * (static_cast<double>(rand()) / (RAND_MAX)) - 1);
		newz = newz > 1 ? 1 : newz;
		newz = newz < 0 ? 0 : newz;
		decoration_XZ[keys[di]].second = newz;
		break;
	case 1: // swap x
		di = rand() % keys.size();
		dj = rand() % keys.size();
		if (di != dj)
		{
			double tmpxi = decoration_XZ[keys[di]].first;
			decoration_XZ[keys[di]].first = decoration_XZ[keys[dj]].first;
			decoration_XZ[keys[dj]].first = tmpxi;
		}
		break;
	case 2: // swap z
		di = rand() % keys.size();
		dj = rand() % keys.size();
		if (di != dj)
		{
			double tmpzi = decoration_XZ[keys[di]].second;
			decoration_XZ[keys[di]].second = decoration_XZ[keys[dj]].second;
			decoration_XZ[keys[dj]].second = tmpzi;
		}
		break;
	default:
		break;
	}

}

double SupportRegion::getCost(QVector<DecorationModel*> models, QMap<QString, QPair<double, double>> decoration_XZ)
{
	double F = 0.0;

	// 碰撞检测
	F += 10*calculate_collide_area(models);
	// 出界检测
	F += 10*calculate_boundary_test(models);
	// 关于前后order
	F += 3*calculate_decoration_orders(decoration_XZ);

	// 关于左右order

	return F;
}

double SupportRegion::calculate_collide_area(QVector<DecorationModel*> models)
{
	double f = 0.0;
	double sum_area = 0.0;
	for (size_t i = 0; i < models.size(); i++)
	{
		DecorationModel* model1 = models[i];
		sum_area += model1->boundingBox->WorldWidth() * model1->boundingBox->WorldDepth();
		for (size_t j = i; j < models.size(); j++)
		{
			if (j == i) // 同一个物体不计算
			{
				continue;
			}			
			DecorationModel* model2 = models[j];

			f += Utility::GetCrossArea(model1->boundingBox->WorldLeftBottomBack(), model1->boundingBox->WorldRightUpFront(),
				model2->boundingBox->WorldLeftBottomBack(), model2->boundingBox->WorldRightUpFront());
		}
	}
	f = f / sum_area; // 归一化
	return f;
}

double SupportRegion::calculate_boundary_test(QVector<DecorationModel*> models)
{
	double f = 0.0;
	QVector3D lb = QVector3D(MinX, Height, MinZ);
	QVector3D rf = QVector3D(MaxX, Height, MaxZ);
	double sum_area = 0.0;
	for (size_t i = 0; i < models.size(); i++)
	{
		// 检测当前模型有多少面积在support region的内部
		double cross = Utility::GetCrossArea(models[i]->boundingBox->WorldLeftBottomBack(), models[i]->boundingBox->WorldRightUpFront(),
			lb,rf);
		// 总面积 - 在内部的即为在外面的面积
		double area = models[i]->boundingBox->WorldDepth() * models[i]->boundingBox->WorldWidth();
		sum_area += area;
		double outside = area - cross;
		f = f + outside;
	}
	f = f / sum_area; // 归一化

	return f;
}

double SupportRegion::calculate_decoration_orders(QMap<DecorationType, QPair<double, double>> decoration_xz_ratios)
{
	double f = 0.0;
	// 可以放在外面
	QList<QPair<DecorationType, double>> decoration_orders;
	QMapIterator<DecorationType, QPair<double, double>> it(decoration_xz_ratios);
	QMap<DecorationType, double> all_decoration_orders = Assets::GetAssetsInstance()->DecorationZOrders;
	while (it.hasNext())
	{
		it.next();
		if (all_decoration_orders.contains(it.key()))
		{
			decoration_orders.push_back(QPair<DecorationType, double>(it.key(), all_decoration_orders[it.key()]));
		}
	}
	// sort decoration orders;
	qSort(decoration_orders.begin(), decoration_orders.end(), Utility::QPairSecondComparerAscending());

	for (size_t i = 0; i < decoration_orders.size() - 1; i++)
	{
		double zback = decoration_orders[i].second;
		double zfront = decoration_orders[i + 1].second;
		double cost = getPairZOrderCost(decoration_xz_ratios[decoration_orders[i].first],
			decoration_xz_ratios[decoration_orders[i + 1].first]);
		f += (zfront - zback) * cost;
	}
	f = f / decoration_orders.last().second;
	return f;
}

double SupportRegion::getPairZOrderCost(QPair<double, double> back, QPair<double, double> front)
{
	FurnitureFrontDirection ffd = furniture->FurnitureFrontDirection;
	double cost = 0.0;
	double ratioback;
	double ratiofront;
	switch (ffd)
	{
	case Invalid:
		break;
	case XPos:	
		ratioback = back.first;
		ratiofront = front.first;
		if (ratioback > ratiofront) // 只有当后面的摆到前面去的时候才有cost
		{
			cost = 1.0/ (1.0 + exp(-(ratioback - ratiofront)));
		}
		break;
	case XNeg:
		ratioback = back.first;
		ratiofront = front.first;
		if (ratioback < ratiofront) // 
		{
			cost = 1.0 / (1.0 + exp(-(ratiofront - ratioback)));
		}
		break;
	case ZPos:
		ratioback = back.second;
		ratiofront = front.second;
		if (ratioback > ratiofront)
		{
			cost = 1.0 / (1.0 + exp(-(ratioback - ratiofront)));
		}
		break;
	case ZNeg:
		ratioback = back.second;
		ratiofront = front.second;
		if (ratioback > ratiofront)
		{
			cost = 1.0 / (1.0 + exp(-(ratioback - ratiofront)));
		}
		/*if (ratioback < ratiofront)
		{
			cost = 1.0 / (1.0 + exp(-(ratiofront - ratioback)));
		}*/
		break;
	default:
		break;
	}
	return cost;	
}
