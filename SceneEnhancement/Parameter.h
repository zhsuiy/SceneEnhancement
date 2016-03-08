#pragma once
#include <QtCore/qstring.h>

class Parameter
{
public:
	static Parameter* GetParameter()
	{
		if (m_parameter == nullptr)
			m_parameter = new Parameter;
		return m_parameter;
	}

	Parameter()
	{
		
	}	

private:
	static Parameter *m_parameter;
	
};
