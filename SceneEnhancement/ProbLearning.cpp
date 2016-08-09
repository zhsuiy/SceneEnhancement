#include "ProbLearning.h"
#include <QtCore/qdir.h>
#include "ColorPalette.h"
#include "ClusterMethods.h"
#include <ctime>
#include <algorithm>
#include "VisualizationTool.h"
#include "factor_graph.hpp"

using namespace std;
using namespace pano;
ProbLearning::ProbLearning()
{
	m_para = Parameter::GetParameterInstance();
	m_assets = Assets::GetAssetsInstance();
	m_adj_name = m_para->AdjName;
	m_islearned = false;
	m_useMI = false;
}

void ProbLearning::Learn(EnergyType et)
{	
	m_islearned = false;
	m_useMI = false;
	m_energy_type = et;
	furniture_color_clusters.clear();

	// 1. process files
	ReadInfoFromLabels();

	// 2. do statistics
	// 2.1 single furniture color	
	CalculateFurnitureColorProb();
	
	// 2.2 pairwise furniture colors
	CalculateFurniturePairwiseColorProb();

	// 2.3 decoration mutual information
	CulculateDecorationProb();

	// 2.4 decoration pairwise information
	//CalculateDecorationPairwiseProb();

	// 2.4 decoration and furniture color corelation


	// 3. optimization
	SimulatedAnnealing();
	
	m_islearned = true;
	//QMap<FurnitureType,ColorPalette*> result = GetFurnitureColorPalette(1);
	//auto list = GetDecorationTypes(15);

}

bool ProbLearning::IsLearned() const
{
	return m_islearned;
}

void ProbLearning::SaveFurnitureClusterResult()
{
	if (m_islearned)
	{
		VisualizationTool::DrawAllFurnitureClusters(furniture_color_clusters);
	}
}

void ProbLearning::SaveFurnitureClusterResultInOrder()
{
	if (m_islearned)
	{
		VisualizationTool::DrawAllFurnitureClustersInOrder(furniture_color_clusters);
	}
}

void ProbLearning::ReadInfoFromLabels()
{
	// 1.1 furniture colors
	QString pathpos = m_para->LabelsPath + m_adj_name + "/pos";
	QString pathneg = m_para->LabelsPath + m_adj_name + "/neg";
	QVector<ImageFurnitureColorType> pos_colors = GetFurnitureColors(pathpos);
	QVector<ImageFurnitureColorType> neg_colors = GetFurnitureColors(pathneg);
	for (size_t i = 0; i < pos_colors.size(); i++)
	{
		auto map = pos_colors[i];
		QMapIterator<QString, ColorPalette*> it(map);
		while (it.hasNext())
		{
			it.next();
			map[it.key()]->SampleType = Pos;
		}
	}
	for (size_t i = 0; i < neg_colors.size(); i++)
	{
		auto map = neg_colors[i];
		QMapIterator<QString, ColorPalette*> it(map);
		while (it.hasNext())
		{
			it.next();
			map[it.key()]->SampleType = Neg;
		}
	}
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
	// ȡ�����е��������е���ɫ
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

	// ��ÿ��Ҿ߾��ಢͳ�Ƹ���
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		// ����
		QVector<ColorPalette*> colors = furniture_color_palettes[m_furniture_types[i]];
		vector<vector<int>> clusters = get_furniture_clusters(m_furniture_types[i],colors);	
		// ͳ��
		QMap<ClusterIndex, double> map;
		for (size_t j = 0; j < clusters.size(); j++)
		{
			map[j] = static_cast<double>(clusters[j].size()) / colors.size();
		}
		furniture_color_probs[m_furniture_types[i]] = map;
	}
}

