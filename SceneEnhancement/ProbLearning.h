#pragma once
#include "Parameter.h"
#include "ColorPalette.h"
using namespace std;
typedef QPair<DecorationType, QPair<FurnitureType, QVector<DecorationLocationType>>> DecorationLabelType;
typedef QList<DecorationLabelType> ImageDecorationType;
typedef QMap<FurnitureType, ColorPalette*> ImageFurnitureColorType;
typedef int ClusterIndex;

enum EnergyType
{
	F1,
	F2,
	F1F2
};

class ProbLearning : public QObject
{
public:
	ProbLearning();
	void Learn(EnergyType et);
	bool IsLearned() const;
	QMap<FurnitureType, ColorPalette*> GetFurnitureColorPalette(int level);
	QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> GetDecorationTypes(int n);
private:
	// state
	bool m_islearned;
	EnergyType m_energy_type;
	// outer key denotes the pos/neg of sample	
	QMap<int, QVector<ImageFurnitureColorType>> m_furniture_colors;
	QMap<int, QVector<ImageDecorationType>> m_decorations;
	Parameter *m_para;
	Assets *m_assets;
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
	// cluster results
	QMap<FurnitureType, QMap<ClusterIndex, QVector<ColorPalette*>>> furniture_color_clusters;
	vector<vector<int>> get_furniture_clusters(FurnitureType furniture_type, QVector<ColorPalette*> colors);

	void CalculateFurniturePairwiseColorProb();
	QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> furniture_pairwise_color_probs;


	// decorations
	void CulculateDecorationProb();
	QVector<DecorationType> m_decoration_types;
	QMap<DecorationType, double> decoration_probs;	
	QMap<DecorationType, QMap<FurnitureType, double>> decoration_support_probs; // 小物件出现在各个大家具上的概率
	void addToDecorationSupportProb(DecorationLabelType single_label);
	
	// optimization
	void SimulatedAnnealing();
	QMap<FurnitureType, ColorPalette*> furniture_color_config;
	// furniture color unary term
	double GetScore(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetScoreF1(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetScoreF2(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetAcceptRate(double F, double Fold, double T0, double deltaT, int k);
	QMap<FurnitureType, ClusterIndex> ChangeFurnitureColor(QMap<FurnitureType,ClusterIndex> furniture_colors);

	// result
	QMap<FurnitureType, ClusterIndex> furniture_color_indices;
	QList<QPair<DecorationType, double>> sorted_decoration_types;

	




		
};