#pragma once
#include "Parameter.h"
#include "ColorPalette.h"
using namespace std;
typedef QList<QPair<DecorationType, QPair<FurnitureType, QVector<DecorationLocationType>>>> ImageDecorationType;
typedef QMap<FurnitureType, ColorPalette*> ImageFurnitureColorType;
typedef int ClusterIndex;
class ProbLearning : public QObject
{
public:
	ProbLearning();
	void Learn();


private:
	// outer key denotes the pos/neg of sample	
	QMap<int, QVector<ImageFurnitureColorType>> m_furniture_colors;
	QMap<int, QVector<ImageDecorationType>> m_decorations;
	Parameter *m_para;
	QString m_adj_name;

	// get info from labels
	void ReadInfoFromLabels();
	QVector<ImageFurnitureColorType> GetFurnitureColors(QString &path);
	QVector<ImageDecorationType> GetDecorations(QString &path);	

	// precomputation, probabilities
	// furniture color
	void CalculateFurnitureColorProb();
	QVector<FurnitureType> m_furniture_types;
	QMap<FurnitureType, QMap<ClusterIndex, QColor>> furniture_color_index;
	QMap<FurnitureType, QMap<ClusterIndex, double>> furniture_color_probs; // in use	
	QMap<FurnitureType, QMap<ClusterIndex, QVector<ColorPalette*>>> furniture_color_clusters;
	vector<vector<int>> get_furniture_clusters(FurnitureType furniture_type, QVector<ColorPalette*> colors);

	void CalculateFurniturePairwiseColorProb();
	QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> furniture_pairwise_color_probs;


	// decorations
	void CulculateDecorationProb();
	QVector<DecorationType> m_decoration_types;
	QMap<DecorationType, double> decoration_probs;
	//vector<vector<int>> decoration_occurrence;
		
};