#ifndef _XMEANS_H_
#define _XMEANS_H_

// 型
#include <Eigen/Core>
#include <Eigen/LU>
#include <valarray>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/eigen.hpp>

// その他
#include <cmath>
#include <random>

// 自作
#include "data.hpp"

class Xmeans{
	
	public:		
		std::vector<std::vector<int> >	_clusSet;
		Eigen::MatrixXd					_centers;
		
		Xmeans(cv::Mat dataSet, int K_init = 2, bool isCovIncluded = false);
		void run();
		
	private:
		cv::Mat			_cv_dataSet;
		Eigen::MatrixXd	_eigen_dataSet;
		int _K_init;
		bool _isCovIncluded;
		int K_split = 2;
		const int KMEANS_ATTEMPTS = 5;
		
		clock_t _start, _end;
		double _total;
		
		void split(std::vector<int> cluster, Eigen::VectorXd center);
		double bic(std::vector<int> cluster, Eigen::VectorXd center);
		double div_bic(std::vector<std::vector<int> > clusSet, Eigen::MatrixXd centers);
		double log_likelihood(std::vector<int> cluster, Eigen::VectorXd center);
		int clusDf(int n_dim);
		void checkParams();
		void insertCluster(std::vector<int> cluster, Eigen::VectorXd center);

		public:
		double static sd(Eigen::MatrixXd data, Eigen::VectorXd center);
		double static norm_cdf(double u);
		double static sim(cv::Mat& dataset, cv::Mat query, std::vector<int> clus, cv::Mat center);
		double static sim(cv::Mat& dataset, Eigen::VectorXd query, std::vector<int> clus, Eigen::VectorXd center);
};

#endif