void ProbLearning::ClusterFurnitureColors(bool useall)
{
	furniture_color_clusters.clear();
	if (useall)	
		m_cluster_type = AllSample;	
	else
		m_cluster_type = PosSample;

	m_furniture_types = m_para->FurnitureTypes;

	furniture_color_palettes.clear();

	QVector<ImageFurnitureColorType> furniture_colors;
	if (useall)
	{		
		int n_neg = m_furniture_colors[0].size();
		int n_pos = m_furniture_colors[1].size();
		for (size_t i = 0; i < n_neg; i++) // neg
		{
			furniture_colors.push_back(m_furniture_colors[0][i]);
		}
		for (size_t i = 0; i < n_pos; i++) // pos
		{
			furniture_colors.push_back(m_furniture_colors[1][i]);
		}
	}
	else
		furniture_colors = m_furniture_colors[1];


	for (size_t i = 0; i < furniture_colors.size(); i++)
	{
		ImageFurnitureColorType map = furniture_colors[i];
		for (size_t j = 0; j < map.keys().size(); j++)
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

	// ��ÿ��Ҿ߾��ಢͳ�Ƹ���
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		// ����
		QVector<ColorPalette*> colors = furniture_color_palettes[m_furniture_types[i]];
		vector<vector<int>> clusters = get_furniture_clusters(m_furniture_types[i], colors);		
	}
	// ��ÿ�������������
	reorder_cluster_results();
}

vector<vector<int>> ProbLearning::get_furniture_clusters(FurnitureType furniture_type,QVector<ColorPalette*> colors)
{	
	bool is_color_order_mattered = false;
	if (furniture_type.compare("Curtain",Qt::CaseInsensitive) == 0
		|| furniture_type.compare("BedSheet", Qt::CaseInsensitive) == 0
		|| furniture_type.compare("BedPillow", Qt::CaseInsensitive) == 0)
	{
		is_color_order_mattered = true;
	}
	int color_num = colors.size();
	vector<vector<double>> distance_matrix(color_num, vector<double>(color_num, 0.0));
	for (size_t i = 0; i < color_num; i++)
	{
		for (size_t j = 0; j < color_num; j++)
		{
			double dis = ColorPalette::GetColorPaletteDistance(colors[i], colors[j], is_color_order_mattered);
			distance_matrix[i][j] = dis;
			distance_matrix[j][i] = dis;
		}
	}
	ClusterMethods cluster_methods(distance_matrix, m_para->FurnitureClusterNum > color_num ? color_num : m_para->FurnitureClusterNum);
	//vector<vector<int>> cluster_results = cluster_methods.getHierarchicalClusters(HC_AVG_DISTANCE);
	//vector<vector<int>> cluster_results = cluster_methods.getHierarchicalClusters(HC_MAX_DISTANCE);
	//vector<vector<int>> cluster_results = cluster_methods.getHierarchicalClusters(HC_MIN_DISTANCE);

	vector<vector<int>> cluster_results = cluster_methods.getKMeansClusters();
	//vector<vector<int>> cluster_results = cluster_methods.getSpectralClusters(1000,color_num/4,1);
	
	// ��¼cluster	
	if (!furniture_color_clusters.contains(furniture_type))
	{
		QMap<ClusterIndex, QVector<ColorPalette*>> map;
		for (size_t i = 0; i < cluster_results.size(); i++)
		{
			QVector<ColorPalette*> colorpalettes;
			for (size_t j = 0; j < cluster_results[i].size(); j++)
			{
				// ��¼ÿ����ɫ������cluster
				colors[cluster_results[i][j]]->ClusterIndex = i;
				// ��¼ÿ��cluster��Ӧ����ɫ
				colorpalettes.push_back(colors[cluster_results[i][j]]);
			}
			map[i] = colorpalettes;			
		}
		furniture_color_clusters[furniture_type] = map;	
	}	
	return cluster_results;
}

