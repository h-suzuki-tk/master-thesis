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

	if ( size() == 0 ) {
		sd = __DBL_MAX__;
	} else {
		double temp = 0.0;
		for (int id : m_ids) {
			temp += (ds().data(id) - centroid()).squaredNorm();
		}
		sd = sqrt(temp / size());
	}

    return sd;
}


double HS::DNNHS::Group::delta() {

	if (m_delta < 0.0) {
		if (m_ids.size() <= 1) { m_delta = __DBL_MAX__; } 
		else { m_delta = dist(ds().query()) + sd(); }
	}

    return m_delta;
}


HS::DNNHS::ExpansionGroup::ExpansionGroup() :
	m_metric(nullptr),
	m_next_pt(PT_UNSET),
	m_epd(EPD_UNSET) {
}


HS::DNNHS::ExpansionGroup::ExpansionGroup(
	DNNHS*    ds,
	const int core_pt) : 
	Group(ds, core_pt),
	m_next_pt( PT_UNSET ),
	m_epd( EPD_UNSET ) {

	m_metric = ExpansionMetric::create( this );
}


HS::DNNHS::ExpansionGroup::~ExpansionGroup() {
	delete m_metric;
}


HS::DNNHS::ExpansionGroup::ExpansionGroup(
	const ExpansionGroup& ep_group) {

	m_ds          = ep_group.m_ds;
	m_ids         = ep_group.m_ids;
	m_centroid    = ep_group.m_centroid;
	m_delta       = ep_group.m_delta;
	m_metric      = ExpansionMetric::create( ep_group.ds().expansionMetric(), this );
	*m_metric     = *( ep_group.m_metric );
	m_next_pt     = ep_group.m_next_pt;
	m_epd         = ep_group.m_epd;

}


HS::DNNHS::ExpansionGroup& HS::DNNHS::ExpansionGroup::operator=(
	const ExpansionGroup& ep_group) {

	if ( this != &ep_group ) {
		this->m_ds          = ep_group.m_ds;
		this->m_ids         = ep_group.m_ids;
		this->m_centroid    = ep_group.m_centroid;
		this->m_delta       = ep_group.m_delta;
		this->m_metric      = ExpansionMetric::create( ep_group.ds().expansionMetric(), this );
		*( this->m_metric ) = *( ep_group.m_metric );
		m_next_pt           = ep_group.m_next_pt;
		m_epd               = ep_group.m_epd;
	}

	return *this;
}


int HS::DNNHS::ExpansionGroup::setNextPt(
	const int pt) {
	
	if ( pt < 0 || ds().data().size() <= pt ) { return 1; }
	m_next_pt = pt;
	return 0;
}


double HS::DNNHS::ExpansionGroup::epd() {

	if ( m_metric == nullptr ) {
		m_epd = __DBL_MAX__;
	} else if ( m_epd == EPD_UNSET ) {
		m_epd = m_metric->value();
	}
	return m_epd;
}


int HS::DNNHS::ExpansionGroup::expand() {
	
	if ( m_next_pt == PT_UNSET ) { return 1; }
	
	m_metric->update();
	ids().emplace_back(m_next_pt);
	m_next_pt = PT_UNSET;
	m_epd     = EPD_UNSET;

	return 0;
}

HS::DNNHS::ExpansionMetric* HS::DNNHS::ExpansionMetric::create(
	Metric             metric, 
	ExpansionGroup* ep_group) {

	switch ( metric ) {
	case Metric::PAIRWISE:
		return new PairwiseExpansionMetric( ep_group );
		break;
	default:
		return nullptr;
		break;
	}

}


HS::DNNHS::ExpansionMetric* HS::DNNHS::ExpansionMetric::create(
	ExpansionGroup* ep_group) {

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
	ExpansionGroup* ep_group) :
	m_ep_group( ep_group ) {	
}


