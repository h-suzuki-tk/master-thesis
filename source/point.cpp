#include "point.hpp"

double DNNHSearch::Point::distFromQuery() {

	if (m_dist_from_query < 0.0) {
		m_dist_from_query = (m_ds->query() - vec()).norm();
	}

	return m_dist_from_query;
}
