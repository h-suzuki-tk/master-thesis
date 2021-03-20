#include "basic.hpp"





HS::DNNHS::Basic::Basic(
	const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha) :
	DNNHS( data, query, alpha ),
	m_procd_pt_count( 0 ) {
}





int HS::DNNHS::Basic::run() {

	// 距離が近い順にソート	
	std::vector<int> ids_sorted(m_data.size());
    for (int i=0; i<m_data.size(); i++) { ids_sorted[i] = i; }
    std::sort(
        ids_sorted.begin(), ids_sorted.end(), [&](int a, int b) { 
		return fromQueryDist(a) < fromQueryDist(b); 
	});

	int procd_time = 0;
    // 各点について処理
    for (auto itr_id = ids_sorted.begin(); 
        itr_id != ids_sorted.end(); itr_id++) {
		
		++m_procd_pt_count;

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





HS::DNNHS::Group HS::DNNHS::Basic::findGroup(
	const int        core_idx,
	std::vector<int> pts) {
	
	ExpansionGroup cur_group( this, pts[core_idx] );
	ExpansionGroup best_group;

	pts.erase( pts.begin() + core_idx );

	//int ep_time = 0;
	while ( pts.size() > 0 ) {
		
		/*
		if ( ++ep_time % 10000 == 0 ) {
			std::cout << "ep_time: " << ep_time << std::endl;
		}
		*/

		// 拡大点を見つける
		auto [nn_idx, nn_dist] = newFindNN( cur_group.centroid(), pts );
		cur_group.setNextPt( pts[nn_idx] );
	
		// 拡大指標の計算
		if ( cur_group.epd() < best_group.epd() ) {
			best_group = cur_group;
		}

		// 拡大
		cur_group.expand();

		// 更新
		pts.erase( pts.begin() + nn_idx );

	}
	//std::cout << "total ep time: " << ep_time << std::endl;

	return best_group;
}