HS::DNNHS::PairwiseExpansionMetric::PairwiseExpansionMetric(
	ExpansionGroup* ep_group) :
	ExpansionMetric( ep_group ),
	m_value( VALUE_UNCALC ),
	m_nd_sum( VALUE_UNCALC ),
	m_pd_sum( VALUE_UNCALC ),
	m_nd_pairs_num( NUM_UNCALC ),
	m_pd_pairs_num( NUM_UNCALC ) {

}


double HS::DNNHS::PairwiseExpansionMetric::value() {

	if ( epGroup().size() <= 1 ) {
		m_value = __DBL_MAX__;
	} else if ( m_value == VALUE_UNCALC ) {

		double pdmean = pdSum() / pdPairsNum();
		double ndmean = ndSum() / ndPairsNum(); 
		m_value = ( pdmean / ( pdmean + ndmean ) ) * epGroup().delta();

	}

	return m_value;
}


int HS::DNNHS::PairwiseExpansionMetric::update() {

	m_value        = VALUE_UNCALC;
	pdSum()        = pdSum() + ndSum();
	ndSum()        = VALUE_UNCALC;
	pdPairsNum()   = pdPairsNum() + ndPairsNum();
	ndPairsNum()   = NUM_UNCALC;

	return 0;
} 


double& HS::DNNHS::PairwiseExpansionMetric::pdSum() {

	if ( m_pd_sum == VALUE_UNCALC ) {
		m_pd_sum = 0.0;
		for ( int id1=0; id1<epGroup().size(); ++id1 ) {
			for ( int id2=id1+1; id2<epGroup().size(); ++id2  ) {
				m_pd_sum += epGroup().ds().betwDist( id1, id2 );
			}
		}
	}

	return m_pd_sum;
}


double& HS::DNNHS::PairwiseExpansionMetric::ndSum() {

	if ( m_nd_sum == VALUE_UNCALC ) {
		m_nd_sum = 0.0;
		for ( const auto& pt : epGroup().ids() ) {
			m_nd_sum += epGroup().ds().betwDist( pt, epGroup().nextPt() );
		}
	}

	return m_nd_sum;
}


int& HS::DNNHS::PairwiseExpansionMetric::pdPairsNum() {

	if ( m_pd_pairs_num == NUM_UNCALC ) {
		m_pd_pairs_num = ( epGroup().size() * ( epGroup().size() - 1) ) / 2.0; 
	}

	return m_pd_pairs_num;
}


int& HS::DNNHS::PairwiseExpansionMetric::ndPairsNum() {

	if ( m_nd_pairs_num == NUM_UNCALC ) {
		m_nd_pairs_num = epGroup().size();
	}

	return m_nd_pairs_num;
}



HS::DNNHS::DNNHS::DNNHS(
	const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha) :
	m_data(Points(data)), 
	m_query(query), 
	m_alpha(alpha),
	m_from_query_dist( Eigen::VectorXd::Constant( data.rows(), DIST_UNCALC ) ) {

	assert(data.rows() > 1 && data.cols() > 0);
	assert(query.size() > 0);
	assert(data.cols() == query.size());
	assert(alpha > 0);

	m_bound = __DBL_MAX__;
}


/** TODO: 三角不等式を用いた効率化 **/
std::tuple<int, double> HS::DNNHS::DNNHS::findNN(
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


double HS::DNNHS::DNNHS::fromQueryDist(
	const int pt) {
	
	assert( 0 <= pt && pt < data().size() );

	if ( m_from_query_dist(pt) == DIST_UNCALC ) {
		m_from_query_dist(pt) = ( query() - data(pt) ).norm();
	}

	return m_from_query_dist(pt);
}


double HS::DNNHS::DNNHS::betwDist(
	const int pt1, 
	const int pt2) {
	
	assert( 0 <= pt1 && pt1 < data().size() );
	assert( 0 <= pt2 && pt2 < data().size() );

	return ( data(pt1) - data(pt2) ).norm();
}
