#include "Utility.h"
#include <iostream>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qfile.h>
#include "Global.h"
#include "Parameter.h"

QVector3D Utility::Str2Vec3D(QString &str)
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

QVector<FurnitureModel*> Utility::ParseFurnitureModels(QString &path)
{
	Parameter *para = Parameter::GetParameterInstance();
	QVector<FurnitureModel*> furniture_models;
	QFile *file = new QFile(path);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + path.toStdString() << endl;
	while (!file->atEnd())
	{
		QByteArray line = file->readLine();
		QString str(line);
		QStringList parts = str.split('=', QString::SkipEmptyParts);
		if (parts.size() == 0) // skip blank line
			continue;
		// room area
		if (parts[0].compare("RoomWidth", Qt::CaseInsensitive) == 0)
			G_RoomWidth = QStr2Float(parts[1]);
		if (parts[0].compare("RoomHeight", Qt::CaseInsensitive) == 0)
			G_RoomHeight = QStr2Float(parts[1]);
		if (parts[0].compare("RoomDepth", Qt::CaseInsensitive) == 0)
			G_RoomDepth = QStr2Float(parts[1]);

		if (parts[0].compare("Furniture",Qt::CaseInsensitive) == 0)
		{
			FurnitureType type = parts[1].trimmed();
			if (!para->FurnitureTypes.contains(type))
			{
				qWarning("%s is not included",type.toStdString().c_str());
				continue;
			}
			FurnitureName name;
			float scale;
			QVector3D translate, rotate;
			for (size_t i = 0; i < 4; i++)
			{
				QByteArray inner_line = file->readLine();
				QString inner_str(inner_line);
				QStringList inner_parts = inner_str.split('=', QString::SkipEmptyParts);
				if (inner_parts.size() == 0)
					continue;
				if (QStrCmp(inner_parts[0],"Path"))
					name = inner_parts[1].trimmed();
				if (QStrCmp(inner_parts[0],"Scale"))
					scale = QStr2Float(inner_parts[1]);
				if (QStrCmp(inner_parts[0],"Translate"))
					translate = Str2Vec3D(inner_parts[1]);
				if (QStrCmp(inner_parts[0], "Rotate"))
					rotate = Str2Vec3D(inner_parts[1]);
			}
			FurnitureModel* model = new FurnitureModel(type, name, translate, rotate, scale);
			furniture_models.push_back(model);
		}
	}
	file->close();
	delete file;
	return furniture_models;
}

float Utility::QStr2Float(QString &str)
{
	return str.trimmed().toFloat();
}

bool Utility::QStrCmp(QString& str1, char* str2)
{
	QString *qstr2 = new QString(str2);
	if (str1.trimmed().compare(qstr2->trimmed(),Qt::CaseInsensitive) == 0)
	{
		delete qstr2;
		return true;
	}
	else
	{
		delete qstr2;
		return false;
	}
}

QString Utility::GetModelPath(QString& type, QString& name)
{
	
	QString result = Parameter::GetParameterInstance()->DatasetPath
		+ type + "/" + name + "/model.obj";
	return result;
}

QVector<Light*> Utility::ParseLights()
{
	Parameter *para = Parameter::GetParameterInstance();
	QVector<Light*> lights;
	QFile *file = new QFile(para->LightDir);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + para->LightDir.toStdString() << endl;
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
				if (parts[0].compare("Dir", Qt::CaseInsensitive) == 0)
				{
					direction = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Ambient", Qt::CaseInsensitive) == 0)
				{
					ambient = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Diffuse", Qt::CaseInsensitive) == 0)
				{
					diffuse = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Specular", Qt::CaseInsensitive) == 0)
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
				if (parts[0].compare("Ambient", Qt::CaseInsensitive) == 0)
				{
					ambient = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Diffuse", Qt::CaseInsensitive) == 0)
				{
					diffuse = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Specular", Qt::CaseInsensitive) == 0)
				{
					specular = Utility::Str2Vec3D(parts[1]);
				}
				if (parts[0].compare("Constant", Qt::CaseInsensitive) == 0)
				{
					constant = parts[1].toFloat();
				}
				if (parts[0].compare("Linear", Qt::CaseInsensitive) == 0)
				{
					linear = parts[1].toFloat();
				}
				if (parts[0].compare("Quadratic", Qt::CaseInsensitive) == 0)
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

QVector<QString> Utility::ParseFurnitureTypes(QString types)
{
	QVector<QString> result;
	QStringList parts = types.split(' ', QString::SkipEmptyParts);
	for (size_t i = 0; i < parts.size(); i++)
	{
		result.push_back(parts[i].trimmed());
	}
	return result;
}

Material* Utility::GetMaterialFromSingleTexture(QString path)
{
	QOpenGLTexture *gl_texture;
	QString filename = "./Resources/texture/floor.jpg";
	gl_texture = new QOpenGLTexture(QImage(filename).mirrored());
	//texture->setAutoMipMapGenerationEnabled(true);
	gl_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	gl_texture->setMagnificationFilter(QOpenGLTexture::Linear);
	gl_texture->setWrapMode(QOpenGLTexture::MirroredRepeat);

	Texture *texture = new Texture();
	texture->texture = gl_texture;
	texture->type = DiffuseTexture;

	QVector<Texture*> tmptextures;
	tmptextures.push_back(texture);

	Material* result = new Material();
	result->Diffuse = new MaterialElement(tmptextures);

	return result;	
}

Material* Utility::GetMaterialFromSingleColor(QVector3D &diffuse_color)
{
	Material* result = new Material();
	result->Diffuse = new MaterialElement(diffuse_color);
	return result;
}