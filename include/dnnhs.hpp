#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#define BASIC "basic"
#define GRID "grid"
#define SPLIT "split"

#include <iostream>
#include <vector>
#include <random>
#include <Eigen/Core>

#include "basic.hpp"

namespace HS {

  class DNNHSearch {
    public:
      DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query = Eigen::VectorXd(0));
      void run(std::string clustWay);
      Eigen::VectorXd query() { return m_query; };
    
    private:
      Eigen::MatrixXd  m_data;
      Eigen::VectorXd  m_query;
      std::vector<std::vector<int>> m_group;
      std::vector<int> m_result;

      int basicSearch();
      std::vector<int> findGroupBasic(const int id_core, std::vector<int> ids_data);
      Eigen::VectorXd center(const std::vector<int> &ids);
      //int gridSearch();
      //int splitSearch();
  };

}

#endif
