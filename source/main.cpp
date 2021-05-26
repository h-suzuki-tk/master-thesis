#include "data.hpp"
#include "xmeans.hpp"
#include "dnnhs.hpp"
#include "basic.hpp"
#include "grid.hpp"

#define XMEANS "xmeans"
#define BASIC  "basic"
#define GRID   "grid"
#define SPLIT  "split"

static const std::vector<std::string> _clustWays = {
	XMEANS, BASIC, GRID, SPLIT
};

void printArgError();
void runXmeansSearch(const std::string& dataPath, const Eigen::VectorXd& query);
void runBasicSearch(const std::string& dataPath, const int& dataSize, const int& dataDim, const Eigen::VectorXd& query, const double& alpha);
void runGridSearch(const std::string& dataPath, const int& dataSize, const int& dataDim, const Eigen::VectorXd& query, const double& alpha, const int& gridSize, const std::string& gridDataPath);
void runSplitSearch();


int main (int argc, char *argv[]) {

    // コマンドライン引数のチェック
    std::vector<std::string> args(argv, argv+argc);
	if (args.size() < 2 
		|| !std::any_of(_clustWays.begin(), _clustWays.end(), [&](std::string way){return way==args[1];})
		) {
		printArgError();
		return 1;
	}
	if (args[1] == XMEANS && args.size() != 3) {
		printArgError();
		return 1;
	}
	if (args[1] == BASIC && ( args.size() != 6
        || !std::all_of(args[3].begin(), args[3].end(), isdigit)
        || !std::all_of(args[4].begin(), args[4].end(), isdigit)
		|| !std::all_of(args[5].begin(), args[5].end(), isdigit)
		)) {
		printArgError();
		return 1;
	}
	if (args[1] == GRID && (args.size() != 8 
		|| !std::all_of(args[3].begin(), args[3].end(), isdigit)
        || !std::all_of(args[4].begin(), args[4].end(), isdigit)
		|| !std::all_of(args[5].begin(), args[5].end(), isdigit)
		|| !std::all_of(args[7].begin(), args[7].end(), isdigit)
		|| stoi(args[7]) <= 0)) { 
		printArgError();
		return 1;
	}
    const std::string clustWay     = args[1];
    const std::string dataPath     = args[2];
    const int         dataSize     = argc >= 3 ? stoi(args[3]) : -1;
    const int         dataDim      = argc >= 4 ? stoi(args[4]) : -1;
	const double      alpha        = argc >= 5 ? stod(args[5]) : -1;
	const std::string gridDataPath = argc >= 6 ? args[6]       : "";
	const int         gridSize     = argc >= 7 ? stoi(args[7]) : 0;


	// クエリの設定
	Eigen::VectorXd query = HS::randomVector(dataDim);


	// 引数の出力
	std::cout << "Arguments:" << std::endl;
    std::cout << "* Clustering way: " << clustWay << std::endl;
    std::cout << "* Dataset: " << dataPath << " (" << dataSize << " pts, " << dataDim << " dims)" << std::endl;
    std::cout << "* Query: " << query.transpose() << std::endl;
	if (clustWay == GRID) {	std::cout << "* Grid size: " << gridSize << std::endl; }
	std::cout << std::endl;


	// DNNH 検索	
	if      (clustWay == XMEANS) { runXmeansSearch(dataPath, query); }
	else if (clustWay == BASIC)  { runBasicSearch(dataPath, dataSize, dataDim, query, alpha); } 
	else if (clustWay == GRID)   { runGridSearch(dataPath, dataSize, dataDim, query, alpha, gridSize, gridDataPath); } 
	else if (clustWay == SPLIT)  { runSplitSearch(); } 
	else { assert(!"clustWay should be one of _clustWays."); }

    return 0;
}


void printArgError() {
    std::cerr << 
        "Params:\n" <<
        "# Way of search (string)\n" <<
			"\t- " << XMEANS << ": Xmeans\n" <<
            "\t- " << BASIC  << ": Naive basic\n" <<
            "\t- " << GRID   << ": Grid\n" <<
            "\t- " << SPLIT  << ": Split filtering\n" <<
        "# Dataset file path (string)\n" <<
        "# [Only basic/grid way] Dataset size (int)\n" <<
        "# [Only basic/grid way] Dataset dimension (int)\n" <<
		"# [Only basic/grid way] Parameter alpha (double)\n" <<
		"# [Only grid way] Grid data file path (string)\n" <<
		"# [Only grid way] Grid size (int)"
    << std::endl;
}


