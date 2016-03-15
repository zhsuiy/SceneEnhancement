#include "Parameter.h"
#include "Utility.h"

Parameter::Parameter()
{
	QFile *file = new QFile(G_ParameterDir);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + G_ParameterDir.toStdString() << endl;
	while (!file->atEnd())
	{
		QByteArray line = file->readLine();
		QString str(line);
		QStringList parts = str.split('=', QString::SkipEmptyParts);
		if (parts.size() == 0) // skip blank line
			continue;		
		if (parts[0].compare("LightDir", Qt::CaseInsensitive) == 0)
		{
			this->lightDir = parts[1].trimmed();
		}
		if (parts[0].compare("ScreenWidth", Qt::CaseInsensitive) == 0)
		{
			G_ScreenWidth = parts[1].toFloat();
		}
		if (parts[0].compare("ScreenHeight", Qt::CaseInsensitive) == 0)
		{
			G_ScreenHeight = parts[1].toFloat();
		}
		if (parts[0].compare("SceneTemplate", Qt::CaseInsensitive) == 0)
		{
			G_SceneTemplatePath = parts[1].trimmed();
		}
	}
	file->close();
	delete file;
}

Parameter::~Parameter()
{
}

QVector<Light*> Parameter::ParseLights() const
{
	QVector<Light*> lights;
	QFile *file = new QFile(this->lightDir);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + this->lightDir.toStdString() << endl;
	while (!file->atEnd())
	{
		QByteArray line = file->readLine();
		QString str(line);
		if (str.contains("Directional light", Qt::CaseInsensitive))
		{
			QVector3D direction, ambient, diffuse, specular;
			for (size_t i = 0; i < 4; i++)
			{
				QByteArray inner_line = file->readLine();
				QString inner_str(inner_line);
				QStringList parts = inner_str.split('=', QString::SkipEmptyParts);
				if (parts.size() == 0)
					continue;
				if (parts[0].compare("Dir",Qt::CaseInsensitive) == 0)
				{
					direction = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Ambient",Qt::CaseInsensitive) == 0)
				{
					ambient = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Diffuse",Qt::CaseInsensitive) == 0)
				{
					diffuse = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Specular",Qt::CaseInsensitive) == 0)
				{
					specular = Utility::Str2Vec3D(parts[1]);
				}				
			}
			lights.push_back(new DirectionLight(direction, ambient, diffuse, specular));			
		}
		if (str.contains("Point Light", Qt::CaseInsensitive))
		{
			int id = 0;
			QVector3D position, ambient, diffuse, specular;
			float constant = 1, linear = 0.09f, quadratic = 0.032f;
			for (size_t i = 0; i < 8; i++)
			{
				QByteArray inner_line = file->readLine();
				QString inner_str(inner_line);
				QStringList parts = inner_str.split('=', QString::SkipEmptyParts);
				if (parts.size() == 0)
					continue;
				if (parts[0].compare("Id", Qt::CaseInsensitive) == 0)
				{
					id = parts[1].toInt();
				}
				if (parts[0].compare("Position", Qt::CaseInsensitive) == 0)
				{
					position = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Ambient",Qt::CaseInsensitive) == 0)
				{
					ambient = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Diffuse",Qt::CaseInsensitive) == 0)
				{
					diffuse = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Specular",Qt::CaseInsensitive) == 0)
				{
					specular = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Constant", Qt::CaseInsensitive) == 0)
				{
					constant = parts[1].toFloat();
				}
				if (parts[0].compare("Linear",Qt::CaseInsensitive) == 0)
				{
					linear = parts[1].toFloat();
				}
				if (parts[0].compare("Quadratic",Qt::CaseInsensitive) == 0)
				{
					quadratic = parts[1].toFloat();
				}
			}
			lights.push_back(new PointLight(id, position, ambient, diffuse, specular, linear, quadratic, constant));
		}
	}
	file->close();
	delete file;

	return lights;
}
