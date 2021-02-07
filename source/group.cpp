#include "group.hpp"

Eigen::VectorXd& HS::DNNHS::Basic::Group::centroid() {

    if (m_centroid.size() == 0) {
        Eigen::VectorXd temp;

		temp = Eigen::VectorXd::Zero(m_ds->m_n_dim);
        for (int id : m_ids) {
            temp += m_ds->m_data[id].vec();
        }
        m_centroid = temp / size();
    }

    return m_centroid;
}


double HS::DNNHS::Basic::Group::dist(const Eigen::VectorXd &v) {
    return (centroid() - v).norm();
}


double HS::DNNHS::Basic::Group::sd() {
    double sd;

    double temp = 0.0;
    for (int id : m_ids) {
        temp += (m_ds->m_data[id].vec() - centroid()).squaredNorm();
    }
    sd = sqrt(temp / size());

    return sd;
}


double HS::DNNHS::Basic::Group::delta() {

	if (m_ids.size() == 0) {
		m_delta = __DBL_MAX__;
	} else if (m_delta < 0.0) {
		m_delta = dist(m_ds->m_query) + sd();
	}

    return m_delta;
}
