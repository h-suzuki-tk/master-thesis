#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#define BASIC "basic"
#define GRID "grid"
#define SPLIT "split"

#include <iostream>
#include <vector>
#include <random>
#include <Eigen/Core>

class DNNHSearch {

	// --------------------------------------------------
	//  Point
	// --------------------------------------------------
	class Point {
	
	public:
		Point(){}
		Point(DNNHSearch* ds, const Eigen::VectorXd& vec) { m_ds = ds; m_vector = vec; }
	
		Eigen::VectorXd& vec          () { return m_vector; }
		double           distFromQuery();

	private:
		Eigen::VectorXd m_vector;
		double          m_dist_from_query = -1.0;

	protected:
		DNNHSearch* m_ds;

	};
	
	// --------------------------------------------------
	//  Group
	// ----------------------------------------
	class Group {
	
	public:
		Group(){}
		Group(DNNHSearch* ds, int id) { m_ds = ds; m_ids.push_back(id); }
		
		std::vector<int>& ids     () { return m_ids; }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();

		void add(const int id) { m_ids.push_back(id); }
	
	protected:
		DNNHSearch*      m_ds;
		std::vector<int> m_ids;
		Eigen::VectorXd  m_centroid;
		double           m_delta = -1.0;

		int size() { return m_ids.size(); }

	};
	
	// --------------------------------------------------
	//  ExpansionGroup
	// --------------------------------------------------
	class ExpansionGroup : public Group {
	
	public:	
		ExpansionGroup() : Group() {}
		ExpansionGroup(DNNHSearch* ds, const int index_core, const std::vector<int>& ids_data, bool isCorePruned);

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
	DNNHSearch(const Eigen::MatrixXd& data, const double alpha, const Eigen::VectorXd& query = Eigen::VectorXd(0));
	
	Eigen::VectorXd& query () { return m_query; }
	Group&           result() { return m_result; }

	void run(std::string clustWay);

private:
	std::vector<Point> m_data;
	int                m_n_dim;
	Eigen::VectorXd    m_query;
	double             m_alpha;
	Eigen::MatrixXd    m_distance;
	std::vector<Group> m_group;
	double             m_bound = __DBL_MAX__;
	Group              m_result;

	int basicSearch();
	//int gridSearch();
	//int splitSearch();
	
	double distance   (int id1, int id2);
	int    findNN     (const Eigen::VectorXd &query, std::vector<int> *ids, const bool shouldDelete = false);
	Group  findGroup  (const int id_core, const std::vector<int>& ids_data);
	void   updateBound(Group& group);
	void   filterPts  (const double bound, std::vector<int> *ids);

};


#endif
