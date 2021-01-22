#include "group.hpp"

Eigen::VectorXd DNNHSearch::Group::centroid() {
  
  if (m_centroid.size() == 0) {
    Eigen::VectorXd::Zero temp(m_data.cols());
    for (int id : ids()) {
      temp += m_data.row(id);
    }
    m_centroid = temp / size();
  }

  return m_centroid;
}



double DNNHSearch::Group::dist(const Eigen::VectorXd &v) {
  return ( centroid() - v ).norm();
}



double DNNHSearch::Group::sd() {
  double sd;

  double temp = 0.0;
  for (int id : ids()) {
    temp += (m_data.row(id) - centroid().transpose()).squaredNorm();
  }
  sd = temp / size();

  return sd;
}



double DNNHSearch::Group::delta() {
  return dist(query()) + sd();
}



double DNNHSearch::Group::epDelta() {

  if (m_epDelta < 0.0) {
    double pdmean = 0.0;
    double ndmean = 0.0;
    for (int i=0; i<size(); i++) {
      for (int j=0; j<size(); j++) {
        pdmean += ( m_data.row(id(i)) - m_data.row(id(j)) ).norm();
      }
      ndmean += ( centroid() - m_data.row(id(i)) ).norm();
    }
    pdmean /= ( size() * (size()-1) ) / 2;
    ndmean /= size();

    m_epDelta = delta() * ( pdmean / (pdmean+ndmean) );
  }

  return m_epDelta;
}