void ProbLearning::reorder_cluster_results()
{
	furniture_color_clusters_ordered.clear();
	auto keys = furniture_color_clusters.keys();
	for (size_t k = 0; k < keys.size(); k++)
	{
		auto map = furniture_color_clusters[keys[k]];
		QMap<int, QVector<ColorPalette*>> ordered_map;
		QMapIterator<int, QVector<ColorPalette*>> it(map);
		while (it.hasNext())
		{
			it.next();
			QList<QPair<int, double>> distances;
			QVector<ColorPalette*> ordered_cps;
			auto cps = it.value();
			for (size_t i = 0; i < cps.size(); i++) // ��ǰ cluster
			{
				double d = 0;
				for (size_t j = 0; j < cps.size(); j++)
				{
					d += ColorPalette::GetColorPaletteDistance(cps[i], cps[j]);
				}
				distances.push_back(QPair<int, double>(i, d));
			}
			qSort(distances.begin(), distances.end(), Utility::QPairSecondComparerAscending());
			for (size_t i = 0; i < distances.size(); i++)
			{
				ordered_cps.push_back(cps[distances[i].first]);
			}
			ordered_map[it.key()] = ordered_cps;
		}
		furniture_color_clusters_ordered[keys[k]] = ordered_map;
	}
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
				// ͬʱ�������ּҾߵ�������һ
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
	QMap<DecorationType, int> neg_decoration_occurrence; // ��¼ÿ��С����ڸ������г��ֵĴ���
	QMap<DecorationType, int> pos_decoration_occurrence; // ��¼ÿ��С������������г��ֵĴ���
	decoration_support_probs.clear(); // С��������ڴ�Ҿߵĸ���
	for (size_t i = 0; i < m_decoration_types.size(); i++)
	{
		neg_decoration_occurrence[m_decoration_types[i]] = 0;
		pos_decoration_occurrence[m_decoration_types[i]] = 0;
		decoration_probs[m_decoration_types[i]] = 0.0;
	}
	QVector<ImageDecorationType> neg_decorations = m_decorations[0];
	QVector<ImageDecorationType> pos_decorations = m_decorations[1];

	// ��i��ͼƬ
	for (size_t i = 0; i < neg_decorations.size(); i++)
	{		
		// ��i��ͼƬ�ĵ�j����ע
		for (size_t j = 0; j < neg_decorations[i].size(); j++)
		{
			if (neg_decoration_occurrence.contains(neg_decorations[i][j].first)) // DecorationType
			{				
				neg_decoration_occurrence[neg_decorations[i][j].first]++;
				// ��ӵ�С����ʹ�ҾߵĹ�ϵ��
				addToDecorationSupportProb(neg_decorations[i][j]);				
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
				// ��ӵ�С����ʹ�ҾߵĹ�ϵ��
				addToDecorationSupportProb(pos_decorations[i][j]);
			}
		}
	}

	// ��С��������ڴ�Ҿ���ת���ɸ���
	QMapIterator<DecorationType,QMap<FurnitureType,double>> it(decoration_support_probs);
	while (it.hasNext())
	{
		it.next();
		auto map = it.value();
		QMapIterator<FurnitureType, double> it_inner(map);	
		double totalnum = pos_decoration_occurrence[it.key()] + neg_decoration_occurrence[it.key()];
		while (it_inner.hasNext())
		{
			it_inner.next();
			map[it_inner.key()] /= totalnum;
		}
		decoration_support_probs[it.key()] = map;
	}
	
	// calculate mutual information
	for (size_t i = 0; i < m_decoration_types.size(); i++)
	{
		double A = pos_decoration_occurrence[m_decoration_types[i]];
		double B = neg_decoration_occurrence[m_decoration_types[i]];
		double C = pos_decorations.size() - A;
		double D = neg_decorations.size() - B;
		double N = pos_decorations.size() + neg_decorations.size();
		double MI = log((A*N + 0.01) / ((A + C)*(A + B) + 0.01));
		//double CHI = N*(A*D - C*B)*(A*D - C*B) / ((A + C)*(B + D)*(A + B)*(C + D));
		// only use mutual information
		//decoration_probs[m_decoration_types[i]] = MI;

		// only use CHI
		//decoration_probs[m_decoration_types[i]] = CHI;
		
		double frenquency = (A + B + 0.1) / N;
		// multiply frequency
		//decoration_probs[m_decoration_types[i]] = 1 / (1 + exp(MI));
		decoration_probs[m_decoration_types[i]] = 1 / (1 + exp(-MI))*frenquency;

		
	}	

	sorted_decoration_types.clear();
	auto keys = decoration_probs.keys();
	for (size_t i = 0; i < keys.size(); i++)
	{
		sorted_decoration_types.push_back(QPair<DecorationType, double>(keys[i], decoration_probs[keys[i]]));
	}
	qSort(sorted_decoration_types.begin(), sorted_decoration_types.end(), Utility::QPairSecondComparer());	
}

