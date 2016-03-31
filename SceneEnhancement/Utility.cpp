#include "Utility.h"
#include <iostream>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qfile.h>
#include "Global.h"
#include "Parameter.h"
#include <QtCore/qdir.h>
#include <ctime>

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
	Assets *assets = Assets::GetAssetsInstance();
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
			assets->RoomWidth = QStr2Float(parts[1]);
		if (parts[0].compare("RoomHeight", Qt::CaseInsensitive) == 0)
			assets->RoomHeight = QStr2Float(parts[1]);
		if (parts[0].compare("RoomDepth", Qt::CaseInsensitive) == 0)
			assets->RoomDepth = QStr2Float(parts[1]);
		if (QStrCmp(parts[0], "WallColor"))
			assets->WallColor = Str2Vec3D(parts[1]);
		if (QStrCmp(parts[0], "FloorTexture"))
			assets->FloorTexture = parts[1].trimmed();
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
			QVector<FurnitureLocationType> locationTypes;
			for (size_t i = 0; i < 5; i++)
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
				if (QStrCmp(inner_parts[0], "Location"))
					locationTypes = ParseLocationTypes(inner_parts[1]);
					
			}
			FurnitureModel* model = new FurnitureModel(type, name, translate, rotate,locationTypes, scale);
			furniture_models.push_back(model);			
		}
	}
	file->close();
	delete file;
	return furniture_models;
}

QVector<DecorationModel*> Utility::ParseDecorationModels(QString& path)
{
	Parameter *para = Parameter::GetParameterInstance();
	QVector<DecorationModel*> decoration_models;
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
		
		if (parts[0].compare("Type", Qt::CaseInsensitive) == 0)
		{
			DecorationType type = parts[1].trimmed();
			if (!para->DecorationTypes.contains(type))
			{
				qWarning("%s is not included", type.toStdString().c_str());
				continue;
			}			
			
			QString path;
			FurnitureType support_type;
			QString location_type;
			float scale;
			QVector3D translate;
			for (size_t i = 0; i < 5; i++)
			{
				QByteArray inner_line = file->readLine();
				QString inner_str(inner_line);
				QStringList inner_parts = inner_str.split('=', QString::SkipEmptyParts);
				if (inner_parts.size() == 0)
					continue;
				if (QStrCmp(inner_parts[0], "Path"))
					path = inner_parts[1].trimmed();
				if (QStrCmp(inner_parts[0], "Support"))
					support_type = inner_parts[1].trimmed();				
				if (QStrCmp(inner_parts[0], "Location"))
					location_type = inner_parts[1].trimmed();
				if (QStrCmp(inner_parts[0], "Scale"))
					scale = QStr2Float(inner_parts[1]);
				if (QStrCmp(inner_parts[0], "Translate"))
					translate = Str2Vec3D(inner_parts[1]);

			}
			DecorationModel* model = new DecorationModel(support_type,type, location_type,scale,translate,path);
			decoration_models.push_back(model);
		}
	}
	file->close();
	delete file;
	return decoration_models;
}

float Utility::QStr2Float(QString &str)
{
	return str.trimmed().toFloat();
}

