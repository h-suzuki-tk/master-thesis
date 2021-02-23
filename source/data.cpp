#include "data.hpp"


cv::Mat readData(std::string dataPath) {
	
	cv::Mat dataSet;
	char splt = ','; // 区切り文字
	
	// 読み込みモードでファイルをオープン
	std::ifstream ifs(dataPath);
	if (ifs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		exit(1);
	}
	
	// 行数、列数を調べる
	std::string 	line;
	int 			row_num, col_num = 0;
	getline(ifs, line);
	sscanf(line.c_str(), "%d,%d", &row_num, &col_num);
	
	// 領域の確保
	dataSet = cv::Mat::zeros(row_num, col_num, FLOAT_DATA_TYPE);
	
	// 読み込みと格納
	int i = 0;
	while (getline(ifs, line)) {
		
		std::vector<float> data;
		std::istringstream ss{line};
		std::string		 buf;
		while (getline(ss, buf, splt)) {
			data.push_back(atof(buf.c_str()));
		}
		
		for (int j=0; j < col_num; j++) {
			dataSet.at<float>(i, j) = data[j];
		}
		i++;
	}

	ifs.close();

	return dataSet;
}


int HS::readData(
	Eigen::MatrixXd*   buf, 
	const std::string& dataPath, 
	const int&         dataSize, 
	const int&         dataDim) {
	
	*buf = Eigen::MatrixXd(dataSize, dataDim);

	// 読み込みモードでファイルをオープン
	std::ifstream ifs(dataPath);
	if (ifs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		return 1;
	}
	
	// 読み込みと格納
	int i = 0;
	int j = 0;
	std::string line;
	std::string s;
	while (getline(ifs, line)) {

		j=0;
		std::istringstream ss(line);
		while (getline(ss, s, ',')) {
			(*buf)(i, j) = stod(s);
			j++;
		}
		i++;

	}

	// データサイズや次元が合っているかチェック
	if (i != dataSize || j != dataDim) {
		std::cerr << "!Uncorrect data size or dimension!" << std::endl;
		return 1;
	}

	ifs.close();

	return 0;
}	


int HS::readData(
	std::vector<std::vector<double>>* buf, 
	const std::string&                dataPath, 
	const int&                        dataSize, 
	const int&                        dataDim) {
			
	// 読み込みモードでファイルをオープン
	std::ifstream ifs(dataPath);
	if (ifs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		return 1;
	}
	
	// 読み込みと格納
	int i = 0;
	int j = 0;
	std::string line;
	std::string s;
	std::vector<double> vec;
	while (getline(ifs, line)) {

		j = 0;
		std::istringstream ss(line);
		while (getline(ss, s, ',')) {
			vec.emplace_back(stod(s));
			j++;
		}
		buf->emplace_back(vec);
		
		vec.clear();
		i++;
	}

	// データサイズや次元が合っているかチェック
	if (i != dataSize || j != dataDim) {
		std::cerr << "!Uncorrect data size or dimension!" << std::endl;
		return 1;
	}

	ifs.close();

	return 1;
}	


int HS::readData(
	std::vector<std::vector<int>>* buf, 
	const std::string&             dataPath, 
	const int&                     dataSize, 
	const int&                     dataDim) {
			
	// 読み込みモードでファイルをオープン
	std::ifstream ifs(dataPath);
	if (ifs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		return 1;
	}
	
	// 読み込みと格納
	int i = 0;
	int j = 0;
	std::string line;
	std::string s;
	std::vector<int> vec;
	while (getline(ifs, line)) {

		j = 0;
		std::istringstream ss(line);
		while (getline(ss, s, ',')) {
			vec.emplace_back(stoi(s));
			j++;
		}
		buf->emplace_back(vec);
		
		vec.clear();
		i++;
	}

	// データサイズや次元が合っているかチェック
	if (i != dataSize || j != dataDim) {
		std::cerr << "!Uncorrect data size or dimension!" << std::endl;
		return 1;
	}

	ifs.close();

	return 0;
}	


int writeData(
	cv::Mat     dataSet, 
	std::string outPath) {
	
	// 引数チェック
	if (dataSet.rows <= 0 || dataSet.cols <= 0) {
		std::cerr << "ERROR(in function " << __func__ << " line " << __LINE__ << "): uncorrect value." << std::endl;
		return 1;
	}
	
	// 書き込みモードでファイルをオープン
	std::ofstream ofs(outPath);
	if (ofs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		exit(1);
	}
	
	// 行数、列数を書き込み
	ofs << dataSet.rows << "," << dataSet.cols << std::endl;
	
	// データを書き込み
	for (int i=0; i < dataSet.rows; i++) {
		for (int j=0; j < dataSet.cols-1; j++) {
			ofs << dataSet.at<float>(i, j) << ",";
		}
		ofs << dataSet.at<float>(i, dataSet.cols-1);
		ofs << std::endl;
	}
	
	ofs.close();
	return 0;
}


int writeData(
	Eigen::MatrixXd dataSet, 
	std::string outPath) {
	
	// 引数チェック
	if (dataSet.rows() <= 0 || dataSet.cols() <= 0) {
		std::cerr << "ERROR(in function " << __func__ << " line " << __LINE__ << "): uncorrect value." << std::endl;
		return 1;
	}
	
	// 書き込みモードでファイルをオープン
	std::ofstream ofs(outPath);
	if (ofs.fail()) {
		std::cerr << "Failed to open file." << std::endl;
		exit(1);
	}
	
	// 行数、列数を書き込み
	ofs << dataSet.rows() << "," << dataSet.cols() << std::endl;
	
	// データを書き込み
	for (int i=0; i < dataSet.rows(); i++) {
		for (int j=0; j < dataSet.cols()-1; j++) {
			ofs << dataSet(i, j) << ",";
		}
		ofs << dataSet(i, dataSet.cols()-1);
		ofs << std::endl;
	}
	
	ofs.close();
	return 0;
}

