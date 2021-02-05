#include "expansion_group.hpp"


HS::BasicDNNHSearch::ExpansionGroup::ExpansionGroup(
	HS::BasicDNNHSearch*             ds,
	const int 			    index_core, 
	const std::vector<int>& ids_data,
	bool                    isCorePruned) : Group(ds, ids_data[index_core]) {

	m_ids_unprocd = ids_data;
	m_pd_sum      = 0.0;
	if (!isCorePruned) {
		m_ids_unprocd.erase(m_ids_unprocd.begin() + index_core);
	}
}


double HS::BasicDNNHSearch::ExpansionGroup::epDelta() {

	if (size() <= 1) {
		m_epDelta = __DBL_MAX__;
	} else if (m_epDelta < 0.0) {
		double pdmean = pdSum() / pairs();
		double ndmean = ndSum() / size();
		m_epDelta = (pdmean / (pdmean + ndmean)) * delta();
	}

	return m_epDelta;
}


void HS::BasicDNNHSearch::ExpansionGroup::expand() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	m_pd_sum += ndSum();
	m_n_pair += size();
	m_ids.push_back(nextId());
	m_centroid = Eigen::VectorXd(0);
	m_id_next = -1;
	m_epDelta = -1.0;
	m_nd_sum = -1.0;
}


int HS::BasicDNNHSearch::ExpansionGroup::nextId() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	if (m_id_next < 0) {
		m_id_next = m_ds->findNN(centroid(), &m_ids_unprocd, true); // true がきもちわるい
	}
	
	return m_id_next;
}


double HS::BasicDNNHSearch::ExpansionGroup::pdSum() {

	if (m_pd_sum < 0.0) {
		double pd_sum = 0.0;
		for (int i=0; i<size(); i++) {
			for (int j=i+1; j<size(); j++) {
				pd_sum += m_ds->distance(m_ids[i], m_ids[j]);
			}
		}
		m_pd_sum = pd_sum;
	}

	return m_pd_sum;
}


double HS::BasicDNNHSearch::ExpansionGroup::ndSum() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	if (m_nd_sum < 0.0) {
		double nd_sum = 0.0;
		for (int id : m_ids) {
			nd_sum += m_ds->distance(id, nextId());
		}
		m_nd_sum = nd_sum;
	}

	return m_nd_sum;
}


int HS::BasicDNNHSearch::ExpansionGroup::pairs() {
	assert(size() > 1);

	if (m_n_pair < 0) {
		m_n_pair = size() * (size()-1) / 2;
	}

	return m_n_pair;
}
