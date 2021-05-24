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
		int               size    () { return m_ids.size(); }
		Eigen::VectorXd&  centroid();
		double            dist    (const Eigen::VectorXd &v);
		double            sd      ();
		double            delta   ();
		DNNHS&            ds      () const { return *m_ds; } 

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
	static ExpansionMetric* create(ExpansionMetric::Metric metric, ExpansionGroup* ep_group);
	static ExpansionMetric* create(ExpansionGroup* ep_group);

	ExpansionMetric(ExpansionGroup* ep_group);

	virtual double value () { return -1.0; }
	virtual int    update() { return -1; }

	ExpansionGroup& epGroup() { return *m_ep_group; }

	protected:

	private:
	ExpansionGroup* m_ep_group;

};

class PairwiseExpansionMetric : public ExpansionMetric {

	static constexpr double VALUE_UNCALC = -1.0;
	static constexpr int    NUM_UNCALC   = -1;

	public:
	PairwiseExpansionMetric(ExpansionGroup* ep_group);

	double value () override;
	int    update() override;

	protected:
	double& pdSum();
	double& ndSum();
	int&    pdPairsNum();
	int&    ndPairsNum();

	private:
	double m_value;
	double m_pd_sum;
	double m_nd_sum;
	int    m_pd_pairs_num;
	int    m_nd_pairs_num;
};


class ExpansionGroup : public Group {

	public:
	ExpansionGroup();
	ExpansionGroup(DNNHS* ds, const int core_pt);
	~ExpansionGroup();

	ExpansionGroup(const ExpansionGroup& ep_group);
	ExpansionGroup &operator=(const ExpansionGroup& ep_group);

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

	private: static constexpr int DIST_UNCALC = -1.0;
	public:
		static constexpr int LOWER_CLUSTER_SIZE = 10;  
		static constexpr int UPPER_CLUSTER_SIZE = 100;

	public:
		DNNHS(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha);

		std::tuple<int, double> findNN  (const Eigen::VectorXd& query, const std::vector<int>& pts);
		void                    filterPts  (std::vector<int> *ids);
		void                    updateBound(Group& group);

		ExpansionMetric::Metric expansionMetric()   { return m_expansion_metric; }
		Points&                 data()              { return m_data; }
		Eigen::VectorXd         data(const int& id) { return m_data[id]; }
		Eigen::VectorXd&        query()             { return m_query; }
		int                     dims() const        { return m_data.dims(); }
		double                  alpha()       const { return m_alpha; }
		std::vector<Group>&     groups()            { return m_groups; }      
		Group&                  result()            { return m_result; }

		double                  betwDist(const int pt1, const int pt2);
		double                  fromQueryDist(const int pt);

	protected:
		ExpansionMetric::Metric m_expansion_metric = ExpansionMetric::Metric::PAIRWISE;
		Points                  m_data;
		Eigen::VectorXd         m_query;
		double                  m_alpha;
		double                  m_bound;
		std::vector<Group>      m_groups;
		Group                   m_result;

		Eigen::VectorXd         m_from_query_dist;
};
}

#endif
