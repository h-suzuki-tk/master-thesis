#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#define BASIC "basic"
#define GRID "grid"
#define SPLIT "split"

#include <iostream>
#include <vector>
#include <Eigen/Core>

#include "basic.hpp"

namespace HS {

  class DNNHSearch {
    public:
      DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query);
      void run(std::string clustWay);
    
    private:
      Eigen::MatrixXd  m_data;
      Eigen::VectorXd  m_query;
      std::vector<int> m_result;

      int basicSearch();
      //int gridSearch();
      //int splitSearch();
  };

}

#endif
