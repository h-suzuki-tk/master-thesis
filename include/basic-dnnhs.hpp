#ifndef _BASIC_DNNHS_HPP_
#define _BASIC_DNNHS_HPP_

#include <iostream>
#include <cassert>
#include <vector>
#include <Eigen/Core>
#include "data.hpp"


namespace HS::DNNHS {
class Basic {

	class Point {
	
	public:
		Point(){}
		Point(Basic* ds, const Eigen::VectorXd& vec) { m_ds = ds; m_vector = vec; }
	
		Eigen::VectorXd& vec          () { return m_vector; }
		double           distFromQuery();

	private:
		Eigen::VectorXd m_vector;
		double          m_dist_from_query = -1.0;

	protected:
		Basic* m_ds;

	};

	class Group {
	
	public:
		Group(){}
		Group(Basic* ds, int id) { m_ds = ds; m_ids.push_back(id); }
		
		std::vector<int>& ids     () { return m_ids; }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();

		void add(const int id) { m_ids.push_back(id); }
	
	protected:
		Basic*      m_ds;
		std::vector<int> m_ids;
		Eigen::VectorXd  m_centroid;
		double           m_delta = -1.0;

		int size() { return m_ids.size(); }

	};
	
	class ExpansionGroup : public Group {
	
	public:	
		ExpansionGroup() : Group() {}
		ExpansionGroup(Basic* ds, const int index_core, const std::vector<int>& ids_data, bool isCorePruned);

		std::vector<int>& unprocdIds() { return m_ids_unprocd; }
		double            epDelta   ();
		
		void expand();

	private:
		std::vector<int> m_ids_unprocd;
		int 			 m_id_next = -1;
		double 			 m_epDelta = -1.0;
		double			 m_pd_sum;
		double           m_nd_sum  = -1.0;
		int              m_n_pair  = 0;

		int               nextId();
		double            pdSum ();
		double            ndSum ();
		int               pairs ();

	};

    public:
        Basic(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha);
        int              run();
		std::vector<int> result();

    protected:

    private:
		std::vector<Point> m_data;
		int                m_n_dim;
		Eigen::VectorXd    m_query;
		double             m_alpha;
		Eigen::MatrixXd    m_distance;
		std::vector<Group> m_group;
		double             m_bound;
		Group              m_result;
		
		double distance   (int id1, int id2);
		int    findNN     (const Eigen::VectorXd &query, std::vector<int> *ids, const bool shouldDelete = false);
		Group  findGroup  (const int id_core, const std::vector<int>& ids_data);
		void   updateBound(Group& group);
		void   filterPts  (const double bound, std::vector<int> *ids);

};
}

#endif
