#pragma once
#include <QtCore/qstring.h>
#include "Light.h"
#include <QtCore/qfile.h>
#include <iostream>
#include "Global.h"
#include "Assets.h"

#define FurnitureType QString

class Parameter
{
public:
	static  Parameter* GetParameterInstance()
	{
		if (!m_parameter)
		{
			m_parameter = new Parameter();
		}
		return m_parameter;
	};
	void Update();
	QString LightDir;
	QString ColorMapPath;
	int ScreenWidth;
	int ScreenHeight;
	QString SceneTemplatePath;
	QString DecorationModelsPath;
	QString MaterialMapPath;
	QString TexturePath;
	QString DecorationScalePath;
	QVector<FurnitureType> FurnitureTypes;
	QVector<FurnitureType> FurnitureTypesUseTextures;
	QVector<DecorationType> DecorationTypes;
	QVector<DecorationType> DecorationMultiTypes; // 允许多次出现的小物体
	QVector<DecorationType> MultiOccurInSameFurniture; // 允许在同一家具上多次出现的小物体
	QString DatasetPath; // 模型路径
	QString LabelsPath; // 标注结果的路径
	QString AdjName; // 形容词
	QString DecorationZOrdersPath; // 小物件的前后顺序关系
	QString DecorationMedialOrderPath; // 小物体靠近中间的顺序关系
	QString DecorationHOrderPath; // 小物体的高低顺序
	int FurnitureClusterNum; // 每个家具的颜色聚类的数量
	int MaxSupportNumber;	// 每个小物件最多能出现在几个家具上
	int DecorationNumber; // 出现多少种小物件
	bool IsDrawBoundingBox;
	bool IsAllowTopSupport; // 是否允许物体的顶层放东西
	
private:
	Parameter();	
	~Parameter();
	void init();
	static Parameter* m_parameter;
	
};
