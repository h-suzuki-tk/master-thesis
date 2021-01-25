#include "basic.hpp"

int DNNHSearch::basicSearch() {
	std::vector<int> ids_sorted(data().size());

    // 距離が近い順にソート	
    for (int i=0; i<data().size(); i++) { ids_sorted[i] = i; }
    std::sort(
        ids_sorted.begin(), ids_sorted.end(), [&](int a, int b) { 
		return data(a).distFromQuery() < data(b).distFromQuery(); 
	});

    // 各点について処理
    while (!ids_sorted.empty()) {

		// 先頭を取り出す
		int id = ids_sorted.front();
        ids_sorted.erase(ids_sorted.begin());

        // 所属グループを検索
        m_group.push_back(findGroup(id, ids_sorted));

        // 総合近似度を計算・フィルタリング
        if (m_group.back().delta() < m_result.delta()) {
            
			updateBound(m_group.back());
            m_result = m_group.back();

            filterPts(bound(), &ids_sorted);
        }
    }

    return 1;
}
