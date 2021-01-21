#include "basic.hpp"

int HS::DNNHSearch::basicSearch() {

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
  for (auto p_id = ids_sorted.begin(); p_id != ids_sorted.end();) {
    ids_sorted.erase(p_id);

    // 所属グループを検索
    m_group.push_back(findGroupBasic(*p_id, ids_sorted));

    // 総合近似度を計算・フィルタリング


    p_id++;
  }

  // 結果を格納


  return 1;
}

std::vector<int> HS::DNNHSearch::findGroupBasic(const int id_core, std::vector<int> ids_data) {

  std::vector<int> group_min;
  std::vector<std::vector<int>> groups;
  
  Eigen::VectorXd c = m_data.row(id_core);
  for (auto id = ids_data.begin(); id != ids_data.end();) {

    // 最近傍点を見つける
    Eigen::VectorXd dist_c(ids_data.size());
    for (int i=0; i<dist_c.size(); i++) {
      dist_c[i] = (m_data.row(ids_data[i]) - c.transpose()).norm();
    }
    groups.back().push_back(ids_data[std::min_element(dist_c.data(), dist_c.data()+dist_c.size()) - dist_c.data()+dist_c.size()]);

    // epΔ を計算して最小のものを保持
    

    // 更新
    c = center(groups.back()); 
    groups.push_back(std::vector<int>());
  }
  
  return group_min;
}

Eigen::VectorXd HS::DNNHSearch::center(const std::vector<int> &ids) {
  Eigen::VectorXd c;

  /***** code *****/

  return c;
}
