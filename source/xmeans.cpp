#include "xmeans.hpp"

//
// コンストラクタ
//
Xmeans::Xmeans(cv::Mat dataSet, int K_init, bool isCovIncluded) {
	assert(K_init >= 2);
	assert(dataSet.rows >= 3 && dataSet.cols > 0);
	
	// 初期化
	_cv_dataSet = dataSet;
	_K_init = K_init;
	_isCovIncluded = isCovIncluded;
	_centers = Eigen::MatrixXd(dataSet.rows, dataSet.cols);	// 行数は効率化できる
	
	// データセットを Eigen型に変換
	_eigen_dataSet = Eigen::MatrixXd(_cv_dataSet.rows, _cv_dataSet.cols);
	cv::cv2eigen(_cv_dataSet, _eigen_dataSet);
}

// --------------------------------------------------
//
// run
//
// --------------------------------------------------
void Xmeans::run() {
	
	// パラメータの確認
	checkParams();
	
	// 最初の k-means++ (kの指定特になければ2)
	cv::Mat clusters, centers;
	cv::kmeans(_cv_dataSet, _K_init, clusters, cv::TermCriteria(CV_TERMCRIT_ITER, 50, 1.0), KMEANS_ATTEMPTS, cv::KMEANS_PP_CENTERS, centers);
	
	// 型変換
	/*
	 * [centers] cv::Mat >> Eigen::Matrix
	 * [clusters] cv::Mat >> std::vector<std::vector<int> >
	 */
	Eigen::MatrixXd eigen_div_centers(_K_init, _cv_dataSet.cols);
	cv::cv2eigen(centers, eigen_div_centers);
	
	std::vector<std::vector<int> > div_clusSet(_K_init);
	HS::arrangeClustersToClusSet(clusters, div_clusSet);
	
	//std::cout << "cv_centers:\n" << centers << std::endl;
	//std::cout << "eigen_centers:\n" << eigen_div_centers << std::endl;
	
	//std::cout << "clusters:\n" << clusters << std::endl;
	//showClusSet(div_clusSet);
	
	// 結果のクラスタそれぞれに対し split
	for (int i=0; i<_K_init; i++) {
		split(div_clusSet[i], eigen_div_centers.row(i));
	}
	//std::cout << _total << std::endl;
}

// --------------------------------------------------
//
// split
//
// --------------------------------------------------
void Xmeans::split(std::vector<int> cluster, Eigen::VectorXd center) {
	
	//std::cout << "Split." << std::endl;
	//std::cout << cluster.size() << ", ";
	
	if (cluster.size() <= 3) {
		_clusSet.insert(_clusSet.end(), cluster);
		return;
	}

	// 与えられたクラスタを cv::Mat型の行列に変換
	cv::Mat cv_cluster(cluster.size(), _cv_dataSet.cols, FLOAT_DATA_TYPE);
	for (int i=0; i<cluster.size(); i++) {
		for (int j=0; j<_cv_dataSet.cols; j++) {
			cv_cluster.at<float>(i, j) = _cv_dataSet.at<float>(cluster[i], j);
		}
	}
	
	// 与えられたクラスタを 2分割
	cv::Mat clusters, centers;
	_start = clock();
	cv::kmeans(cv_cluster, K_split, clusters, cv::TermCriteria(CV_TERMCRIT_ITER, 50, 1.0), KMEANS_ATTEMPTS, cv::KMEANS_PP_CENTERS, centers);
	_end = clock();
	_total += static_cast<double>(_end - _start) / CLOCKS_PER_SEC * 1000.0;

	
	// 型変換
	/*
	 * [centers] cv::Mat >> Eigen::Matrix
	 * [clusters] cv::Mat >> std::vector<std::vector<int> >
	 */
	Eigen::MatrixXd eigen_div_centers(K_split, _cv_dataSet.cols);
	cv::cv2eigen(centers, eigen_div_centers);
	
	std::vector<std::vector<int> > div_clusSet(K_split);
	HS::arrangeClustersToClusSetWithParentIndex(cluster, clusters, div_clusSet);
	
	//std::cout << "* cluster: "; for (int i=0; i<cluster.size(); i++) { std::cout << cluster[i] << ", "; }
	//std::cout << "* div_clusSet:" << std::endl; showClusSet(div_clusSet);
	
	// BIC の計算と判定
	/*
	 * 分割前の bic と分割後の bic を比較し
	 * 分割前の bic が大きければ分割後がよいとして split を繰り返す
	 * 分割前の bic が小さければ分割前がよいとして終了
	 */
	double bic1 = bic(cluster, center);
	double bic2 = div_bic(div_clusSet, eigen_div_centers);
	
	//std::cout << "* BIC, div_BIC: " << bic1 << ", " << bic2 << std::endl;
	
	if (bic1 > bic2) {
		for (int i=0; i<K_split; i++) {
			split(div_clusSet[i], eigen_div_centers.row(i));
		}
	} else {
		insertCluster(cluster, center);
	}
	
}

