#include "point.hpp"

double HS::DNNHS::Basic::Point::distFromQuery() {

	if (m_dist_from_query < 0.0) {
		m_dist_from_query = (m_ds->m_query - vec()).norm();
	}

	return m_dist_from_query;
}