// --------------------------------------------------
//  createRandomData
//  - 一様分布の乱数行列データを作成・書き出し
// --------------------------------------------------
int createRandomData(char *fileName, int n_row, int n_col, double min, double max) {
	
	cv::RNG gen(cv::getTickCount());
	cv::Mat mat(n_row, n_col, FLOAT_DATA_TYPE);
	gen.fill(mat, cv::RNG::UNIFORM, cv::Scalar(min), cv::Scalar(max));
	writeData(mat, fileName);
	
	return 0;
}

// --------------------------------------------------
//  arrangeClustersToClusSet
//  - cv::kmeans等で出力されるクラスタリング結果をvector型に変換
// --------------------------------------------------
int arrangeClustersToClusSet(cv::Mat clusters, std::vector<std::vector<int>>& clusSet) {
	
	for (int i = 0; i < clusters.rows; i++) {	
		
		int clus_no = clusters.at<int>(i);
		assert(clus_no < clusSet.size());
		clusSet[clus_no].push_back(i);
	}

	return 0;
}

// --------------------------------------------------
//  arrangeClustersToClusSetWithParentIndex
//  - 部分的なデータのクラスタリング結果をもとのデータのインデックスを用いてvector型に変換
// --------------------------------------------------
void arrangeClustersToClusSetWithParentIndex(std::vector<int> parentClusSet, cv::Mat clusters, std::vector<std::vector<int>>& clusSet) {
	
	assert(parentClusSet.size() == clusters.rows);
	
	for (int clus_no=0, i=0; i<clusters.rows; i++) {
		
		clus_no = clusters.at<int>(i);
		assert(clus_no < clusSet.size());
		
		clusSet[clusters.at<int>(i)].push_back(parentClusSet[i]);
	}
}

// --------------------------------------------------
//  showClusSet
//  - vector型のクラスタセットを出力
// --------------------------------------------------
void showClusSet(std::vector<std::vector<int>> clusSet) {
	
	assert(clusSet.size() >= 0);
	
	printf("[");
	switch (clusSet.size()) {
		
		case 0:
			break;
			
		case 1:
			for (int i = 0; i < clusSet[0].size() - 1; i++) {
				printf("%d, ", clusSet[0][i]);
			}
			printf("%d", clusSet[0][clusSet[0].size() - 1]);
			break;
			
		default:
			for (int i = 0; i < clusSet[0].size(); i++) {
				printf("%d, ", clusSet[0][i]);
			}
			printf("\n");
			for (int i = 1; i < clusSet.size() - 1; i++) {
				printf(" ");
				for (int j = 0; j < clusSet[i].size(); j++) {
					printf("%d, ", clusSet[i][j]);
				}
				printf("\n");
			}
			printf(" ");
			for (int i = 0; i < clusSet[clusSet.size() - 1].size() - 1; i++) {
				printf("%d, ", clusSet[clusSet.size() - 1][i]);
			}
			printf("%d", clusSet[clusSet.size() - 1][clusSet[clusSet.size() - 1].size() - 1]);
			break;
	}
	printf("]\n");
	
}

void cvToEigenVec(cv::Mat cvVec, Eigen::VectorXd& eigenVec) {
	
	assert(cvVec.rows == 1 || cvVec.cols == 1);
	assert(cvVec.rows == eigenVec.size() || cvVec.cols == eigenVec.size());

	if (cvVec.rows == 1) {
		for (int i=0; i<eigenVec.size(); i++) {
			eigenVec(i) = cvVec.at<float>(0, i);
		}
	} else if (cvVec.cols == 1) {
		for (int i=0; i<eigenVec.size(); i++) {
			eigenVec(i) = cvVec.at<float>(i, 0);
		}
	} else {
		exit(1);
	}
}

Eigen::VectorXd HS::randomVector(const int n_dim, const double min, const double max) {
	assert(n_dim > 0);
	assert(min <= max);

	Eigen::VectorXd vec;

	std::random_device seed;
    std::mt19937 rand(seed());
    std::uniform_real_distribution<> unif(min, max);
    vec = Eigen::VectorXd::Zero(n_dim).unaryExpr([&](double dummy) { return unif(rand); });

	return vec;
}


bool combInc(
	std::vector<int>&       comb, 
	const std::vector<int>& lower, 
	const std::vector<int>& upper) {

	assert(comb.size() == lower.size());
	assert(comb.size() == upper.size());

	int scan_pos = 0;
	while ( scan_pos < comb.size() ) {
		
		if ( comb[scan_pos] < upper[scan_pos] ) {
			++comb[scan_pos];
			break;

		} else if ( comb[scan_pos] == upper[scan_pos] ) {
			comb[scan_pos] = lower[scan_pos];

		} else { assert(true); }
	
		++scan_pos;
	}

	bool is_covered;
	if ( scan_pos == comb.size() ) {
		is_covered = true;

	} else if ( scan_pos < comb.size() ) {
		is_covered = false;

	} else { assert(true); };

	return is_covered; 
}

