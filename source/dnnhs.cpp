#include "dnnhs.hpp"


HS::DNNHSearch::DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query) {
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

void HS::DNNHSearch::run(std::string clustWay) {
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


