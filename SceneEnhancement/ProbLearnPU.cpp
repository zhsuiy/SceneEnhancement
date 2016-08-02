#include "ProbLearning.h"
#include <QtCore/qdir.h>
#include "ColorPalette.h"
#include "ClusterMethods.h"
#include <ctime>
#include <algorithm>
#include "VisualizationTool.h"

void ProbLearning::LearnPU(PUType put)
{
	m_islearned = false;
	//m_useMI = false;
	m_pu_type = put;
	m_energy_type = F1F2;
	

	// 2. do statistics
	// 2.0 cluster
	if (m_cluster_type != AllSample)
	{
		// 1. process files
		ReadInfoFromLabels();
		furniture_color_clusters.clear();
		ClusterFurnitureColors(true);
	}	

	// 2.1 single furniture color	
	CalculateFurnitureColorProbPU();

	// 2.2 pairwise furniture colors
	CalculateFurniturePairwiseColorProbPU();

	// 2.3 decoration mutual information
	CulculateDecorationProb();

	// 2.4 decoration and furniture color corelation


	// 3. optimization
	//SimulatedAnnealing();
	ConvexMaxProduct();
	//BruteForce();

	m_islearned = true;
	//QMap<FurnitureType,ColorPalette*> result = GetFurnitureColorPalette(1);
	//auto list = GetDecorationTypes(15);

}


void ProbLearning::CalculateFurnitureColorProbPU()
{	
	// 对每类家具聚类并统计概率
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		// 统计
		QMap<ClusterIndex, double> map;
		auto clustercolor = furniture_color_clusters[m_furniture_types[i]];

		QMapIterator<int, QVector<ColorPalette*>> it(clustercolor);
		double N = 0.0;
		while (it.hasNext())
		{
			it.next();
			N += it.value().size();
		}
		it.toFront();
		while (it.hasNext())
		{
			it.next();
			int clusterIndex = it.key();
			double P = 0.0, U = 0.0;
			double ngs = 0;
			auto colorpalettes = it.value();
			
			for (size_t j = 0; j < colorpalettes.size(); j++)
			{
				if (colorpalettes[j]->SampleType == Pos)
				{
					ngs++;
				}
			}			

			P = ngs / (N+1);
			U = ngs / (colorpalettes.size()+1);
			assert(!isnan(P));
			assert(!isnan(U));
			double score = 0.0;
			switch (m_pu_type)
			{
			case Prevalence:
				score = P;
				break;
			case Uniqueness:
				score = U;
				break;
			case PU:
				score = P*U;
				break;
			default:
				break;
			}
			map[clusterIndex] = score;
		}		
		furniture_color_probs[m_furniture_types[i]] = map;
	}
}


void ProbLearning::CalculateFurniturePairwiseColorProbPU()
{
	m_furniture_types = m_para->FurnitureTypes;
	// 记录每个cluster里有多少正样本
	QMap<FurnitureType, QMap<ClusterIndex, int>> positive_count;
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		auto cluster = furniture_color_clusters[m_furniture_types[i]];
		QMap<ClusterIndex, int> number;
		QMapIterator<int, QVector<ColorPalette*>> it(cluster);
		while (it.hasNext())
		{
			it.next();
			auto cp = it.value();
			int pos_n = 0;
			for (size_t j = 0; j < cp.size(); j++)
			{
				if (cp[j]->SampleType == Pos)
				{
					pos_n++;
				}
			}
			number[it.key()] = pos_n;
		}
		positive_count[m_furniture_types[i]] = number;
	}

	QMap<QPair<FurnitureType, FurnitureType>, int> pairwise_num;
	//QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> furniture_pairwise_color_probs;
	QVector<ImageFurnitureColorType> pos_images = m_furniture_colors[1];
	QVector<ImageFurnitureColorType> neg_images = m_furniture_colors[0];
	QVector<ImageFurnitureColorType> all_images;
	for (size_t i = 0; i < pos_images.size(); i++)
	{
		all_images.push_back(pos_images[i]);
	}
	for (size_t i = 0; i < neg_images.size(); i++)
	{
		all_images.push_back(neg_images[i]);
	}

	// c(f1,f2,g1,g2)
	QMap<QPair<FurnitureType, FurnitureType>, QMap<QPair<ClusterIndex, ClusterIndex>, double>> clustersize;


	// initialize
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		for (size_t j = i + 1; j < m_furniture_types.size(); j++)
		{
			QMap<QPair<ClusterIndex, ClusterIndex>, double> map;
			QMap<QPair<ClusterIndex, ClusterIndex>, double> mapcluster;

			for (size_t k = 0; k < m_para->FurnitureClusterNum; k++)
			{
				for (size_t w = 0; w < m_para->FurnitureClusterNum; w++)
				{
					map[QPair<ClusterIndex, ClusterIndex>(k, w)] = 0;
					mapcluster[QPair<ClusterIndex, ClusterIndex>(k, w)] = 0;
				}
			}
			pairwise_num[QPair<FurnitureType, FurnitureType>(m_furniture_types[i], m_furniture_types[j])] = 0;
			furniture_pairwise_color_probs[QPair<FurnitureType, FurnitureType>(m_furniture_types[i], m_furniture_types[j])]
				= map;

			// initialize
			clustersize[QPair<FurnitureType, FurnitureType>(m_furniture_types[i], m_furniture_types[j])] = mapcluster;
		}
	}

	
	for (size_t i = 0; i < all_images.size(); i++)
	{
		QList<QPair<FurnitureType, FurnitureType>> keys = pairwise_num.keys();
		for (size_t j = 0; j < keys.size(); j++)
		{
			ImageFurnitureColorType colorlabels = all_images[i];
			QList<FurnitureType> furniture_types = colorlabels.keys();
			if (furniture_types.contains(keys[j].first) && furniture_types.contains(keys[j].second))
			{
				// 同时有这两种家具的总数加一
				pairwise_num[keys[j]]++;
				int c1 = colorlabels[keys[j].first]->ClusterIndex; // cluster_index
				int c2 = colorlabels[keys[j].second]->ClusterIndex;
				if (colorlabels[keys[j].first]->SampleType == Pos &&
					colorlabels[keys[j].second]->SampleType == Pos)
				{
					furniture_pairwise_color_probs[keys[j]][QPair<ClusterIndex, ClusterIndex>(c1, c2)]++;
				}
				clustersize[keys[j]][QPair<ClusterIndex, ClusterIndex>(c1, c2)]++;				
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
			double f1f2g1g2s1 = furniture_pairwise_color_probs[keys[j]][key];
			double f1f2 = pairwise_num[keys[j]] + 1;
			double f1f2g1g2 = clustersize[keys[j]][key] + 1;
			double score = 0.0;
			double P = f1f2g1g2s1 / (f1f2 + 1);
			double U = f1f2g1g2s1 / (f1f2g1g2 + 1);
			switch (m_pu_type)
			{
			case Prevalence:
				score = P;
				break;
			case Uniqueness:
				score = U;
				break;
			case PU:
				score = P*U;
				break;
			default:
				break;
			}
			furniture_pairwise_color_probs[keys[j]][key] = score;
		}
	}
}