bool Utility::QStr2Bool(QString& str)
{
	if (QStrCmp(str, "true"))
		return true;
	else if (QStrCmp(str, "false"))
		return false;
	else
	{
		qWarning("Wrong parameter at DrawboundingBox");
		return false;
	}
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



QString Utility::GetFurnitureModelPath(QString& type, QString& name)
{
	
	QString result = Parameter::GetParameterInstance()->DatasetPath
		+ "furniture/" + type + "/" + name + "/model.obj";
	return result;
}

QString Utility::GetDecorationModelPath(QString& type, QString& name)
{
	QString result = Parameter::GetParameterInstance()->DatasetPath
		+ "decoration/" + type + "/" + name + "/model.obj";
	return result;
}

QString Utility::GetDecorationModelPath(QString& type)
{
	QString path = Parameter::GetParameterInstance()->DatasetPath
		+ "decoration/" + type;
	QDir directory(path);
	if (!directory.exists())
		return 0;
	QStringList names;
	QFileInfoList list = directory.entryInfoList();
	for (int i = 2; i<list.size(); i++)
	{
		QFileInfo fileInfo = list.at(i);
		if (fileInfo.isDir())
		{
			names.push_back(fileInfo.fileName());			
		}		
	}	
	qsrand(time(NULL));
	// random
	int r = qrand() % names.size();
	QString name = names[r];	
	return path + "/" + name + "/model.obj";
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

QVector<QString> Utility::QStr2StrVector(QString types)
{
	QVector<QString> result;
	QStringList parts = types.split(' ', QString::SkipEmptyParts);
	for (size_t i = 0; i < parts.size(); i++)
	{
		result.push_back(parts[i].trimmed());
	}
	return result;
}

QMap<QString, QVector3D> Utility::ParseColorsFromFile(QString& path)
{
	Parameter *para = Parameter::GetParameterInstance();
	QMap<QString, QVector3D> colors;
	QFile *file = new QFile(path);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + path.toStdString() << endl;
	while (!file->atEnd())
	{
		QByteArray line = file->readLine();
		QString str(line);
		QStringList parts = str.split('=', QString::SkipEmptyParts);
		if (parts.size() < 2) // skip blank line
			continue;

		QString key = parts[0].trimmed();
		QVector3D color = Str2Vec3D(parts[1]) / 255.0;
		if (!colors.contains(key))
		{
			colors[key] = color;
		}
	}
	file->close();
	delete file;
	return colors;
}

QMap<FurnitureType, QVector<QString>> Utility::ParseMaterialMapFromFile(QString& path)
{
	Parameter *para = Parameter::GetParameterInstance();
	QMap<FurnitureType, QVector<QString>> materialColors;
	QFile *file = new QFile(path);
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		std::cout << "Can't open file " + path.toStdString() << endl;
	while (!file->atEnd())
	{
		QByteArray line = file->readLine();
		QString str(line);
		QStringList parts = str.split('=', QString::SkipEmptyParts);
		if (parts.size() < 2) // skip blank line
			continue;
		QString key = parts[0].trimmed();
		QVector<QString> value;
		value = QStr2StrVector(parts[1]);		
		if (!materialColors.contains(key))
		{
			materialColors[key] = value;
		}
	}
	file->close();
	delete file;
	return materialColors;
}

Material* Utility::GetMaterialFromSingleTexture(QString path)
{
	QOpenGLTexture *gl_texture;
	gl_texture = new QOpenGLTexture(QImage(path).mirrored());
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

Material* Utility::GetMaterialFromString(QString& material)
{
	if (Assets::GetAssetsInstance()->GetColors().contains(material)) // it is a color
	{
		return GetMaterialFromSingleColor(Assets::GetAssetsInstance()->GetColorByName(material));
	}
	else // it is an image path
	{
		return GetMaterialFromSingleTexture(material);
	}
}

DecorationLocationType Utility::GetLocationTypeFromString(QString type)
{
	DecorationLocationType result = NotSet;
	if (QStrCmp(type,"NotSet"))
		result = NotSet;
	else if (QStrCmp(type, "Center"))
		result = Center;
	else if (QStrCmp(type, "Left"))
		result = Left;
	else if (QStrCmp(type, "Right"))
		result = Right;
	else if (QStrCmp(type, "Back"))
		result = Back;
	else if (QStrCmp(type, "Front"))
		result = Front;
	else
		qWarning("Invalid decoration location: %s ", type.toStdString().c_str());
	return result;
}

QVector<FurnitureLocationType> Utility::ParseLocationTypes(QString types)
{
	QVector<FurnitureLocationType> result;
	QStringList type_list = types.split(' ', QString::SkipEmptyParts);
	for (size_t i = 0; i < type_list.size(); i++)
	{
		if (QStrCmp(type_list[i], "Bottom"))
			result.push_back(FTBottom);
		else if (QStrCmp(type_list[i], "Up"))
			result.push_back(FTUp);
		else if (QStrCmp(type_list[i], "Left"))
			result.push_back(FTLeft);
		else if (QStrCmp(type_list[i], "Right"))
			result.push_back(FTRight);
		else if (QStrCmp(type_list[i], "Back"))
			result.push_back(FTBack);
		else if (QStrCmp(type_list[i], "Front"))
			result.push_back(FTFront);
		else
			qWarning("Invalid decoration location: %s ", type_list[i].toStdString().c_str());

	}
	return result;
}
