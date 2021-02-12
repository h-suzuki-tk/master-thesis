// --------------------------------------------------
//  data.h
//  - データの作成、編集、出力やファイルへの読み書きを扱う
// --------------------------------------------------
#ifndef _DATA_H_
#define _DATA_H_

#include <iostream>
#include <random>
#include <fstream>
#include <opencv2/core.hpp>

#include <Eigen/Core>
#include <Eigen/LU>

#define FLOAT_DATA_TYPE	CV_32FC1
#define INT_DATA_TYPE		CV_32SC1


namespace HS {

cv::Mat readData(std::string dataPath);
int readData(Eigen::MatrixXd *buf, const std::string& dataPath, const int& dataSize, const int& dataDim);
int readData(std::vector<std::vector<double>>* buf, const std::string& dataPath, const int& dataSize, const int& dataDim);
int writeData(cv::Mat dataSet, std::string fileName);
int writeData(Eigen::MatrixXd dataSet, std::string fileName);

int createRandomData(char *fileName, int n_row, int n_col, double min, double max);

int arrangeClustersToClusSet(cv::Mat clusters, std::vector<std::vector<int>>& clusSet);
void arrangeClustersToClusSetWithParentIndex(std::vector<int> parentClusSet, cv::Mat clusters, std::vector<std::vector<int>>& clusSet);
void cvToEigenVec(cv::Mat cvVec, Eigen::VectorXd& eigenVec);
Eigen::VectorXd randomVector(const int n_dim, const double min = 0.0, const double max = 1.0);

void showClusSet(std::vector<std::vector<int>> clusSet);
template <class T> void printVector(const std::vector<T>& vec);

}


template <class T>
void HS::printVector(const std::vector<T>& vec) {
	auto itr = vec.begin();
	std::cout << *itr;
	while (++itr != vec.end()) { std::cout << ", " << *itr; }
}

template <class T, size_t Dim>
class HS::MultiVector : public std::vector<HS::MultiVector<T, Dim-1> > {
	public:
		template <typename N, typename... Sizes>
		HS::MultiVector(T i, N n, Sizes... sizes) : std::vector<HS::MultiVector<T, Dim-1> >(n, HS::MultiVector<T, Dim-1>(i, sizes...)) { }
};

template <class T>
class HS::MultiVector<T, 1> : public std::vector<T> {
	public:
		template <typename N>
		HS::MultiVector(T i, N n) : std::vector<T>(n, i) { }
};

#endif