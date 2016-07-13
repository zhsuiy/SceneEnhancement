#include "Parameter.h"
#include "Utility.h"

Parameter* Parameter::m_parameter;

void Parameter::Update()
{
	init();
}

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
		if (parts[0].compare("ColorMap",Qt::CaseInsensitive) == 0)
		{
			this->ColorMapPath = parts[1].trimmed();
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
		if (parts[0].compare("DecorationPath",Qt::CaseInsensitive) == 0)
		{
			this->DecorationModelsPath = parts[1].trimmed();
		}
		if (parts[0].compare("MaterialPath",Qt::CaseInsensitive) == 0)
		{
			this->MaterialMapPath = parts[1].trimmed();
		}
		if (parts[0].compare("FurnitureType", Qt::CaseInsensitive) == 0)
		{
			this->FurnitureTypes = Utility::QStr2StrVector(parts[1]);
		}
		if (parts[0].compare("UseFurnitureType",Qt::CaseInsensitive) == 0)
		{
			this->FurnitureTypesUseTextures = Utility::QStr2StrVector(parts[1]);
		}
		if (parts[0].compare("DecorationType", Qt::CaseInsensitive) == 0)
		{
			this->DecorationTypes = Utility::QStr2StrVector(parts[1]);
		}
		if (parts[0].compare("MultiDecorationType",Qt::CaseInsensitive) == 0)
		{
			this->DecorationMultiTypes = Utility::QStr2StrVector(parts[1]);
		}
		if (parts[0].compare("DecorationScalePath",Qt::CaseInsensitive) == 0)
		{
			this->DecorationScalePath = parts[1].trimmed();
		}
		if (parts[0].compare("Dataset", Qt::CaseInsensitive) == 0)
		{
			this->DatasetPath = parts[1].trimmed();
		}
		if (parts[0].compare("Labels",Qt::CaseInsensitive) == 0)
		{
			this->LabelsPath = parts[1].trimmed();
		}
		if (parts[0].compare("Texture",Qt::CaseInsensitive) == 0)
		{
			this->TexturePath = parts[1].trimmed();
		}
		if (parts[0].compare("AdjName",Qt::CaseInsensitive) == 0)
		{
			this->AdjName = parts[1].trimmed();
		}
		if (parts[0].compare("DecorationZOrderPath",Qt::CaseInsensitive) == 0)
		{
			this->DecorationZOrdersPath = parts[1].trimmed();
		}
		if (parts[0].compare("DecorationMedialOrderPath", Qt::CaseInsensitive) == 0)
		{
			this->DecorationMedialOrderPath = parts[1].trimmed();
		}
		if (parts[0].compare("DecorationHOrderPath",Qt::CaseInsensitive) == 0)
		{
			this->DecorationHOrderPath = parts[1].trimmed();
		}
		if (parts[0].compare("FurnitureClusterNum",Qt::CaseInsensitive) == 0)
		{
			this->FurnitureClusterNum = Utility::QStr2Int(parts[1]);
		}
		if (parts[0].compare("DrawBoundingBox",Qt::CaseInsensitive) == 0)
		{
			this->IsDrawBoundingBox = Utility::QStr2Bool(parts[1]);
		}
		if (parts[0].compare("MaxFurnitureNum",Qt::CaseInsensitive) == 0)
		{
			this->MaxSupportNumber = Utility::QStr2Int(parts[1]);
		}
	}
	file->close();
	delete file;
}

