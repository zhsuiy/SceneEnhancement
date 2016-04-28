#include "ColorPalette.h"
#include <algorithm>
#include <QColor>
using namespace std;
double ColorPalette::GetColorPaletteDistance(ColorPalette *cp1, ColorPalette *cp2, bool use_color_order)
{
	if (!use_color_order)
	{
		int n1 = cp1->Colors.size();
		int n2 = cp2->Colors.size();
		QList<QPair<int, int>> pairs;
		vector<int> index_list1;
		for (size_t i = 0; i < n1; i++)
			index_list1.push_back(i);
		vector<int> index_list2;
		for (size_t i = 0; i < n2; i++)
			index_list2.push_back(i);
		int n = qMin(n1, n2);
		double min_distance = 0.0;
		// init distance
		for (size_t i = 0; i < n; i++)
		{
			min_distance += GetColorDistance(cp1->Colors[i], cp2->Colors[i]);
		}

		while (next_permutation(index_list2.begin(), index_list2.end()))
		{
			double distance = 0;
			for (size_t i = 0; i < n; i++)
			{
				distance += GetColorDistance(cp1->Colors[index_list1[i]], cp2->Colors[index_list2[i]]);
			}
			min_distance = min_distance > distance ? distance : min_distance;
		}
		min_distance = min_distance / n;
		return min_distance;
	}
	else
	{
		int colornum = qMin(cp1->Colors.size(), cp2->Colors.size());
		double distance = 0;
		for (size_t i = 0; i < colornum; i++)
		{
			distance += (double)(colornum - i)/colornum * GetColorDistance(cp1->Colors[i], cp2->Colors[i]);
		}
		distance = distance / colornum;
		return distance;
	}
}

double ColorPalette::GetColorDistance(QColor& c1, QColor& c2)
{
	double distance = 0.0;
	int h1, s1, v1;
	int h2, s2, v2;	
	c1.getHsv(&h1, &s1, &v1);
	c2.getHsv(&h2, &s2, &v2);
	double n_h = abs(h1 - h2) / 360.0;
	double n_s = abs(s1 - s2) / 255.0;
	double n_v = abs(v1 - v2) / 255.0;
	n_h = qMin(n_h, 1.0f - n_h);
	distance = n_h*n_h + 0.2*n_s*n_s + 0.1*n_v*n_v;
	return distance;

	////double distance = 0.0;
	//double n_r = abs(c1.redF() - c2.redF());
	//double n_g = abs(c1.greenF() - c2.greenF());
	//double n_b = abs(c1.blueF() - c2.blueF());
	//return sqrt(n_r*n_r + n_g*n_g + n_b*n_b);

}

ColorPalette::ColorPalette():ClusterIndex(0)
{
}

ColorPalette::ColorPalette(QVector<QColor> colors)
{
	Colors = colors;
	ClusterIndex = 0;
}
