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
	QString DatasetPath; // 模型路径
	QString LabelsPath; // 标注结果的路径
	QString AdjName; // 形容词
	QString DecorationZOrdersPath; // 小物件的前后顺序关系
	int FurnitureClusterNum; // 每个家具的颜色聚类的数量
	bool IsDrawBoundingBox;
private:
	Parameter();	
	~Parameter();
	void init();
	static Parameter* m_parameter;
	
};
