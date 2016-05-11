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
	QString DatasetPath; // ģ��·��
	QString LabelsPath; // ��ע�����·��
	QString AdjName; // ���ݴ�
	QString DecorationZOrdersPath; // С�����ǰ��˳���ϵ
	int FurnitureClusterNum; // ÿ���Ҿߵ���ɫ���������
	bool IsDrawBoundingBox;
private:
	Parameter();	
	~Parameter();
	void init();
	static Parameter* m_parameter;
	
};
