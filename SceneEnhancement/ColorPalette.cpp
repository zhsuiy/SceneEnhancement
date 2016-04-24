#include "ColorPalette.h"
#include <QColor>
double ColorPalette::GetColorPaletteDistance(ColorPalette *cp1, ColorPalette *cp2)
{
	int colornum = qMin(cp1->Colors.size(), cp2->Colors.size());
	double distance = 0;
	for (size_t i = 0; i < colornum; i++)
	{
		int h1, s1, v1;
		int h2, s2, v2;
		cp1->Colors[i].getHsv(&h1, &s1, &v1);
		cp2->Colors[i].getHsv(&h2, &s2, &v2);
		double n_h = abs(h1 - h2) / 360.0;
		double n_s = abs(s1 - s2) / 255.0;
		double n_v = abs(v1 - v2) / 255.0;
		n_h = qMin(n_h, 1.0f - n_h);
		distance += (n_h + n_s + 0.3*n_v);	
	}
	distance = distance / colornum; 
	return distance;
}

ColorPalette::ColorPalette()
{
}

ColorPalette::ColorPalette(QVector<QColor> colors)
{
	Colors = colors;
}