// --------------------------------------------------
//
// bic
//
// --------------------------------------------------
double Xmeans::bic(std::vector<int> cluster, Eigen::VectorXd center) {
	
	//std::cout << "* log_likelihood: " << log_likelihood(cluster, center) << std::endl;
	
	return -2 * log_likelihood(cluster, center) + clusDf(_cv_dataSet.cols) * log(cluster.size());
}

// --------------------------------------------------
//
// div_bic
//
// --------------------------------------------------
double Xmeans::div_bic(std::vector<std::vector<int> > clusSet, Eigen::MatrixXd centers) {
	
	// クラスタサイズの合計を求める
	int sumClusSize = clusSet[0].size() + clusSet[1].size();
	
	// クラスタデータを Eigen型 の行列にまとめる
	Eigen::MatrixXd clus1(clusSet[0].size(), _cv_dataSet.cols);
	Eigen::MatrixXd clus2(clusSet[1].size(), _cv_dataSet.cols);
	for (int i=0; i<clusSet[0].size(); i++) {
		clus1.row(i) << _eigen_dataSet.row(clusSet[0][i]);
	}
	for (int i=0; i<clusSet[1].size(); i++) {
		clus2.row(i) << _eigen_dataSet.row(clusSet[1][i]);
	}
	
	//showClusSet(clusSet);
	
	// 分散共分散行列を求める
	Eigen::MatrixXd var_cov1 = Eigen::MatrixXd::Zero(_cv_dataSet.cols, _cv_dataSet.cols);
	Eigen::MatrixXd var_cov2 = Eigen::MatrixXd::Zero(_cv_dataSet.cols, _cv_dataSet.cols);
	if (_isCovIncluded == true) {
		Eigen::MatrixXd devs1 = ( clus1.rowwise() - centers.row(0) ).transpose();
		Eigen::MatrixXd devs2 = ( clus2.rowwise() - centers.row(1) ).transpose();
		var_cov1 << ( devs1 * devs1.transpose() ) / clus1.rows();
		var_cov2 << ( devs2 * devs2.transpose() ) / clus2.rows();
		
	} else {
		for (int i=0; i<_cv_dataSet.cols; i++) {
			var_cov1(i, i) = (clus1.col(i).array() - centers(0, i)).pow(2).sum() / clusSet[0].size();
			var_cov2(i, i) = (clus2.col(i).array() - centers(1, i)).pow(2).sum() / clusSet[1].size();
		}
	}
	
	//std::cout << "* clus1:\n" << clus1 << std::endl;
	//std::cout << "* clus2:\n" << clus2 << std::endl;
	
	//std::cout << "* var_cov1:\n" << var_cov1 << std::endl;
	//std::cout << "* var_cov2:\n" << var_cov2 << std::endl;
	
	// 尤度を求める
	double beta = sqrt( (centers.row(0) - centers.row(1)).squaredNorm() / (var_cov1.determinant() + var_cov2.determinant()) );
	double alpha = 0.5 / norm_cdf(beta);
	double div_log_likelihood = sumClusSize * log(alpha) + log_likelihood(clusSet[0], centers.row(0)) + log_likelihood(clusSet[1], centers.row(1));
	
	//std::cout << "* centers: \n" << centers << std::endl;
	//std::cout << "* centers squaredNorm: " << (centers.row(0) - centers.row(1)).squaredNorm() << std::endl;
	
	//std::cout << "* beta: " << beta << std::endl;;
	//std::cout << "* alpha: " << alpha << std::endl;
	//std::cout << "* div_log_likelihood: " << div_log_likelihood << std::endl;
	
	return -2 * div_log_likelihood + (2 * clusDf(_cv_dataSet.cols)) * log(sumClusSize);
}