void ProbLearning::addToDecorationSupportProb(DecorationLabelType cur_label)
{
	// ͳ��С��������ڴ������ϵĸ���	
	if (decoration_support_probs.contains(cur_label.first)) // ����С�����key
	{
		// ���д������key
		if (decoration_support_probs[cur_label.first].contains(cur_label.second.first))
		{
			decoration_support_probs[cur_label.first][cur_label.second.first]++;
		}
		else // �¼Ӵ������key
		{
			decoration_support_probs[cur_label.first][cur_label.second.first] = 1.0;
		}
	}
	else
	{
		QMap<FurnitureType, double> map;
		map[cur_label.second.first] = 1.0;
		decoration_support_probs[cur_label.first] = map;
	}
}

void ProbLearning::SimulatedAnnealing()
{
	srand(time(NULL));
	double lambda1 = 1 / 3.0, lambda2 = 1 / 3.0, lambda3 = 1 / 3.0;

	//
	//22
	QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	int n = current_furniture_models.size();
	QVector<FurnitureType> types;
	for (size_t i = 0; i < n; i++)
	{
		types.push_back(current_furniture_models[i]->Type);
	}

	// aim: to get QMap<FurnitureType, ColorPalette*>

	// 0. initialize, send random clusterIndex to furniture
	furniture_color_indices.clear();
	for (size_t i = 0; i < n; i++)
	{
		if (furniture_color_clusters[types[i]].keys().size() > 0)
		{
			int index = rand() % furniture_color_clusters[types[i]].keys().size();
			furniture_color_indices[types[i]] = furniture_color_clusters[types[i]].keys()[index];
		}
	}

	// 1. iterate	
	double F = GetScore(furniture_color_indices);
	double Fold = F;
	double min_F = INT_MAX;
	QMap<FurnitureType, ClusterIndex> min_energy_result;
		
	int k = 0;
	double T0 = -log(0.01);
	double deltaT = T0 / (20 * pow(n, 2));
	int max_k = (int)(T0 / deltaT);
	QFile file("./simulated_anealing.txt");
	QTextStream txtOutput(&file);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cout << "can not open simulated_anealing.txt" << std::endl;
		return;
	}
	while (k++ < max_k)
	{
		Fold = F;
		QMap<FurnitureType,ClusterIndex> tmpcolorconfig = ChangeFurnitureColor(furniture_color_indices);
		F = GetScore(tmpcolorconfig);
		double accept_rate = GetAcceptRate(F, Fold, T0, deltaT, k);
		if ((static_cast<double>(rand()) / (RAND_MAX)) < accept_rate) // accetpted
		{
			furniture_color_indices = tmpcolorconfig;
		}
		else // if not, keep F unchanged
		{
			F = Fold;
		}		
		if (k%10 == 0)
		{
			txtOutput << F << "\n";
		}
	}

	txtOutput << "final F = " << F << "\n";
	QMapIterator<QString, int> it(furniture_color_indices);
	while (it.hasNext())
	{
		it.next();
		txtOutput << it.key() << " " << it.value() << "\n";
	}
	file.close();

	// convert cluster index to colorpalette
}

