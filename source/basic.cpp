#include "basic.hpp"

int DNNHSearch::basicSearch() {
	std::vector<int> ids_sorted(m_data.size());

    // 距離が近い順にソート	
    for (int i=0; i<m_data.size(); i++) { ids_sorted[i] = i; }
    std::sort(
        ids_sorted.begin(), ids_sorted.end(), [&](int a, int b) { 
		return m_data[a].distFromQuery() < m_data[b].distFromQuery(); 
	});

    // 各点について処理
	int id = ids_sorted.front();
	ids_sorted.erase(ids_sorted.begin());
    while (!ids_sorted.empty()) {

        // 所属グループを検索
        m_group.push_back(findGroup(id, ids_sorted));

        // 総合近似度を計算・フィルタリング
        if (m_group.back().delta() < m_result.delta()) {

			updateBound(m_group.back());
            m_result = m_group.back();

            filterPts(m_bound, &ids_sorted);
        }
		
		// 先頭を取り出す
		id = ids_sorted.front();
        ids_sorted.erase(ids_sorted.begin());
    }

    return 1;
}
