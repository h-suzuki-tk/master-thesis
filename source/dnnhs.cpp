#include "dnnhs.hpp"


HS::DNNHS::Points::Points(
	const Eigen::MatrixXd& pts) :
	m_pts(pts); {
}

Eigen::VectorXd& HS::DNNHS::Points::operator[](
	const int& id) const {
	
	assert(id < size());
	return m_pts.row(id);
}

int HS::DNNHS::Points::size() const {
	return m_pts.rows();
}


int HS::DNNHS::Points::dims() const {
	return m_pts.cols();
}


HS::DNNHS::Group::Group(
	const DNNHS& ds,
	const int&   id) :
	m_ds(ds) {
	
	m_ids.emplace_back(id);
}


Eigen::VectorXd& HS::DNNHS::Group::centroid() {

    if (m_centroid.size() == 0) {
        Eigen::VectorXd temp;

		temp = Eigen::VectorXd::Zero(m_data.dims());
        for (int id : m_ids) {
            temp += m_data.pt(id).vec();
        }
        m_centroid = temp / size();
    }

    return m_centroid;
}


double HS::DNNHS::Group::dist(const Eigen::VectorXd &v) {
    return (centroid() - v).norm();
}


double HS::DNNHS::Group::sd() {
    double sd;

    double temp = 0.0;
    for (int id : m_ids) {
        temp += (m_data.pt(id).vec() - centroid()).squaredNorm();
    }
    sd = sqrt(temp / size());

    return sd;
}


double HS::DNNHS::Group::delta() {

	if (m_ids.size() == 0) {
		m_delta = __DBL_MAX__;
	} else if (m_delta < 0.0) {
		m_delta = dist(m_ds.m_query) + sd();
	}

    return m_delta;
}


HS::DNNHS::ExpansionGroup::ExpansionGroup(
	const DNNHS&            ds;
	const int 			    index_core, 
	const std::vector<int>& ids_data,
	bool                    isCorePruned) : Group(ds, ids_data[index_core]) {

	m_ids_unprocd = ids_data;
	m_pd_sum      = 0.0;
	if (!isCorePruned) {
		m_ids_unprocd.erase(m_ids_unprocd.begin() + index_core);
	}
	m_data_pw_dist = Eigen::MatrixXd::Constant(m_data.size(), m_data.size(), -1.0);
}


double HS::DNNHS::ExpansionGroup::epDelta() {

	if (size() <= 1) {
		m_epDelta = __DBL_MAX__;
	} else if (m_epDelta < 0.0) {
		double pdmean = pdSum() / pairs();
		double ndmean = ndSum() / size();
		m_epDelta = (pdmean / (pdmean + ndmean)) * delta();
	}

	return m_epDelta;
}


void HS::DNNHS::ExpansionGroup::expand() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	m_pd_sum += ndSum();
	m_n_pair += size();
	m_ids.push_back(nextId());
	m_centroid = Eigen::VectorXd(0);
	m_id_next = -1;
	m_epDelta = -1.0;
	m_nd_sum = -1.0;
}


int HS::DNNHS::ExpansionGroup::nextId() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	if (m_id_next < 0) {
		m_id_next = findNN(centroid(), &m_ids_unprocd, true);
	}
	
	return m_id_next;
}


double HS::DNNHS::ExpansionGroup::pdSum() {

	if (m_pd_sum < 0.0) {
		double pd_sum = 0.0;
		for (int i=0; i<size(); i++) {
			for (int j=i+1; j<size(); j++) {
				pd_sum += distBetw(m_ids[i], m_ids[j]);
			}
		}
		m_pd_sum = pd_sum;
	}

	return m_pd_sum;
}


double HS::DNNHS::ExpansionGroup::ndSum() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	if (m_nd_sum < 0.0) {
		double nd_sum = 0.0;
		for (int id : m_ids) {
			nd_sum += m_ds->distBetw(id, nextId());
		}
		m_nd_sum = nd_sum;
	}

	return m_nd_sum;
}


int HS::DNNHS::ExpansionGroup::pairs() {
	assert(size() > 1);

	if (m_n_pair < 0) {
		m_n_pair = size() * (size()-1) / 2;
	}

	return m_n_pair;
}


ouble HS::DNNHS::ExpansionGroup::distBetw(
	const int id1, 
	const int id2) {

	if (m_data_pw_dist(id1, id2) < 0.0 && m_data_pw_dist(id2, id1) < 0.0) {
		m_data_pw_dist(id1, id2) = (m_data[id1] - m_data[id2]).norm();
		m_data_pw_dist(id2, id1) = m_data_pw_dist(id1, id2);
	}

	return m_distance(id1, id2);
}


HS::DNNHS::DNNHS::DNNHS(
	const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha) :
	m_data(Points(data)), m_query(query), m_alpha(alpha) {

	assert(data.rows() > 1 && data.cols() > 0);
	assert(query.size() > 0);
	assert(data.cols() == query.size());
	assert(alpha > 0);

	m_bound = __DBL_MAX__;
}


int HS::DNNHS::DNNHS::findNN(
	const Eigen::VectorXd& query
	std::vector<int>*      ids,
	const bool             shouldDelete) {
	
	assert(!ids->empty());
	
	int    id_NN   = -1;
	double dist_NN = __DBL_MAX__;
	std::vector<int>::iterator itr_NN;

	for (auto itr_id = ids->begin(); itr_id != ids->end(); ) {
		double dist = ( m_data[*itr_id]-query ).norm();
		if (dist < dist_NN) {
			id_NN   = *itr_id;
			dist_NN = dist;
			itr_NN  = itr_id;
		}
		itr_id++;
	}
	if (shouldDelete) { ids->erase(itr_NN); }

	return id_NN;
}


void HS::DNNHS::DNNHS::filterPts(
	std::vector<int>* ids) {

	ids->erase(std::remove_if(ids->begin(), ids->end(),	[&](int id) { 
		return ( m_data[id]-m_query ).norm() > m_bound; }), ids->end());
}


void HS::DNNHS::DNNHS::updateBound(
	Group& group) {
	
	m_bound = ( 2 * m_alpha / (m_alpha + 1) ) * group.delta();
}
