#include "basic.hpp"

int HS::DNNHSearch::basicSearch() {

  // 距離が近い順にソート
  std::vector<std::pair<int, double>> que(m_data.rows());
  for (int i=0; i<que.size(); i++) {
    que[i] = std::make_pair(i, (m_data.row(i) - m_query.transpose()).norm());
  }
  std::sort(
    que.begin(), que.end(), 
    [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
      return a.second < b.second;
    });

  // 各点について処理
  std::vector<std::pair<std::int>> group(m_data.rows()); /***** これをメンバ変数にする  *****/
  double bound = __DBL_MAX__;
  int result = -1;
  while (!que.empty()) {
  
    //// 所属グループを検索
    findGroup(que[i].first, BASIC);

    //// 総合近似度を計算
    /* 最小のものを保持 */
    if (m_group[que[i].first].delta() < min_delta) {
      bound = m_group[que[i].first].delta();
      result = que[i].first; /***** m_group を可変長ベクトルにして core として入れちゃうのもあり *****/
    }
  }

  // 結果を格納


  return 1;
}