void ProbLearning::ConvexMaxProduct()
{
	QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	int n = current_furniture_models.size();
	QVector<FurnitureType> types;
	
	for (size_t i = 0; i < n; i++)
	{
		types.push_back(current_furniture_models[i]->Type);
	}

	int var_num = types.size(); // number of variables
	int label_num = m_para->FurnitureClusterNum;
	core::FactorGraph fg;	
	auto vcid = fg.addVarCategory(label_num, 1.0);

	auto furniture_unary_probs = furniture_color_probs;
	for (size_t f = 0; f < var_num; f++)
	{
		auto fcid = fg.addFactorCategory(
			
			[f, furniture_unary_probs,types,n](const std::vector<int> &labels) -> double {
			double e = -0.5*log(furniture_unary_probs[types[f]][labels[0]] + 0.01) / n;
			assert(e >= 0);
			return e;
		},
			1.0);

		auto vh = fg.addVar(vcid);
		auto fh = fg.addFactor(fcid, { vh });
	}

	auto furniture_binary_probs = furniture_pairwise_color_probs;

	for (int f1 = 0; f1 < var_num; f1++)
	{
		for (int f2 = 0; f2 < var_num; f2++)
		{
			if (furniture_binary_probs.contains(QPair<FurnitureType, FurnitureType>(types[f1], types[f2])))
			{
				auto fcid = fg.addFactorCategory(
					[f1, f2, furniture_binary_probs, types, n](const std::vector<int> &labels) -> double {
						double e = -0.5*log(furniture_binary_probs[QPair<FurnitureType, FurnitureType>(types[f1], types[f2])]
							[QPair<ClusterIndex, ClusterIndex>(labels[0], labels[1])] + 0.01)*2.0 / (n*(n - 1));
						assert(e >= 0);
						return e;

				},
					1.0);
				auto fh = fg.addFactor(fcid, { f1,f2 });
			}				
		}	
	}	

	auto results = fg.solve(100, 1, [](int epoch, double energy) {
		std::cout << epoch << "energy: " << energy << std::endl;
		return true;
	});

	furniture_color_indices.clear();
	for (size_t t = 0; t < types.size(); t++)
	{
		furniture_color_indices[types[t]] = results[t];
	}

//	ASSERT(results[vh] == 1);

}

void ProbLearning::ConvexMaxProductDecorations()
{
	//QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	
	//QVector<FurnitureType> types;
	auto types = m_para->DecorationTypes;
	int n = m_decoration_types.size();
	int var_num = n;	
	int label_num = 2; // 1 for presence, 0 for absence
	core::FactorGraph fg;
	auto vcid = fg.addVarCategory(label_num, 1.0);

	auto decoration_unary_prob = decoration_probs_pu;
	for (size_t d = 0; d < var_num; d++)
	{
		auto fcid = fg.addFactorCategory(

			[d, decoration_unary_prob, types, n](const std::vector<int> &labels) -> double {
			//double e = -0.5*log(decoration_unary_prob[types[d]][labels[0]] + 0.000000001);
			double e = 0.1;
			assert(e >= 0);
			return e;
		},
			1.0);
		auto vh = fg.addVar(vcid);
		auto fh = fg.addFactor(fcid, { vh });
	}

	auto decoration_binary_probs = decoration_pairwise_probs_pu;

	for (int d1 = 0; d1 < var_num; d1++)
	{
		for (int d2 = 0; d2 < var_num; d2++)
		{
			if (decoration_binary_probs.contains(QPair<DecorationType, DecorationType>(types[d1], types[d2])))
			{
				auto fcid = fg.addFactorCategory(
					[d1, d2, decoration_binary_probs, types, n](const std::vector<int> &labels) -> double {
					double e = -0.5*log(decoration_binary_probs[QPair<DecorationType, DecorationType>(types[d1], types[d2])]
						[QPair<int, int>(labels[0], labels[1])] + 0.000000001);
					assert(e >= 0);
					return e;

				},
					1.0);
				auto fh = fg.addFactor(fcid, { d1,d2 });
			}
		}
	}

	QList<QPair<std::vector<int>,double>> all_results;
	auto results = fg.solve(50, 1, [&all_results](int epoch, double energy, double denergy,
		const std::vector<int> &cur_best_var_labels) {
		all_results.push_back(QPair<std::vector<int>, double>(cur_best_var_labels, energy));
		std::cout << epoch << " energy: " << energy << std::endl;
		return true;
	});

	qSort(all_results.begin(), all_results.end(), Utility::QPairSecondComparerAscending());

	QVector<QMap<DecorationType, int>> all_decoration_presence;
	for (size_t i = 0; i < all_results.size(); i++)
	{
		QMap<DecorationType, int> presence;
		for (size_t t = 0; t < types.size(); t++)
		{
			presence[types[t]] = all_results[i].first[t];
		}
		all_decoration_presence.push_back(presence);
	}

	/*decoration_presence.clear();
	for (size_t t = 0; t < types.size(); t++)
	{
		decoration_presence[types[t]] = results[t];
	}*/
}

