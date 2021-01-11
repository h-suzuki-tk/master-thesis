// --------------------------------------------------
//  data.h
//  - データの作成、編集、出力やファイルへの読み書きを扱う
// --------------------------------------------------
#ifndef _DATA_H_
#define _DATA_H_

#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>

#include <Eigen/Core>
#include <Eigen/LU>

#define DATA_DIR	"./data"
#define FLOAT_DATA_TYPE	CV_32FC1
#define INT_DATA_TYPE		CV_32SC1

namespace HS {

cv::Mat readData(std::string dataPath);
int readData(Eigen::MatrixXd *buf, std::string dataPath, int dataSize, int dataDim, char split = ',');
int writeData(cv::Mat dataSet, std::string fileName);
int writeData(Eigen::MatrixXd dataSet, std::string fileName);

int createRandomData(char *fileName, int n_row, int n_col, double min, double max);

int arrangeClustersToClusSet(cv::Mat clusters, std::vector<std::vector<int>>& clusSet);
void arrangeClustersToClusSetWithParentIndex(std::vector<int> parentClusSet, cv::Mat clusters, std::vector<std::vector<int>>& clusSet);
void showClusSet(std::vector<std::vector<int>> clusSet);
void cvToEigenVec(cv::Mat cvVec, Eigen::VectorXd& eigenVec);

}

#endif