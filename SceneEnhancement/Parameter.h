#pragma once
#include <QtCore/qstring.h>
#include "Light.h"
#include <QtCore/qfile.h>
#include <iostream>
#include "Global.h"

class Parameter
{
public:
	Parameter();	
	~Parameter();
	QString lightDir;
	QVector<Light*> ParseLights() const;	
};
