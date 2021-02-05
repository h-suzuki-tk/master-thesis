#ifndef _BASIC_DNNHS_HPP_
#define _BASIC_DNNHS_HPP_

#include <iostream>
#include <cassert>
#include <vector>
#include <Eigen/Core>
#include "data.hpp"


namespace HS {
class BasicDNNHSearch {

	class Point {
	
	public:
		Point(){}
		Point(BasicDNNHSearch* ds, const Eigen::VectorXd& vec) { m_ds = ds; m_vector = vec; }
	
		Eigen::VectorXd& vec          () { return m_vector; }
		double           distFromQuery();

	private:
		Eigen::VectorXd m_vector;
		double          m_dist_from_query = -1.0;

	protected:
		BasicDNNHSearch* m_ds;

	};

	class Group {
	
	public:
		Group(){}
		Group(BasicDNNHSearch* ds, int id) { m_ds = ds; m_ids.push_back(id); }
		
		std::vector<int>& ids     () { return m_ids; }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();

		void add(const int id) { m_ids.push_back(id); }
	
	protected:
		BasicDNNHSearch*      m_ds;
		std::vector<int> m_ids;
		Eigen::VectorXd  m_centroid;
		double           m_delta = -1.0;

		int size() { return m_ids.size(); }

	};
	
	class ExpansionGroup : public Group {
	
	public:	
		ExpansionGroup() : Group() {}
		ExpansionGroup(BasicDNNHSearch* ds, const int index_core, const std::vector<int>& ids_data, bool isCorePruned);

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
        BasicDNNHSearch();
        void setData(const std::string dataPath, const int dataSize, const int dataDim);
        void setQuery(const Eigen::VectorXd& query);
        void run(const double alpha = 2.0);
        void printResult();

    protected:

    private:
		std::vector<Point> m_data;
		int                m_n_dim;
		Eigen::VectorXd    m_query;
		double             m_alpha;
		Eigen::MatrixXd    m_distance;
		std::vector<Group> m_group;
		double             m_bound = __DBL_MAX__;
		Group              m_result;
		
		double distance   (int id1, int id2);
		int    findNN     (const Eigen::VectorXd &query, std::vector<int> *ids, const bool shouldDelete = false);
		Group  findGroup  (const int id_core, const std::vector<int>& ids_data);
		void   updateBound(Group& group);
		void   filterPts  (const double bound, std::vector<int> *ids);

};
}

#endif
