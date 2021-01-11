#include "calc.hpp"

// --------------------------------------------------
//  sd
//  - 標準偏差を計算
// --------------------------------------------------
double sd(std::valarray<double> ary) {
	
	int size;
	double mean;
	double squared_mean;
	double var;
	double sd;
	
	size 	= ary.size();
	mean 	= 1.0/size * ary.sum();
	squared_mean = 1.0/size * (ary*ary).sum();
	var 	= squared_mean - mean*mean;
	sd 		= sqrt(var);
	
	return sd;
}

double sd(Eigen::MatrixXd data, Eigen::VectorXd center) {
	assert(data.cols() == center.size());
	
	return sqrt( (data.rowwise() - center.transpose()).rowwise().squaredNorm().array().sum() / data.rows() );
	//return sqrt( data.rowwise().squaredNorm().array().sum() / data.rows() - center.squaredNorm() );
}

// --------------------------------------------------
//
// norm_cdf
// ガウス分布の累積確率
//
// --------------------------------------------------
/*
 * 参考: http://www5.airnet.ne.jp/tomy/cpro/sslib11.htm
 */
double norm_cdf(double u) {
	
	static double a[9] = {	 1.24818987e-4, -1.075204047e-3, 5.198775019e-3,
							-0.019198292004, 0.059054035642,-0.151968751364,
							 0.319152932694,-0.5319230073,   0.797884560593};
	static double b[15] = {	-4.5255659e-5,   1.5252929e-4,  -1.9538132e-5,
							-6.76904986e-4,  1.390604284e-3,-7.9462082e-4,
							-2.034254874e-3, 6.549791214e-3,-0.010557625006,
							 0.011630447319,-9.279453341e-3, 5.353579108e-3,
							-2.141268741e-3, 5.35310549e-4,  0.999936657524};
	double w, y, z;
	int i;

	if(u == 0.)	{ return 0.5; }
	y = u / 2.;
	if(y < -6.) { return 0.; }
	if(y > 6.) { return 1.; }
	if(y < 0.) { y = - y; }
	if(y < 1.) {
		w = y * y;
		z = a[0];
		for(i = 1; i < 9; i++)	{ z = z * w + a[i]; }
		z *= (y * 2.);
	} else {
		y -= 2.;
		z = b[0];
		for(i = 1; i < 15; i++) { z = z * y + b[i]; }
	}

	if(u < 0.) { return (1. - z) / 2.; }
	return (1. + z) / 2.;
}