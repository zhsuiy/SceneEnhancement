#include "ProbLearning.h"
#include <QtCore/qdir.h>
#include "ColorPalette.h"
#include "ClusterMethods.h"
using namespace std;

ProbLearning::ProbLearning()
{
	m_para = Parameter::GetParameterInstance();
	m_adj_name = m_para->AdjName;
}

void ProbLearning::Learn()
{	
	// 1. process files
	ReadInfoFromLabels();

	// 2. do statistics
	// 2.1 single furniture color	
	CalculateFurnitureColorProb();
	
	// 2.2 pairwise furniture colors
	CalculateFurniturePairwiseColorProb();

	// 2.3 decoration mutual information
	CulculateDecorationProb();

	// 2.4 decoration and furniture color corelation


	// 3. optimization

}

void ProbLearning::ReadInfoFromLabels()
{
	// 1.1 furniture colors
	QString pathpos = m_para->LabelsPath + m_adj_name + "/pos";
	QString pathneg = m_para->LabelsPath + m_adj_name + "/neg";
	QVector<ImageFurnitureColorType> pos_colors = GetFurnitureColors(pathpos);
	QVector<ImageFurnitureColorType> neg_colors = GetFurnitureColors(pathneg);
	m_furniture_colors[0] = neg_colors;
	m_furniture_colors[1] = pos_colors;
	// 1.2 decoration info
	QVector<ImageDecorationType> pos_decorations = GetDecorations(pathpos);
	QVector<ImageDecorationType> neg_decorations = GetDecorations(pathneg);
	m_decorations[0] = neg_decorations;
	m_decorations[1] = pos_decorations;
}

QVector<ImageFurnitureColorType> ProbLearning::GetFurnitureColors(QString& path)
{
	QVector<ImageFurnitureColorType> list;
	QDir directory(path);
	if (!directory.exists())
		return list;
	QStringList names = Utility::GetFileNames(path);
	
	for (size_t i = 0; i < names.size(); i++)
	{
		QString imgpath = path + "/" + names[i];
		ImageFurnitureColorType furnitures = Utility::ReadImageFurnitureInfo(imgpath);
		list.push_back(furnitures);	
	}
	return list;
}

QVector<ImageDecorationType> ProbLearning::GetDecorations(QString& path)
{
	QVector<ImageDecorationType> list;
	QDir directory(path);
	if (!directory.exists())
		return list;
	QStringList names = Utility::GetFileNames(path);

	for (size_t i = 0; i < names.size(); i++)
	{
		QString imgpath = path + "/" + names[i];
		ImageDecorationType decorations = Utility::ReadImageDecorationInfo(imgpath);
		list.push_back(decorations);
	}
	return list;
}

void ProbLearning::CalculateFurnitureColorProb()
{
	m_furniture_types = m_para->FurnitureTypes;
	QMap<FurnitureType, QVector<ColorPalette*>> furniture_color_palettes;
	// 取出所有的正样本中的颜色
	QVector<ImageFurnitureColorType> furniture_colors = m_furniture_colors[1]; // 

	for (size_t i = 0; i < furniture_colors.size(); i++)
	{
		ImageFurnitureColorType map = furniture_colors[i];
		for (size_t j = 0; j < map.keys().size();j++)
		{
			if (!furniture_color_palettes.contains(map.keys()[j]))
			{
				QVector<ColorPalette*> palettes;
				palettes.push_back(map[map.keys()[j]]);
				furniture_color_palettes[map.keys()[j]] = palettes;
			}
			else
			{
				furniture_color_palettes[map.keys()[j]].push_back(map[map.keys()[j]]);
			}
		}
	}

	// 对每类家具聚类并统计概率
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		// 聚类
		QVector<ColorPalette*> colors = furniture_color_palettes[m_furniture_types[i]];
		vector<vector<int>> clusters = get_furniture_clusters(m_furniture_types[i],colors);	
		// 统计
		QMap<ClusterIndex, double> map;
		for (size_t j = 0; j < clusters.size(); j++)
		{
			map[j] = static_cast<double>(clusters[j].size()) / colors.size();
		}
		furniture_color_probs[m_furniture_types[i]] = map;
	}
}

vector<vector<int>> ProbLearning::get_furniture_clusters(FurnitureType furniture_type,QVector<ColorPalette*> colors)
{
	int color_num = colors.size();
	vector<vector<double>> distance_matrix(color_num, vector<double>(color_num, 0.0));
	for (size_t i = 0; i < color_num; i++)
	{
		for (size_t j = 0; j < color_num; j++)
		{
			double dis = ColorPalette::GetColorPaletteDistance(colors[i], colors[j]);
			distance_matrix[i][j] = dis;
			distance_matrix[j][i] = dis;
		}
	}
	ClusterMethods cluster_methods(distance_matrix, m_para->FurnitureClusterNum);
	vector<vector<int>> cluster_results = cluster_methods.getHierarchicalClusters(HC_AVG_DISTANCE);
	//vector<vector<int>> cluster_results = cluster_methods.getKMeansClusters();
	//vector<vector<int>> cluster_results = cluster_methods.getSpectralClusters(100,3,1);
	
	// 记录cluster	
	if (!furniture_color_clusters.contains(furniture_type))
	{
		QMap<ClusterIndex, QVector<ColorPalette*>> map;
		for (size_t i = 0; i < cluster_results.size(); i++)
		{
			QVector<ColorPalette*> colorpalettes;
			for (size_t j = 0; j < cluster_results[i].size(); j++)
			{
				// 记录每个颜色所属的cluster
				colors[cluster_results[i][j]]->ClusterIndex = i;
				// 记录每个cluster对应的颜色
				colorpalettes.push_back(colors[cluster_results[i][j]]);
			}
			map[i] = colorpalettes;
		}
		furniture_color_clusters[furniture_type] = map;
	}		
	return cluster_results;
}

