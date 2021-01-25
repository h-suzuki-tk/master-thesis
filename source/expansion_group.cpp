#include "expansion_group.hpp"


DNNHSearch::ExpansionGroup::ExpansionGroup(
	DNNHSearch*             ds,
	const int 			    id, 
	const std::vector<int>& ids_data) : Group(ds, id) {

	m_ids_data = ids_data;
}


double DNNHSearch::ExpansionGroup::epDelta() {

	if (m_epDelta < 0.0) {
		double pdmean = pdSum() / pairs();
		double ndmean = ndSum() / size();
		m_epDelta = pdmean / (pdmean + ndmean);
	}

	return m_epDelta;
}


void DNNHSearch::ExpansionGroup::expand() {

	m_pd_sum += ndSum();
	m_nd_sum = -1.0;
	m_n_pair += size();
	add(m_ds->findNN(centroid(), &dataIds(), true));

}


int DNNHSearch::ExpansionGroup::nextId() {

	if (m_id_next < 0) {
		m_id_next = m_ds->findNN(centroid(), &dataIds(), true);
	}
	
	return m_id_next;
}


double DNNHSearch::ExpansionGroup::pdSum() {

	if (m_pd_sum < 0.0) {
		double pd_sum = 0.0;
		for (int i=0; i<size(); i++) {
			for (int j=i+1; j<size(); j++) {
				pd_sum += m_ds->distance(id(i), id(j));
			}
		}
		m_pd_sum = pd_sum;
	}

	return m_pd_sum;
}


double DNNHSearch::ExpansionGroup::ndSum() {

	if (m_nd_sum < 0.0) {
		double nd_sum = 0.0;
		for (int id : ids()) {
			nd_sum += m_ds->distance(id, nextId());
		}
		m_nd_sum = nd_sum;
	}

	return m_nd_sum;
}


int DNNHSearch::ExpansionGroup::pairs() {

	if (m_n_pair < 0) {
		m_n_pair = size() * (size()-1) / 2;
	}

	return m_n_pair;
}
