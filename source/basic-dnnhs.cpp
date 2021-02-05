#include "basic-dnnhs.hpp"



HS::BasicDNNHSearch::BasicDNNHSearch() {

	m_bound = __DBL_MAX__;
}


void HS::BasicDNNHSearch::setData(
	const std::string dataPath, 
	const int         dataSize, 
	const int         dataDim) {

	assert(dataSize > 0);
	assert(dataDim > 0);

	/*** とても気持ちわるいコード ***/
	Eigen::MatrixXd d(dataSize, dataDim);
	HS::readData(&d, dataPath, dataSize, dataDim);
	
	m_data = std::vector<Point>(d.rows());
	for (int i=0; i<d.rows(); i++) {
		m_data[i] = Point(this, d.row(i));
	}

	m_n_dim = d.cols();
	m_distance = Eigen::MatrixXd::Constant(m_data.size(), m_data.size(), -1.0);
}


void HS::BasicDNNHSearch::setQuery(
	const Eigen::VectorXd& query) {

	m_query = query;
}


void HS::BasicDNNHSearch::run(
	const double alpha) {
	
	m_alpha = alpha;
	std::vector<int> ids_sorted(m_data.size());

    // 距離が近い順にソート	
    for (int i=0; i<m_data.size(); i++) { ids_sorted[i] = i; }
    std::sort(
        ids_sorted.begin(), ids_sorted.end(), [&](int a, int b) { 
		return m_data[a].distFromQuery() < m_data[b].distFromQuery(); 
	});

    // 各点について処理
    for (auto itr_id = ids_sorted.begin(); 
        itr_id != ids_sorted.end(); itr_id++) {
        int index = std::distance(ids_sorted.begin(), itr_id);

        // 所属グループを検索
        m_group.push_back(findGroup(index, ids_sorted));

        // 総合近似度を計算・フィルタリング
        if (m_group.back().delta() < m_result.delta()) {

			updateBound(m_group.back());
            m_result = m_group.back();

            filterPts(m_bound, &ids_sorted);
        }
    }
}


void HS::BasicDNNHSearch::printResult() {
	
	std::cout << std::endl;
	std::cout << "# Result:" << std::endl;
	for (int id : m_result.ids()) {
		std::cout << id << ", ";
	}
	std::cout << std::endl;
}


double HS::BasicDNNHSearch::distance(
	int id1, 
	int id2) {

	if (m_distance(id1, id2) < 0.0 && m_distance(id2, id1) < 0.0) {
		m_distance(id1, id2) = (m_data[id1].vec() - m_data[id2].vec()).norm();
		m_distance(id2, id1) = m_distance(id1, id2);
	}

	return m_distance(id1, id2);
}



int HS::BasicDNNHSearch::findNN(
	const Eigen::VectorXd& query,
	std::vector<int>      *ids,
	const bool 			   shouldDelete) {
	
	assert(ids->size() > 0);
	
	int    id_NN   = -1;
	double dist_NN = __DBL_MAX__;
	std::vector<int>::iterator itr_NN;

	for (auto itr_id = ids->begin(); itr_id != ids->end(); ) {
		double dist = ( m_data[*itr_id].vec()-query ).norm();
		if (dist < dist_NN) {
			id_NN   = *itr_id;
			dist_NN = dist;
			itr_NN  = itr_id;
		}
		itr_id++;
	}
	if (shouldDelete) { ids->erase(itr_NN); }

	return id_NN;
}


HS::BasicDNNHSearch::Group HS::BasicDNNHSearch::findGroup(
	const int               index, 
	const std::vector<int>& ids_data) {
	
	assert(ids_data.size() > 0);
    
	ExpansionGroup g_min;
    ExpansionGroup g(this, index, ids_data, false);

	g.expand();
	g_min = g;
	while (!g.unprocdIds().empty()) {
		// 拡大
        g.expand();
		if (g.unprocdIds().empty()) { break; } // while 文と一緒で気持ち悪い

        // epΔ を計算して最小のものを保持
		if (g.epDelta() < g_min.epDelta()) {
            g_min = g;
        }
	}

    return g_min;
}


void HS::BasicDNNHSearch::updateBound(Group& group) {
	m_bound = ( 2 * m_alpha / (m_alpha + 1) ) * group.delta();
}


void HS::BasicDNNHSearch::filterPts(
	const double      bound, 
	std::vector<int> *ids) {

	ids->erase(std::remove_if(ids->begin(), ids->end(),	[&](int id) { 
		return m_data[id].distFromQuery() > bound; }), ids->end()
	);
}


