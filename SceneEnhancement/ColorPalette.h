#pragma once
#include <QtCore/QVector>

class QColor;

class ColorPalette
{
public:
	static double GetColorPaletteDistance(ColorPalette *cp1, ColorPalette *cp2);
	ColorPalette();
	ColorPalette(QVector<QColor> colors);
	QVector<QColor> Colors;
private:


	
};
