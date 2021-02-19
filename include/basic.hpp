#ifndef _BASIC_HPP_
#define _BASIC_HPP_

#include <iostream>
#include <cassert>
#include <vector>
#include <Eigen/Core>
#include "dnnhs.hpp"

namespace HS::DNNHS {
class Basic : public DNNHS {

    public:
      	Basic(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha);
      	int              run();

    private:
		Eigen::VectorXd    m_data_query_dist;
		double distFromQuery(const int id);
		Group  findGroup    (const int id_core, const std::vector<int>& ids_data);

};
}

#endif
