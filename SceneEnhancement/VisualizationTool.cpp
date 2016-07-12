#include "VisualizationTool.h"
#include <QtGui/qimage.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <QtCore/qdir.h>

void VisualizationTool::DrawAllFurnitureClusters(QMap<QString, QMap<int, QVector<ColorPalette*>>> all_furnitures)
{
	QDir dir;
	QString path = "./clusterresult";
	if (!dir.exists(path))
		dir.mkdir(path);
	path = path + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss/");
	if (!dir.exists(path))
		dir.mkdir(path);
	QMapIterator<QString, QMap<int, QVector<ColorPalette*>>> it(all_furnitures);
	while (it.hasNext())
	{
		it.next();
		QString furniturepath = path + "/" + it.key();
		if (!dir.exists(furniturepath))
		{
			dir.mkdir(furniturepath);
		}
		DrawClusterColors(furniturepath, it.value());
	}
}

// single palette
void VisualizationTool::DrawClusterColors(QString path, QMap<int, QVector<ColorPalette*>> colors)
{
	QDir dir;
	QMapIterator<int, QVector<ColorPalette*>> it(colors);
	while (it.hasNext())
	{
		it.next();
		QString filepath = path + "/" + QString::number(it.key());
		if (!dir.exists(filepath))
			dir.mkdir(filepath);
		auto clustercolors = it.value(); // 某个聚类的所有颜色
		for (size_t i = 0; i <clustercolors.size(); i++) //每个颜色都要保存成一张图片
		{
			QImage iim(150, 150, QImage::Format_ARGB32);
			QPainter *painter = new QPainter(&iim);
			QBrush *brush = new QBrush(QColor(255, 0, 0));
			auto sampletype = clustercolors[i]->SampleType;
			QString type;
			if (sampletype == Pos)
				type = "pos";
			else
				type = "neg";
			for (size_t j = 0; j < clustercolors[i]->Colors.size(); j++)
			{
				brush->setColor(clustercolors[i]->Colors[j]);
				painter->setBrush(*brush);
				painter->drawRect(50 * j , 0, 50, 150);
			}	
			painter->end();			
			iim.save(filepath + "/" + type + "_" + QString::number(i) + ".png");
		}
	}	
}


