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
        itr_id != ids_sorted.end() && ids_sorted.size() > MIN_CLUSTER_SIZE;) {
		
		++m_procd_pt_count;

        int index = std::distance(ids_sorted.begin(), itr_id);

        // 所属グループを検索
		m_groups.emplace_back( findGroup( index, ids_sorted ) );

		// 発見グループの所属点を全体から削除
		ids_sorted.erase( std::remove_if( ids_sorted.begin(), ids_sorted.end(), [&](const int& pt) {
			return std::find( m_groups.back().ids().begin(), m_groups.back().ids().end(), pt ) != m_groups.back().ids().end();
		} ), ids_sorted.end() );

        // 総合近似度を計算・フィルタリング
        if (m_groups.back().delta() < m_result.delta()) {

			updateBound(m_groups.back());
            m_result = m_groups.back();

            filterPts(&ids_sorted);
        }

		// 更新
		itr_id = ids_sorted.begin();
    }

	return 0;
}





HS::DNNHS::Group HS::DNNHS::Basic::findGroup(
	const int        core_idx,
	std::vector<int> pts) {

	assert( pts.size() > MIN_CLUSTER_SIZE );
	
	ExpansionGroup cur_group( this, pts[core_idx] );
	ExpansionGroup best_group = cur_group;

	pts.erase( pts.begin() + core_idx );

	while ( pts.size() > 0 
		&& cur_group.size() < UPPER_CLUSTER_SIZE
		&& cur_group.sd() <= m_result.sd() ) {

		// 拡大点を見つける
		auto [nn_idx, nn_dist] = findNN( cur_group.centroid(), pts );
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

	return best_group;
}
