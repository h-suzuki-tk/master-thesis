#include "dnnhs.hpp"


HS::DNNHSearch::DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query)
  : m_data(data)
  , m_query(query)
  , m_result(std::vector<int>(data.rows(), 0))
  {}

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
