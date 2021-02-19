#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#include<vector>
#include<Eigen/Core>

namespace HS::DNNHS {

class DNNHS;
class Points;
class Group;
class ExpansionGroup;

class Points {

	public:
		Points(const Eigen::MatrixXd& pts);
		
		Eigen::VectorXd operator[](const int& id);
		int             size() const;
		int             dims() const;

	protected:
		Eigen::MatrixXd m_pts;

};
	
class Group {

	public:
		Group();
		Group(DNNHS* ds, const int& id);
		
		std::vector<int>& ids     () { return m_ids; }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();

		void add(const int id) { m_ids.push_back(id); }
	
	protected:
		DNNHS*           m_ds;
		std::vector<int> m_ids;
		Eigen::VectorXd  m_centroid;
		double           m_delta;

		int size() { return m_ids.size(); }

};

class ExpansionGroup : public Group {

	public:
		ExpansionGroup() { }
		ExpansionGroup(DNNHS* ds, const int index_core, const std::vector<int>& ids_data, bool isCorePruned);

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
		Eigen::MatrixXd  m_data_pw_dist; // pw: pairwise

		int               nextId();
		double            pdSum ();
		double            ndSum ();
		int               pairs ();
		double            distBetw(const int id1, const int id2);

};

class DNNHS {

	public:
		DNNHS(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha);
		int  findNN     (const Eigen::VectorXd& query, std::vector<int>* ids, const bool shouldDelete = false);
		void filterPts  (std::vector<int> *ids);
		void updateBound(Group& group);

		Points&             data()              { return m_data; }
		Eigen::VectorXd     data(const int& id) { return m_data[id]; }
		Eigen::VectorXd&    query()             { return m_query; }
		int                 dims() const        { return m_data.dims(); }
		std::vector<Group>& groups()            { return m_groups; }      
		Group&              result()            { return m_result; } 

	protected:
		Points             m_data;
		Eigen::VectorXd    m_query;
		double             m_alpha;
		double             m_bound;
		std::vector<Group> m_groups;
		Group              m_result;
};
}

#endif
