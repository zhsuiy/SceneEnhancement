#include "Utility.h"
#include <iostream>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

QVector3D Utility::Str2Vec3D(QString str)
{
	float x, y, z;
	QStringList numbers = str.split(' ', QString::SkipEmptyParts);
	if (numbers.size() < 3)
		std::cout << "Invalid direction parameter\n";
	x = numbers[0].toFloat();
	y = numbers[1].toFloat();
	z = numbers[2].toFloat();
	return QVector3D(x,y,z);
}
