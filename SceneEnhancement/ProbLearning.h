#pragma once
#include "Parameter.h"

typedef QList<QPair<DecorationType, QPair<FurnitureType, QVector<DecorationLocationType>>>> ImageDecorationType;
typedef QMap<FurnitureType, QVector<QColor>> ImageFurnitureColorType;
class ProbLearning : public QObject
{
public:
	ProbLearning();
	void Learn();


private:
	// outer key denotes the pos/neg of sample	
	QMap<int, QVector<ImageFurnitureColorType>> m_furniture_colors;
	QMap<int, QVector<ImageDecorationType>> m_decorations;
	Parameter *m_para;
	QString m_adj_name;

	QVector<ImageFurnitureColorType> GetFurnitureColors(QString &path);
	QVector<ImageDecorationType> GetDecorations(QString &path);
	ImageFurnitureColorType readFurnitureInfo(QString &filepath);
};