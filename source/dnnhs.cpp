#include "dnnhs.hpp"

DNNHSearch::DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query) {
  m_data = data;
  if (query.size() == 0) {
    std::random_device seed;
    std::mt19937 rand(seed());
    std::uniform_real_distribution<> unif(0.0, 1.0);
    m_query = Eigen::VectorXd::Zero(m_data.cols()).unaryExpr([&](double dummy){return unif(rand);});
  } else {
    m_query = query;
  }
  m_result = std::vector<int>(data.rows(), 0);
}


void DNNHSearch::run(std::string clustWay) {
  int isOK = 0;

  if (clustWay == BASIC) {
    isOK = basicSearch();
  } else if (clustWay == GRID) {
    /*isOK = gridSearch();*/
  } else if (clustWay == SPLIT) {
    /*isOK = splitSearch();*/
  }

  if (isOK == 0) {
    std::cerr << "!Failed to run " << clustWay << " search!" << std::endl;
  }
}



int findNN(const Eigen::VectorXd &query, const std::vector<int> ids) {
  std::pair<int, double> pt_min = std::make_pair(-1, __DBL_MAX__);

  for (int id : ids) {
    double dist = (m_data.row(ids) - query.transpose()).norm();
    if (dist < pt_min.second) {
      pt_min.first = id;
      pt_min.second = dist;
    }
  }

  return pt_min.first;
}



std::vector<int> DNNHSearch::findGroup(const int id_core, std::vector<int> ids_data) {

  Group g_min;
  std::vector<Group> gs;
  
  for (auto id = ids_data.begin(); id != ids_data.end();) {

    // 最近傍点を見つける
    gs.back().add(findNN(gs.back().centroid(), ids_data));
    // epΔ を計算して最小のものを保持
    if (gs.back().epDelta() < g_min.epDelta()) { g_min = gs.back(); }
    // 更新
    gs.push_back(Group());

  }
  
  return g_min;
}