void ProbLearning::CalculateFurniturePairwiseColorProb()
{
	QMap<QPair<FurnitureType, FurnitureType>, int> pairwise_num;
	//QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> furniture_pairwise_color_probs;
	QVector<ImageFurnitureColorType> pos_images = m_furniture_colors[1];
	m_furniture_types = m_para->FurnitureTypes;
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		for (size_t j = i + 1; j < m_furniture_types.size(); j++)
		{
			QMap<QPair<ClusterIndex, ClusterIndex>, double> map;
			for (size_t k = 0; k < m_para->FurnitureClusterNum; k++)
			{
				for (size_t w = 0; w < m_para->FurnitureClusterNum; w++)
				{
					map[QPair<ClusterIndex, ClusterIndex>(k, w)] = 0;
				}
			}
			pairwise_num[QPair<FurnitureType, FurnitureType>(m_furniture_types[i], m_furniture_types[j])] = 0;
			furniture_pairwise_color_probs[QPair<FurnitureType, FurnitureType>(m_furniture_types[i], m_furniture_types[j])]
				= map;
		}
	}
	for (size_t i = 0; i < pos_images.size(); i++)
	{
		QList<QPair<FurnitureType, FurnitureType>> keys = pairwise_num.keys();
		for (size_t j = 0; j < keys.size(); j++)
		{
			ImageFurnitureColorType colorlabels = pos_images[i];
			QList<FurnitureType> furniture_types = colorlabels.keys();
			if (furniture_types.contains(keys[j].first) && furniture_types.contains(keys[j].second))
			{
				// 同时有这两种家具的总数加一
				pairwise_num[keys[j]]++;
				int c1 = colorlabels[keys[j].first]->ClusterIndex; // cluster_index
				int c2 = colorlabels[keys[j].second]->ClusterIndex;
				furniture_pairwise_color_probs[keys[j]][QPair<ClusterIndex, ClusterIndex>(c1, c2)]++;
			}
		}
	}

	// normalization of frequency	
	QList<QPair<FurnitureType, FurnitureType>> keys = pairwise_num.keys();
	for (size_t j = 0; j < keys.size(); j++)
	{
		for (size_t k = 0; k < furniture_pairwise_color_probs[keys[j]].keys().size(); k++)
		{
			// cluster index pair
			auto key = furniture_pairwise_color_probs[keys[j]].keys()[k]; 
			furniture_pairwise_color_probs[keys[j]][key] = (furniture_pairwise_color_probs[keys[j]][key] + 0.1)/ (pairwise_num[keys[j]] + 0.1);
		}			
	}	
}

void ProbLearning::CulculateDecorationProb()
{
	m_decoration_types = m_para->DecorationTypes;
	QMap<DecorationType, int> neg_decoration_occurrence; // 记录每个小物件在负样本中出现的次数
	QMap<DecorationType, int> pos_decoration_occurrence; // 记录每个小物件在正样本中出现的次数
	for (size_t i = 0; i < m_decoration_types.size(); i++)
	{
		neg_decoration_occurrence[m_decoration_types[i]] = 0;
		pos_decoration_occurrence[m_decoration_types[i]] = 0;
		decoration_probs[m_decoration_types[i]] = 0.0;
	}
	QVector<ImageDecorationType> neg_decorations = m_decorations[0];
	QVector<ImageDecorationType> pos_decorations = m_decorations[1];

	// 第i张图片
	for (size_t i = 0; i < neg_decorations.size(); i++)
	{		
		// 第i张图片的第j个标注
		for (size_t j = 0; j < neg_decorations[i].size(); j++)
		{
			if (neg_decoration_occurrence.contains(neg_decorations[i][j].first)) // DecorationType
			{
				neg_decoration_occurrence[neg_decorations[i][j].first]++;
			}
		}		
	}
	for (size_t i = 0; i < pos_decorations.size(); i++)
	{
		for (size_t j = 0; j < pos_decorations[i].size(); j++)
		{
			if (pos_decoration_occurrence.contains(pos_decorations[i][j].first))
			{
				pos_decoration_occurrence[pos_decorations[i][j].first]++;
			}
		}
	}

	// calculate mutual information
	for (size_t i = 0; i < m_decoration_types.size(); i++)
	{
		double A = pos_decoration_occurrence[m_decoration_types[i]];
		double B = neg_decoration_occurrence[m_decoration_types[i]];
		double C = pos_decorations.size() - A;
		double N = pos_decorations.size() + neg_decorations.size();
		decoration_probs[m_decoration_types[i]] = log((A*N + 0.01) / ((A + C)*(A + B) + 0.01));
	}
}

void ProbLearning::SimulatedAnnealing()
{
	double E1 = 0;
	double E2 = 0;

	//
	//22
	QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	//QVector<FurnitureType> 




}
