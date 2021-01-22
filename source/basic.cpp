#include "basic.hpp"

int DNNHSearch::basicSearch() {

  // 距離が近い順にソート
  std::vector<int> ids_sorted(m_data.rows());
  for (int i=0; i<ids_sorted.size(); i++) { ids_sorted[i] = i; }
  
  Eigen::VectorXd dist_query = (m_data.rowwise() - m_query.transpose()).rowwise().norm();
  std::sort(
    ids_sorted.begin(), ids_sorted.end(),
    [&](double a, double b) { return dist_query(a) < dist_query(b); }
  );

  // 各点について処理
  double bound = __DBL_MAX__;
  Group *g_min;
  for (auto p_id = ids_sorted.begin(); p_id != ids_sorted.end();) {
    ids_sorted.erase(p_id);

    // 所属グループを検索
    m_group.push_back(findGroup(*p_id, ids_sorted));

    // 総合近似度を計算・フィルタリング
    if (m_group.back().delta() < bound) {
      bound = m_group.back().delta();
      g_min = m_group.back();
      //filterPts(bound, &ids_sorted, dist_query); /*****/
    }

    p_id++;
  }

  // 結果を格納


  return 1;
}