void runXmeansSearch(
	const std::string&     dataPath,
	const Eigen::VectorXd& query) {
	
	// データ読み込み・整形
	cv::Mat data = HS::readData(dataPath);

	// 検索
	std::cout << "Starting xmeans DNNH search..." << std::endl;
	Xmeans xmeans(data, 2, 1);
	xmeans.run();
	int n_clus	= xmeans._clusSet.size();
	std::vector<std::vector<int>> clusSet = xmeans._clusSet;
	Eigen::MatrixXd _centers = Eigen::MatrixXd(data.rows, data.cols);
	_centers << xmeans._centers;
	double min_sim = DBL_MAX;
	int num_clus = -1;
	std::vector<int> clus;
	for (int i=0; i < clusSet.size(); i++) {
		double tmp_sim = Xmeans::sim(data, query, clusSet[i], _centers.row(i));
		if (tmp_sim < min_sim) {
			num_clus	= i;
			clus		= clusSet[i];
			min_sim 	= tmp_sim;
		}
	}
	std::cout << "Ended xmeans DNNH search." << std::endl << std::endl;

	// 出力
	std::cout << "Result:" << std::endl;
	std::cout << "* DNNH: " << std::endl;
	HS::printVector(clusSet[num_clus]);
}


void runBasicSearch(
	const std::string&     dataPath, 
	const int&             dataSize, 
	const int&             dataDim, 
	const Eigen::VectorXd& query,
	const double&          alpha) {

	assert(dataSize > 0);
	assert(dataDim > 0);

	// データ読み込み・整形
	Eigen::MatrixXd data;
	if (HS::readData(&data, dataPath, dataSize, dataDim) != 0) {
		std::cerr << "[E] Failed to read data." << std::endl;
		return;
	}
	
	// 検索
	std::cout << "Starting basic DNNH search..." << std::endl;
	HS::DNNHS::Basic dsb(data, query, alpha);
	if (dsb.run() != 0) {
		std::cerr << "[E] Failed to run basic DNNH search." << std::endl;
		return;
	}
	std::cout << "Ended basic DNNH search." << std::endl << std::endl;

	// 出力
	std::cout << "Result:" << std::endl;
	std::cout << "* Lower/upper cluster size: " << dsb.LOWER_CLUSTER_SIZE << "/" << dsb.UPPER_CLUSTER_SIZE << std::endl;
	std::cout << "* Total processed core points (filterd): " << dsb.procdPtCount() << " (" << dsb.data().size() - dsb.procdPtCount() << ")" << std::endl;
	std::cout << "* DNNH: " << std::endl;
	HS::printVector(dsb.result().ids());
	std::cout << std::endl;
}


void runGridSearch(
	const std::string&     dataPath, 
	const int&             dataSize, 
	const int&             dataDim, 
	const Eigen::VectorXd& query,
	const double&          alpha,
	const int&             gridSize,
	const std::string&     gridDataPath) { 

	// データ読み込み・整形
	Eigen::MatrixXd data;
	if (HS::readData(&data, dataPath, dataSize, dataDim) != 0) {
		std::cerr << "[E] Failed to read data." << std::endl;
		return;
	}
	std::vector<std::vector<int>> belongGrid;
	if (HS::readData(&belongGrid, gridDataPath, dataSize, dataDim) != 0) {
		std::cerr << "[E] Failed to read grid data." << std::endl;
		return;
	}

	// 検索
	std::cout << "Starting grid DNNH search..." << std::endl;
	HS::DNNHS::Grid dsg(data, query, alpha, gridSize, belongGrid);
	if (dsg.run() != 0) {
		std::cerr << "[E] Failed to run grid DNNH search." << std::endl;
		return;
	}
	std::cout << "Ended grid DNNH search." << std::endl << std::endl;

	// 出力
	std::cout << "Result:" << std::endl;
	std::cout << "* Lower/upper cluster size: " << dsg.LOWER_CLUSTER_SIZE << "/" << dsg.UPPER_CLUSTER_SIZE << std::endl;
	std::cout << "* Expansion count (total: " << std::accumulate( dsg.epCount().begin(), dsg.epCount().end(), 0 )  << "): \n";
	HS::printVector(dsg.epCount());
	std::cout << std::endl;
	std::cout << "* DNNH (" << dsg.result().size() << " pts): " << std::endl;
	HS::printVector(dsg.result().ids());
	std::cout << std::endl;
}


void runSplitSearch() { /***/ }



