#include "ColorPalette.h"
#include <algorithm>
#include <QColor>
#include <QtGui/QVector3D>
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
inline float gamma(float x)
{
	return x>0.04045 ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92;
};

inline QVector3D* RGBToLab(QColor &color)
{
	QVector3D *lab = new QVector3D;
	int r = color.red();
	int g = color.green();
	int b = color.blue();
	float B = gamma(color.blueF());	
	float G = gamma(color.greenF());
	float R = gamma(color.redF());
	float X = 0.412453*R + 0.357580*G + 0.180423*B;
	float Y = 0.212671*R + 0.715160*G + 0.072169*B;
	float Z = 0.019334*R + 0.119193*G + 0.950227*B;

	X /= 0.950456;
	Y /= 1.0;
	Z /= 1.088754;

	float FX = X > 0.008856f ? pow(X, 1.0f / 3.0f) : (7.787f * X + 16.0 / 116.0f);
	float FY = Y > 0.008856f ? pow(Y, 1.0f / 3.0f) : (7.787f * Y + 16.0 / 116.0f);
	float FZ = Z > 0.008856f ? pow(Z, 1.0f / 3.0f) : (7.787f * Z + 16.0 / 116.0f);
	lab->setX(Y > 0.008856f ? (116.0f * FY - 16.0f) : (903.3f * Y));
	lab->setY(500.f * (FX - FY));
	lab->setZ(200.f * (FY - FZ));
	return lab;
}

double ColorPalette::GetColorDistance(QColor& c1, QColor& c2, ColorDistanceType type)
{
	double distance = 0.0;
	if (type == HSV)
	{
		int h1, s1, v1;
		int h2, s2, v2;
		c1.getHsv(&h1, &s1, &v1);
		c2.getHsv(&h2, &s2, &v2);


		double n_h = abs(h1 - h2) / 360.0;
		double n_s = abs(s1 - s2) / 255.0;
		double n_v = abs(v1 - v2) / 255.0;
		n_h = qMin(n_h, 1.0f - n_h);
		// 判断h和s是否绝对值很小
		double alpha_h = 1.0, alpha_s = 0.2, alpha_v = 0.1;

		if (s1 < 0.01 || s2 < 0.01)
		{
			alpha_h = 0.1;
			alpha_s = 0.6;
			alpha_v = 0.6;
		}

		if (v1 < 0.1 || v2 < 0.1) // v很小的时候，基本是黑色的，只匹配暗色的
		{
			alpha_h = 0.1;
			alpha_s = 0.2;
			alpha_v = 1.0;
		}
		distance = alpha_h*n_h*n_h + alpha_s*n_s*n_s + alpha_v*n_v*n_v;

	}
	else if (type == LAB)
	{
		QVector3D *lab1 = RGBToLab(c1);
		QVector3D *lab2 = RGBToLab(c2);
		distance = lab1->distanceToPoint(*lab2);
		delete lab1;
		delete lab2;
	}	
	
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
