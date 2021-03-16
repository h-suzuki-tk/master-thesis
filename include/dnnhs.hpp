#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#include<vector>
#include<Eigen/Core>

namespace HS::DNNHS {

class DNNHS;
class Points;
class Group;
class ExpansionMetric;
class ExpansionGroup;
class NewExpansionGroup;

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
		int               size    () { return m_ids.size(); }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();
		DNNHS&            ds      () { return *m_ds; } 

		void add(const int id) { m_ids.push_back(id); }
	
	protected:
		DNNHS*           m_ds;
		std::vector<int> m_ids;
		Eigen::VectorXd  m_centroid;
		double           m_delta;

};

class ExpansionMetric {

	public:
	enum class Metric {
		PAIRWISE
	};
	static ExpansionMetric* create(NewExpansionGroup* ep_group);

	ExpansionMetric(NewExpansionGroup* ep_group);

	virtual double value () { return -1.0; }
	virtual int    update() { return -1; }

	NewExpansionGroup& epGroup() { return *m_ep_group; }

	protected:

	private:
	NewExpansionGroup* m_ep_group;

};

class PairwiseExpansionMetric : public ExpansionMetric {

	static constexpr double VALUE_UNCALC = -1.0;

	public:
	PairwiseExpansionMetric(NewExpansionGroup* ep_group);

	double value () override;
	int    update() override;

	protected:

	private:
	double m_value;
	double m_pd_sum;
	double m_nd_sum;
	int    m_pd_pairs_num;
	int    m_nd_pairs_num;
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

class NewExpansionGroup : public Group {

	public:
	NewExpansionGroup();
	NewExpansionGroup(DNNHS* ds, const int core_pt);
	~NewExpansionGroup();

	int    setNextPt(const int pt);
	double epd      ();
	int    expand   ();
	int    nextPt   () { return m_next_pt; }

	protected:

	private:
	static constexpr int    PT_UNSET  = -1;
	static constexpr double EPD_UNSET = -1.0;

	ExpansionMetric* m_metric;
	int              m_next_pt;
	double           m_epd;

};


class DNNHS {

	static constexpr int DIST_UNCALC = -1.0;

	public:
		DNNHS(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha);

		int                     findNN     (const Eigen::VectorXd& query, std::vector<int>* ids, const bool shouldDelete = false);
		std::tuple<int, double> newFindNN  (const Eigen::VectorXd& query, const std::vector<int>& pts);
		void                    filterPts  (std::vector<int> *ids);
		void                    updateBound(Group& group);

		ExpansionMetric::Metric expansionMetric()   { return m_expansion_metric; }
		Points&                 data()              { return m_data; }
		Eigen::VectorXd         data(const int& id) { return m_data[id]; }
		Eigen::VectorXd&        query()             { return m_query; }
		int                     dims() const        { return m_data.dims(); }
		std::vector<Group>&     groups()            { return m_groups; }      
		Group&                  result()            { return m_result; }

		double                  betwDist(const int pt1, const int pt2);

	protected:
		ExpansionMetric::Metric m_expansion_metric = ExpansionMetric::Metric::PAIRWISE;
		Points                  m_data;
		Eigen::VectorXd         m_query;
		double                  m_alpha;
		double                  m_bound;
		std::vector<Group>      m_groups;
		Group                   m_result;

		Eigen::MatrixXd         m_betw_dist;
};
}

#endif