void ProbLearning::BruteForce()
{
	srand(time(NULL));

	//
	//22
	QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	int n = current_furniture_models.size();
	QVector<FurnitureType> types;
	for (size_t i = 0; i < n; i++)
	{
		types.push_back(current_furniture_models[i]->Type);
	}

	//QList<QMap<QString, int>> all_indices;
	QList<QPair<QMap<QString, int>, double>> index_score;
	int clusternum = m_para->FurnitureClusterNum;
	unsigned int totalcomp = pow(clusternum, types.size());
	for (size_t i = 0; i < totalcomp; i++)
	{
		QMap<QString, int> map;
		for (size_t t = 0; t < types.size(); t++)
		{
			map[types[t]] = (int)(i / (pow(clusternum, t))) % clusternum;
		}		
		double F = GetScore(map);
		index_score.push_back(QPair<QMap<QString, int>, double>(map, F));
	}

	qSort(index_score.begin(), index_score.end(), Utility::QPairSecondComparerAscending());

	furniture_color_indices = index_score[0].first;


	QFile file("./bruteforce.txt");
	QTextStream txtOutput(&file);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cout << "can not open simulated_anealing.txt" << std::endl;
		return;
	}
	txtOutput << "final F = " << index_score[0].second << "\n";
	QMapIterator<QString, int> it(furniture_color_indices);
	while (it.hasNext())
	{
		it.next();
		txtOutput << it.key() << " " << it.value() << "\n";
	}
	file.close();
	
}

double ProbLearning::GetScore(QMap<QString, ClusterIndex> furniture_colors)
{
	double score = 0.0;
	switch (m_energy_type)
	{
	case F1:
		score = GetScoreF1(furniture_colors);
		break;
	case F2:
		score = GetScoreF2(furniture_colors);
		break;
	case F1F2:
		score = 0.5*GetScoreF1(furniture_colors) + 0.5*GetScoreF2(furniture_colors);
		break;
	default:
		break;
	}
	return score;
}

double ProbLearning::GetScoreF1(QMap<FurnitureType, ClusterIndex> furniture_colors)
{
	double score = 0.0;
	QMapIterator<FurnitureType, ClusterIndex> it(furniture_colors);
	while (it.hasNext()) 
	{
		it.next();
		score += log(furniture_color_probs[it.key()][it.value()] + 0.01);
	}
	score = - 1.0 / furniture_colors.size() * score;	
	return score;
}

double ProbLearning::GetScoreF2(QMap<FurnitureType, ClusterIndex> furniture_colors)
{
	double score = 0.0;
	QList<FurnitureType> all_types = furniture_colors.keys();
	int n = all_types.size();
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			int ci = furniture_colors[all_types[i]];
			int cj = furniture_colors[all_types[j]];
			if (furniture_pairwise_color_probs.contains(QPair<FurnitureType, FurnitureType>(all_types[i], all_types[j])))
			{
				score += log(furniture_pairwise_color_probs[QPair<FurnitureType, FurnitureType>(all_types[i], all_types[j])]
					[QPair<ClusterIndex, ClusterIndex>(ci, cj)] + 0.01);
			}			
		}		
	}
	score = -2.0 / (n*(n - 1))*score;
	return score;
}

double ProbLearning::GetAcceptRate(double F, double Fold, double T0, double deltaT, int k)
{
	return qMin(1.0, exp(-(F - Fold) / (T0 - deltaT*k)));
}

QMap<QString, ClusterIndex> ProbLearning::ChangeFurnitureColor(QMap<QString, ClusterIndex> furniture_colors)
{
	QMap<QString, ClusterIndex> map;
	QMapIterator<FurnitureType, ClusterIndex> it(furniture_colors);
	while (it.hasNext())
	{
		it.next();
		map[it.key()] = it.value();
	}
	// randomly choose a furniture
	int index = rand() % map.size();
	int new_cluster = 0;
	FurnitureType furniture_type = map.keys()[index];
	// choose a new cluster index for this furniture
	while (1)
	{
		new_cluster = rand() % furniture_color_clusters[furniture_type].keys().size();
		if (new_cluster != furniture_colors[furniture_type])
			break;		
	}
	map[furniture_type] = new_cluster;
	return map;
}

