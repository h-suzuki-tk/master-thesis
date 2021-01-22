#ifndef _DNNHS_HPP_
#define _DNNHS_HPP_

#define BASIC "basic"
#define GRID "grid"
#define SPLIT "split"

#include <iostream>
#include <vector>
#include <random>
#include <Eigen/Core>

#include "basic.hpp"

class DNNHSearch {

  public:
    DNNHSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query = Eigen::VectorXd(0));
    void run(std::string clustWay);
    Eigen::MatrixXd data() { return m_data; }
    Eigen::VectorXd query() { return m_query; }
  
  private:

    // ----------------------------------------
    class Group {
      public:
        
        // コンストラクタ
        Group() {};
        Group(std::vector<int> ids) { m_ids = ids; }

        // 参照メソッド
        std::vector<int> ids()           { return m_ids; }
        std::vector<int> id(const int i) { return ids()[i]; }
        int              size()          { return ids().size(); }
        Eigen::VectorXd  centroid();
        double           dist(const Eigen::VectorXd &v);
        double           sd();
        double           delta();
        double           epDelta();
        
        // 操作メソッド
        void add(const int id) { m_ids.push_back(id); };

      private:
        std::vector<int> m_ids;
        Eigen::VectorXd  m_centroid;
        double           m_delta   = -1.0;
        double           m_epDelta = -1.0;
    };
    // ----------------------------------------

    Eigen::MatrixXd  m_data;
    Eigen::VectorXd  m_query;
    std::vector<Group> m_group;
    Group m_result;

    int findNN(const Eigen::VectorXd &query, const std::vector<int> ids);
    Group findGroup(const int id_core, std::vector<int> ids_data);
    
    int basicSearch();
    //int gridSearch();
    //int splitSearch();
};


#endif
