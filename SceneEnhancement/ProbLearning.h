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
enum PUType
{
	Prevalence,
	Uniqueness,
	PU
};
enum ClusterType
{	
	PosSample,
	AllSample
};


class ProbLearning : public QObject
{
public:
	ProbLearning();
	void Learn(EnergyType et);
	void LearnPU(PUType put);
	void LearnSmallObjects();
	void LearnMI();	
	bool IsLearned() const;
	void SaveFurnitureClusterResult();
	void SaveFurnitureClusterResultInOrder(); // 按照距离聚类中心的距离排序保存
	void ClusterFurnitureColors(bool useall = false); // set furniture_color_clusters

	QMap<FurnitureType, ColorPalette*> GetFurnitureColorPalette(int level);
	QMap<FurnitureType, ColorPalette*> GetFurnitureColorPaletteRandom();
	QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> GetDecorationTypes(int n);
	QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> GetDecorationTypesRandom(int n);
private:
	//tool
	
	// state
	bool m_islearned;
	EnergyType m_energy_type;
	PUType m_pu_type;
	bool m_useMI;

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
	void CalculateFurnitureColorProbPU();
	QVector<FurnitureType> m_furniture_types;
	QMap<FurnitureType, QMap<ClusterIndex, QColor>> furniture_color_index;
	QMap<FurnitureType, QMap<ClusterIndex, double>> furniture_color_probs; // in use	
	// cluster results	
	QMap<FurnitureType, QMap<ClusterIndex, QVector<ColorPalette*>>> furniture_color_clusters;
	QMap<FurnitureType, QMap<ClusterIndex, QVector<ColorPalette*>>> furniture_color_clusters_ordered;
	QMap<FurnitureType, QVector<ColorPalette*>> furniture_color_palettes;
	vector<vector<int>> get_furniture_clusters(FurnitureType furniture_type, QVector<ColorPalette*> colors);
	void reorder_cluster_results();
	ClusterType m_cluster_type;

	void CalculateFurniturePairwiseColorProb();
	void CalculateFurniturePairwiseColorProbPU();
	QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> furniture_pairwise_color_probs;

	// use mutual information to calculate colors
	void CalculateFunirtureColorMI();

	// decorations
	void CalculateDecorationProbAll();
	void CalculateDecorationPairwiseProbAll();
	void CulculateDecorationProb();
	void CalculateDecorationProbPU();
	void CalculateDecorationPairwiseProbPU();
	
	QVector<DecorationType> m_decoration_types;
	QMap<DecorationType, double> decoration_probs;
	QMap<DecorationType, QMap<int, double>> decoration_probs_pu;
	QMap<QPair<DecorationType, DecorationType>, QMap<QPair<int, int>, double>> decoration_pairwise_probs_pu;
	QMap<DecorationType, QMap<FurnitureType, double>> decoration_support_probs; // 小物件出现在各个大家具上的概率
	void addToDecorationSupportProb(DecorationLabelType single_label);
	
	// optimization
	void SimulatedAnnealing();
	void ConvexMaxProduct();
	void ConvexMaxProductDecorations();
	void BruteForce();
	QMap<FurnitureType, ColorPalette*> furniture_color_config;
	// furniture color unary term
	double GetScore(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetScoreF1(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetScoreF2(QMap<FurnitureType, ClusterIndex> furniture_colors);
	double GetAcceptRate(double F, double Fold, double T0, double deltaT, int k);
	QMap<FurnitureType, ClusterIndex> ChangeFurnitureColor(QMap<FurnitureType,ClusterIndex> furniture_colors);

	// result
	QMap<FurnitureType, ClusterIndex> furniture_color_indices;
	QMap<DecorationType, int> decoration_presence;
	QList<QPair<DecorationType, double>> sorted_decoration_types;
	

		
};