// --------------------------------------------------
//
// log_likelihood
//
// --------------------------------------------------
double Xmeans::log_likelihood(std::vector<int> cluster, Eigen::VectorXd center) {
	
	//std::cout << "* center:\n" << center << std::endl;
	
	// クラスタデータを Eigen型 の行列にまとめる
	Eigen::MatrixXd eigen_cluster(cluster.size(), _cv_dataSet.cols);
	for (int i=0; i<cluster.size(); i++) {
		eigen_cluster.row(i) << _eigen_dataSet.row(cluster[i]);
	}
	
	// 分散共分散(var_cov)の計算
	Eigen::MatrixXd var_cov = Eigen::MatrixXd::Zero(_cv_dataSet.cols, _cv_dataSet.cols);
	if (_isCovIncluded == true) {
		Eigen::MatrixXd devs = ( eigen_cluster.rowwise() - center.transpose() ).transpose();
		var_cov << ( devs * devs.transpose() ) / eigen_cluster.rows();
		
	} else {
		for (int i=0; i<_cv_dataSet.cols; i++) {
			var_cov(i, i) = (eigen_cluster.col(i).array() - center(i)).pow(2).sum() / cluster.size();
		}
	}
	
	//std::cout << "* var_cov:\n" << var_cov << std::endl;
	
	// 対数確率密度(logpdf)の計算
	Eigen::VectorXd logpdf(cluster.size());
	Eigen::MatrixXd inv_var_cov = var_cov.inverse();
	Eigen::MatrixXd dev(_cv_dataSet.cols, 1);
	const double C = -1.0/2 * ( _cv_dataSet.cols*log(2*M_PI) + log(var_cov.determinant()) );
	for (int i=0; i<cluster.size(); i++) {
		dev << ( eigen_cluster.row(i) - center.transpose() ).transpose();
		logpdf(i) = -1.0/2 * (dev.transpose() * inv_var_cov * dev).value() + C;
	}
	
	// 対数尤度は対数確率密度の総和
	return logpdf.sum();
}

// --------------------------------------------------
//
// clusDf
//
// --------------------------------------------------
int Xmeans::clusDf(int n_dim) {
	
	switch (_isCovIncluded) {
		case true:
			return n_dim * (n_dim + 3) / 2.0;
		
		case false:
			return 2 * n_dim;
	}
}

// --------------------------------------------------
//
// checkParams
//
// --------------------------------------------------
void Xmeans::checkParams() {
	
	assert(_cv_dataSet.rows >= 0);
	
	if (_cv_dataSet.rows == 0) {
		std::cout << "不正なプロパティです。" << std::endl;
		return;
	}
}

// --------------------------------------------------
//
// insertCluster
//
// --------------------------------------------------
void Xmeans::insertCluster(std::vector<int> cluster, Eigen::VectorXd center) {
	
	//std::cout << "Insert.\n" << std::endl;
	//std::cout << cluster.size() << ", ";
	
	int endIdx = _clusSet.size();
	_centers.row(endIdx) << center.transpose();
	_clusSet.insert(_clusSet.end(), cluster);
}

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

double Xmeans::sd(Eigen::MatrixXd data, Eigen::VectorXd center) {
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
double Xmeans::norm_cdf(double u) {
	
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


// --------------------------------------------------
//  sim
//	- 総合近似度を計算
// --------------------------------------------------
double Xmeans::sim(cv::Mat& dataset, cv::Mat query, std::vector<int> clus, cv::Mat center) {
	
	double dist, max_dist = -DBL_MAX;
	std::valarray<double> s1(dataset.cols), s2(dataset.cols);
	
	for (int i = 0; i < clus.size(); i++) {
		for (int ii=0; ii < dataset.cols; ii++) {
			s1[ii] = dataset.at<float>(clus[i], ii);
		}
		
		for (int j = i+1; j < clus.size(); j++) {
			for (int jj=0; jj < dataset.cols; jj++) {
				s2[jj] = dataset.at<float>(clus[j], jj);
			}
			
			dist = (pow(s1-s2, 2)).sum();
			if (dist > max_dist) {
				max_dist = dist;
			}
		}
	}
	
	return cv::norm(query, center) + max_dist/clus.size();
}

double Xmeans::sim(cv::Mat& dataset, Eigen::VectorXd query, std::vector<int> clus, Eigen::VectorXd center) {
	
	Eigen::MatrixXd cluster(clus.size(), dataset.cols);
	for (int i=0; i<clus.size(); i++) {
		for (int j=0; j<dataset.cols; j++) {
			cluster(i, j) = dataset.at<float>(clus[i], j);
		}
	}
	
	return (query-center).norm() + sd(cluster, center);
}

