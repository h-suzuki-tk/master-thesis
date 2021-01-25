#include "dnnhs.hpp"

DNNHSearch::DNNHSearch(
	const Eigen::MatrixXd& data, 
	const double           alpha,
	const Eigen::VectorXd& query) {

	m_data  = std::vector<Point>(data.rows());
	m_n_dim = data.cols();
    for (int i=0; i<data.rows(); i++) {
		m_data[i] = Point(this, data.row(i));
	}

    if (query.size() == 0) {
        std::random_device seed;
        std::mt19937 rand(seed());
        std::uniform_real_distribution<> unif(0.0, 1.0);
        m_query =
            Eigen::VectorXd::Zero(data.cols()).unaryExpr([&](double dummy) {
                return unif(rand);
            });
    } else {
        m_query = query;
    }

	m_alpha    = alpha;
	m_distance = Eigen::MatrixXd::Constant(m_data.size(), m_data.size(), -1.0);
}


void DNNHSearch::run(std::string clustWay) {
    int isOK = 0;

    if (clustWay == BASIC) {
        isOK = basicSearch();
    } else if (clustWay == GRID) {
        /*isOK = gridSearch();*/
    } else if (clustWay == SPLIT) {
        /*isOK = splitSearch();*/
    }

    if (isOK == 0) {
        std::cerr << "!Failed to run " << clustWay << " search!" << std::endl;
    }
}


double DNNHSearch::distance(
	int id1, 
	int id2) {

	if (m_distance(id1, id2) < 0.0 && m_distance(id2, id1) < 0.0) {
		m_distance(id1, id2) = (m_data[id1].vec() - m_data[id2].vec()).norm();
		m_distance(id2, id1) = m_distance(id1, id2);
	}

	return m_distance(id1, id2);
}



int DNNHSearch::findNN(
	const Eigen::VectorXd& query,
	std::vector<int>      *ids,
	const bool 			   shouldDelete) {
	
	assert(ids->size() > 0);
	
	int    id_NN   = -1;
	double dist_NN = __DBL_MAX__;
	std::vector<int>::iterator itr_NN;

	for (auto itr_id = ids->begin(); itr_id != ids->end(); ) {
		double dist = ( m_data[*itr_id].vec()-query ).norm();
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


DNNHSearch::Group DNNHSearch::findGroup(
	const int               id_core, 
	const std::vector<int>& ids_data) {
	
	assert(ids_data.size() > 0);
    
	ExpansionGroup g_min;
    ExpansionGroup g(this, id_core, ids_data);

	g.expand();
	g_min = g;
	while (!g.unprocdIds().empty()) {
		// 拡大
        g.expand();
		if (g.unprocdIds().empty()) { break; } // while 文と一緒で気持ち悪い

        // epΔ を計算して最小のものを保持
        if (g.epDelta() < g_min.epDelta()) {
            g_min = g;
        }
	}

    return g_min;
}


void DNNHSearch::updateBound(Group& group) {
	m_bound = ( 2 * m_alpha / (m_alpha + 1) ) * group.delta();
}


void DNNHSearch::filterPts(
	const double      bound, 
	std::vector<int> *ids) {

	ids->erase(std::remove_if(ids->begin(), ids->end(),	[&](int id) { 
		return m_data[id].distFromQuery() > bound; }), ids->end()
	);
}