QMap<FurnitureType, ColorPalette*> ProbLearning::GetFurnitureColorPalette(int level = 0)
{
	QMap<FurnitureType, ColorPalette*> map;
	QMapIterator<FurnitureType, ClusterIndex> it(furniture_color_indices);
	
	while (it.hasNext())
	{
		it.next();
		// the colorpalette num in this cluster
		//auto all_cp = furniture_color_clusters[it.key()][it.value()];
		auto all_cp = furniture_color_clusters_ordered[it.key()][it.value()];
		if (all_cp.size() == 0)
		{
			qWarning("The %d th cluster for %s is empty", it.value(), it.key().toStdString().c_str());
			continue;
		}
		//int num = all_cp.size();
		int num = all_cp.size() / 3 + 1;
		// randomly choose a colorpalette from that cluster
		/*vector<ColorPalette*> pos_cps;
		for (size_t i = 0; i < all_cp.size(); i++)
		{
			if (all_cp[i]->SampleType == Pos)
			{
				pos_cps.push_back(all_cp[i]);
			}
		}
		int num = pos_cps.size();*/
		ColorPalette* cp;
		if (all_cp.size() > 0)
		{
			cp = all_cp[0];
		}			
		if (num > 0) // only select from positive samples
		{
			//cp = pos_cps[rand() % num];
			cp = all_cp[rand() % num];
			map[it.key()] = cp;
		}
		else
		{
			qWarning("The %d th cluster for %s is empty", it.value(), it.key().toStdString().c_str());
		}			
	}	
	return map;
}

QMap<QString, ColorPalette*> ProbLearning::GetFurnitureColorPaletteRandom()
{
	QMap<FurnitureType, ColorPalette*> map;
	if (furniture_color_palettes.size() > 0)
	{
		QMapIterator<FurnitureType, QVector<ColorPalette*>> it(furniture_color_palettes);
		while (it.hasNext())
		{
			it.next();
			// all colorpalette of this furniture type
			auto all_cp = it.value();
			int num = all_cp.size();
			// randomly choose a colorpalette from that cluster
			/*vector<ColorPalette*> pos_cps;
			for (size_t i = 0; i < all_cp.size(); i++)
			{
				if (all_cp[i]->SampleType == Pos)
				{
					pos_cps.push_back(all_cp[i]);
				}
			}
			int num = pos_cps.size();*/
			ColorPalette* cp;
			if (all_cp.size() > 0)
			{
				cp = all_cp[0];
			}
			if (num > 0) // only select from positive samples
			{
				//cp = pos_cps[rand() % num];
				cp = all_cp[rand() % num];
				map[it.key()] = cp;
			}
			else
			{
				qWarning("The %d th cluster for %s is empty", it.value(), it.key().toStdString().c_str());
			}
		}
	}	
	return map;
}

QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> ProbLearning::GetDecorationTypes(int n)
{
	QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> list;
	n = n > sorted_decoration_types.size() ? sorted_decoration_types.size() : n;
	for (size_t i = 0; i < n; i++)
	{
		DecorationType dt = sorted_decoration_types[i].first;
		if (decoration_support_probs.contains(dt))
		{
			QPair<DecorationType, QList<QPair<FurnitureType, double>>> pair;
			pair.first = dt;
			auto innerlist = Utility::QMap2QList(decoration_support_probs[dt]);
			qSort(innerlist.begin(), innerlist.end(), Utility::QPairSecondComparer());
			pair.second = innerlist;
			list.push_back(pair);
		}
	}
	return list;
}

QList<QPair<QString, QList<QPair<QString, double>>>> ProbLearning::GetDecorationTypesRandom(int n)
{
	QList<QPair<DecorationType, QList<QPair<FurnitureType, double>>>> list;
	n = n > sorted_decoration_types.size() ? sorted_decoration_types.size() : n;
	
	// copy list;
	for (size_t i = 0; i < sorted_decoration_types.size(); i++)
	{
		DecorationType dt = sorted_decoration_types[i].first;
		if (decoration_support_probs.contains(dt))
		{
			QPair<DecorationType, QList<QPair<FurnitureType, double>>> pair;
			pair.first = dt;
			auto innerlist = Utility::QMap2QList(decoration_support_probs[dt]);
			qSort(innerlist.begin(), innerlist.end(), Utility::QPairSecondComparer());			
			pair.second = innerlist;
			list.push_back(pair);
		}
	}

	// random remove
	int m = list.size() - n;
	for (size_t i = 0; i < m; i++)
	{
		list.removeAt(rand() % list.size());
	}
	return list;
}

