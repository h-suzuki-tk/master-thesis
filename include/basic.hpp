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
      	int run();
		
		int procdPtCount() const { return m_procd_pt_count; }

    private:		
		int m_procd_pt_count;
		Group findGroup (const int core_pt, std::vector<int> pts);
};
}

#endif
