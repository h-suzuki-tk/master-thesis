#include "dnnhs.hpp"


HS::DNNHS::Points::Points(
	const Eigen::MatrixXd& pts) :
	m_pts(pts) {
}

Eigen::VectorXd HS::DNNHS::Points::operator[](
	const int& id) {
	
	assert(id < size());
	return m_pts.row(id);
}

int HS::DNNHS::Points::size() const {
	return m_pts.rows();
}


int HS::DNNHS::Points::dims() const {
	return m_pts.cols();
}


HS::DNNHS::Group::Group() :
	m_delta(-1.0) {
}


HS::DNNHS::Group::Group(
	DNNHS*     ds,
	const int& id) :
	m_ds(ds),
	m_delta(-1.0) {
	
	m_ids.emplace_back(id);
}


Eigen::VectorXd& HS::DNNHS::Group::centroid() {

    if (m_centroid.size() == 0) {
        Eigen::VectorXd temp;

		temp = Eigen::VectorXd::Zero(ds().data().dims());
        for (int id : m_ids) {
            temp += ds().data(id);
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
        temp += (ds().data(id) - centroid()).squaredNorm();
    }
    sd = sqrt(temp / size());

    return sd;
}


double HS::DNNHS::Group::delta() {

	if (m_delta < 0.0) {
		if (m_ids.size() <= 1) { m_delta = __DBL_MAX__; } 
		else { m_delta = dist(ds().query()) + sd(); }
	}

    return m_delta;
}


HS::DNNHS::ExpansionGroup::ExpansionGroup(
	DNNHS*                  ds,
	const int 			    index_core, 
	const std::vector<int>& ids_data,
	bool                    isCorePruned) : Group(ds, ids_data[index_core]) {

	m_ids_unprocd = ids_data;
	m_pd_sum      = 0.0;
	if (!isCorePruned) {
		m_ids_unprocd.erase(m_ids_unprocd.begin() + index_core);
	}
	m_data_pw_dist = Eigen::MatrixXd::Constant( ds->data().size(), ds->data().size(), -1.0);
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
	m_id_next  = -1;
	m_epDelta  = -1.0;
	m_nd_sum   = -1.0;
	m_delta    = -1.0;
}


int HS::DNNHS::ExpansionGroup::nextId() {
	assert(m_ids_unprocd.size() > 0 || m_id_next >= 0);

	if (m_id_next < 0) {
		m_id_next = ds().findNN(centroid(), &m_ids_unprocd, true);
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
			nd_sum += distBetw(id, nextId());
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


double HS::DNNHS::ExpansionGroup::distBetw(
	const int id1, 
	const int id2) {

	if (m_data_pw_dist(id1, id2) < 0.0 && m_data_pw_dist(id2, id1) < 0.0) {
		m_data_pw_dist(id1, id2) = (ds().data(id1) - ds().data(id2)).norm();
		m_data_pw_dist(id2, id1) = m_data_pw_dist(id1, id2);
	}

	return m_data_pw_dist(id1, id2);
}


HS::DNNHS::NewExpansionGroup::NewExpansionGroup() :
	m_metric(nullptr),
	m_next_pt(PT_UNSET),
	m_epd(EPD_UNSET) {
}


HS::DNNHS::NewExpansionGroup::NewExpansionGroup(
	DNNHS*    ds,
	const int core_pt) : 
	Group(ds, core_pt),
	m_next_pt( PT_UNSET ),
	m_epd( EPD_UNSET ) {

	m_metric = ExpansionMetric::create( this );
}


HS::DNNHS::NewExpansionGroup::~NewExpansionGroup() {
	delete m_metric;
}


int HS::DNNHS::NewExpansionGroup::setNextPt(
	const int pt) {
	
	if ( pt < 0 || ds().data().size() <= pt ) { return 1; }
	return 0;
}


double HS::DNNHS::NewExpansionGroup::epd() {

	if ( m_epd == EPD_UNSET ) {
		m_epd = m_metric->value();
	}
	return m_epd;
}


int HS::DNNHS::NewExpansionGroup::expand() {
	
	if ( m_next_pt == PT_UNSET ) { return 1; }
	
	ids().emplace_back(m_next_pt);
	m_metric->update();
	m_next_pt = PT_UNSET;
	m_epd     = EPD_UNSET;

	return 0;
}


HS::DNNHS::ExpansionMetric* HS::DNNHS::ExpansionMetric::create(
	NewExpansionGroup* ep_group) {

	switch ( ep_group->ds().expansionMetric() ) {
	case Metric::PAIRWISE:
		return new PairwiseExpansionMetric( ep_group );
		break;
	default:
		return nullptr;
		break;
	}

}


HS::DNNHS::ExpansionMetric::ExpansionMetric(
	NewExpansionGroup* ep_group) :
	m_ep_group( ep_group ) {	
}


HS::DNNHS::PairwiseExpansionMetric::PairwiseExpansionMetric(
	NewExpansionGroup* ep_group) :
	ExpansionMetric( ep_group ),
	m_value( VALUE_UNCALC ),
	m_nd_sum( 0.0 ),
	m_pd_sum( 0.0 ),
	m_pd_pairs_num( 0 ),
	m_nd_pairs_num( 0 ) {
}


double HS::DNNHS::PairwiseExpansionMetric::value() {

	if ( epGroup().size() <= 1 ) {
		m_value = __DBL_MAX__;
	} else if ( m_value == VALUE_UNCALC ) {

		for ( const auto& pt : epGroup().ids() ) {
			m_nd_sum += epGroup().ds().betwDist( pt, epGroup().nextPt() );
		}
		m_nd_pairs_num = epGroup().size();
		double pdmean = m_pd_sum / m_pd_pairs_num;
		double ndmean = m_nd_sum / m_nd_pairs_num; 
		m_value = ( pdmean / ( pdmean + ndmean ) ) * epGroup().delta();

	}

	return m_value;
}


int HS::DNNHS::PairwiseExpansionMetric::update() {

	m_value        = VALUE_UNCALC;
	m_pd_sum       = m_pd_sum + m_nd_sum;
	m_nd_sum       = 0.0;
	m_pd_pairs_num = m_pd_pairs_num + m_nd_pairs_num;
	m_nd_pairs_num = 0;

	return 0;
} 


HS::DNNHS::DNNHS::DNNHS(
	const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha) :
	m_data(Points(data)), m_query(query), m_alpha(alpha), 
	m_betw_dist( Eigen::MatrixXd::Constant( data.rows(), data.rows(), DIST_UNCALC ) ) {

	assert(data.rows() > 1 && data.cols() > 0);
	assert(query.size() > 0);
	assert(data.cols() == query.size());
	assert(alpha > 0);

	m_bound = __DBL_MAX__;
}


int HS::DNNHS::DNNHS::findNN(
	const Eigen::VectorXd& query,
	std::vector<int>*      ids,
	const bool             shouldDelete) {
	
	assert(!ids->empty());
	
	int    id_NN   = -1;
	double dist_NN = __DBL_MAX__;
	std::vector<int>::iterator itr_NN;

	for (auto itr_id = ids->begin(); itr_id != ids->end(); ) {
		double dist = ( m_data[*itr_id]-m_query ).norm();
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


/** TODO: 三角不等式を用いた効率化 **/
std::tuple<int, double> HS::DNNHS::DNNHS::newFindNN(
	const Eigen::VectorXd&  query, 
	const std::vector<int>& pts) {

	int    nn_index = -1;
	double nn_dist  = __DBL_MAX__;

	for ( size_t i=0; i<pts.size(); ++i ) {
		double d = ( data(i) - query ).norm();
		if ( d < nn_dist ) {
			nn_index = i;
			nn_dist  = d;
		}
	}

	return { nn_index, nn_dist };
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


double HS::DNNHS::DNNHS::betwDist(
	const int pt1, 
	const int pt2) {
	
	assert( 0 <= pt1 && pt1 < data().size() );
	assert( 0 <= pt2 && pt2 < data().size() );

	if ( m_betw_dist(pt1, pt2) == DIST_UNCALC ) {
		m_betw_dist(pt1, pt2) = ( data(pt1) - data(pt2) ).norm();
		m_betw_dist(pt2, pt1) = m_betw_dist(pt1, pt2);
	}

	return m_betw_dist(pt1, pt2);
}