void ProbLearning::LearnMI()
{
	m_islearned = false;
	m_useMI = true;
	furniture_color_clusters.clear();
	// 1. process files
	ReadInfoFromLabels();

	// 2. do statistics
	// 2.1 single furniture color	
	CalculateFunirtureColorMI();

	// 2.2 pairwise furniture colors
	//CalculateFurniturePairwiseColorProb();

	// 2.3 decoration mutual information
	//CulculateDecorationProb();

	// 2.4 decoration and furniture color corelation


	// 3. optimization
	//SimulatedAnnealing();
	srand(time(NULL));	

	QVector<FurnitureModel*> current_furniture_models = m_assets->GetFurnitureModels();
	int n = current_furniture_models.size();
	QVector<FurnitureType> types;
	for (size_t i = 0; i < n; i++)
	{
		types.push_back(current_furniture_models[i]->Type);
	}

	// aim: to get QMap<FurnitureType, ColorPalette*>

	// 0. initialize, send random clusterIndex to furniture
	furniture_color_indices.clear();
	for (size_t i = 0; i < n; i++)
	{
		auto map = furniture_color_probs[types[i]]; // ȡ������cluster index
		auto keys = map.keys();
		QList<QPair<ClusterIndex, double>> sorted_clusters;
		for (size_t j = 0; j < keys.size(); j++)
		{
			sorted_clusters.push_back(QPair<ClusterIndex, double>(keys[j], map[keys[j]]));
		}
		qSort(sorted_clusters.begin(), sorted_clusters.end(), Utility::QPairSecondComparer());
		int index = sorted_clusters[0].first;	
		furniture_color_indices[types[i]] = index;
	}

	m_islearned = true;
	//QMap<FurnitureType,ColorPalette*> result = GetFurnitureColorPalette(1);
	auto list = GetDecorationTypes(15);
}


void ProbLearning::CalculateFunirtureColorMI()
{
	m_furniture_types = m_para->FurnitureTypes;
	QMap<FurnitureType, QVector<ColorPalette*>> furniture_color_palettes;
	// ȡ�����������е���ɫ
	QVector<ImageFurnitureColorType> all_furniture_colors;
	int n_neg = m_furniture_colors[0].size();
	int n_pos = m_furniture_colors[1].size();
	for (size_t i = 0; i < n_neg; i++) // neg
	{
		all_furniture_colors.push_back(m_furniture_colors[0][i]);
	}
	for (size_t i = 0; i < n_pos; i++) // pos
	{
		all_furniture_colors.push_back(m_furniture_colors[1][i]);
	}

	for (size_t i = 0; i < all_furniture_colors.size(); i++)
	{
		ImageFurnitureColorType map = all_furniture_colors[i];
		for (size_t j = 0; j < map.keys().size(); j++)
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

	// ��ÿ��Ҿ߾��ಢͳ�Ƹ���
	for (size_t i = 0; i < m_furniture_types.size(); i++)
	{
		// ����
		QVector<ColorPalette*> colors = furniture_color_palettes[m_furniture_types[i]];
		vector<vector<int>> clusters = get_furniture_clusters(m_furniture_types[i], colors);
		// ͳ��
		QMap<ClusterIndex, double> map;
		for (size_t j = 0; j < clusters.size(); j++)
		{
			double N = colors.size(); // �ܹ�����ɫ����
			double A = 0.0, B = 0.0;
			for (size_t k = 0; k < clusters[j].size(); k++) // ͳ�����cluster��������������
			{
				if (colors[clusters[j][k]]->SampleType == Neg) // �ж��Ƿ��Ǹ�����				
					B++;				
				else
					A++;
			}
			double C = (double)n_pos - A;
			double MI = log((A*N + 0.01) / ((A + C)*(A + B) + 0.01));
			map[j] = 1 / (1 + exp(-MI));
		}
		furniture_color_probs[m_furniture_types[i]] = map;
	}
}