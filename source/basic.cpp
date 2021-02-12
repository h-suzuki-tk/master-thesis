#include "basic.hpp"


HS::DNNHS::Basic::Basic(
	const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha) :
	DNNHS(data, query, alpha) {

	m_data_query_dist = Eigen::VectorXd::Constant(m_data.size(), -1.0);
}


int HS::DNNHS::Basic::run() {

    // 距離が近い順にソート	
	std::vector<int> ids_sorted(m_data.size());
    for (int i=0; i<m_data.size(); i++) { ids_sorted[i] = i; }
    std::sort(
        ids_sorted.begin(), ids_sorted.end(), [&](int a, int b) { 
		return distFromQuery(a) < distFromQuery(b); 
	});

    // 各点について処理
    for (auto itr_id = ids_sorted.begin(); 
        itr_id != ids_sorted.end(); itr_id++) {
        int index = std::distance(ids_sorted.begin(), itr_id);

        // 所属グループを検索
        m_groups.push_back(findGroup(index, ids_sorted));

        // 総合近似度を計算・フィルタリング
        if (m_groups.back().delta() < m_result.delta()) {

			updateBound(m_groups.back());
            m_result = m_groups.back();

            filterPts(&ids_sorted);
        }
    }

	return 0;
}


std::vector<int> HS::DNNHS::Basic::result() {
	assert(m_result.ids().size() > 0);
	return m_result.ids();
}


double HS::DNNHS::Basic::distFromQuery(
	const int id) {
	
	assert(id < m_data.size());
	if (m_data_query_dist(id) < 0.0 ) {
		m_data_query_dist(id) = ( m_data[id]-m_query ).norm();
	}
	return m_data_query_dist(id);
}


HS::DNNHS::Group HS::DNNHS::Basic::findGroup(
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
