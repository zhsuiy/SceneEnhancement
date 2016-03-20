#include "Parameter.h"
#include "Utility.h"

Parameter* Parameter::m_parameter;

Parameter::Parameter()
{	
	init();
}

Parameter::~Parameter()
{
}

void Parameter::init()
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
			this->LightDir = parts[1].trimmed();
		}
		if (parts[0].compare("ScreenWidth", Qt::CaseInsensitive) == 0)
		{
			this->ScreenWidth = parts[1].toFloat();
		}
		if (parts[0].compare("ScreenHeight", Qt::CaseInsensitive) == 0)
		{
			this->ScreenHeight = parts[1].toFloat();
		}
		if (parts[0].compare("SceneTemplate", Qt::CaseInsensitive) == 0)
		{
			this->SceneTemplatePath = parts[1].trimmed();
		}
		if (parts[0].compare("FurnitureType", Qt::CaseInsensitive) == 0)
		{
			this->FurnitureTypes = Utility::ParseFurnitureTypes(parts[1]);
		}
		if (parts[0].compare("Dataset", Qt::CaseInsensitive) == 0)
		{
			this->DatasetPath = parts[1].trimmed();
		}
	}
	file->close();
	delete file;
}

