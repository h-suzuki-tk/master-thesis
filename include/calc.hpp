#ifndef _CALC_H_
#define _CALC_H_

#include <valarray>
#include <Eigen/Core>

double sd(std::valarray<double> ary);
double sd(Eigen::MatrixXd data, Eigen::VectorXd center);
double norm_cdf(double u);

#endif