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
void runXmeansSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query);
void runBasicSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const double& alpha);
void runGridSearch(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const double& alpha, const int& gridSize, const int queryId);
void runSplitSearch();

std::chrono::system_clock::time_point g_start, g_end, g_s, g_e;
double                                g_total;

int main (int argc, char *argv[]) {

    // コマンドライン引数のチェック
    std::vector<std::string> args(argv, argv+argc);
	if (args.size() < 2 
		|| !std::any_of(_clustWays.begin(), _clustWays.end(), [&](std::string way){return way==args[1];})
		) {
		printArgError();
		return 1;
	}
	if (args[1] == XMEANS && ( args.size() != 5
		|| !std::all_of(args[3].begin(), args[3].end(), isdigit)
        || !std::all_of(args[4].begin(), args[4].end(), isdigit)
		)) {
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
    const int         dataSize     = stoi(args[3]);
    const int         dataDim      = stoi(args[4]);
	const double      alpha        = argc >= 6 ? stod(args[5]) : -1;
	const std::string gridDataPath = argc >= 7 ? args[6]       : "";
	const int         gridSize     = argc >= 8 ? stoi(args[7]) : 0;

	
	Eigen::MatrixXd data;
	Eigen::VectorXd query;
	const int       queryId = HS::rand(0, dataSize-1);

	g_start = std::chrono::system_clock::now();

	// データ, クエリの読み込み
	try { 
		HS::readData( &data, &query, dataPath, dataSize, dataDim, queryId );
	} catch ( const std::exception& e ) {
		std::cout << e.what() << std::endl; 
		return 1;
	}

	// 引数の出力
	std::cout << "Arguments:" << std::endl;
    std::cout << "* Clustering way: " << clustWay << std::endl;
    std::cout << "* Dataset: " << dataPath << " (" << dataSize << " pts, " << dataDim << " dims)" << std::endl;
    std::cout << "* Query: " << query.transpose() << std::endl;
	if (clustWay == GRID) {	std::cout << "* Grid size: " << gridSize << std::endl; }
	std::cout << std::endl;


	// DNNH 検索	
	if      (clustWay == XMEANS) { runXmeansSearch(data, query); }
	else if (clustWay == BASIC)  { runBasicSearch(data, query, alpha); } 
	else if (clustWay == GRID)   { runGridSearch(data, query, alpha, gridSize, queryId); } 
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
        "# Dataset size (int)\n" <<
        "# Dataset dimension (int)\n" <<
		"# [Only basic/grid way] Parameter alpha (double)\n" <<
		"# [Only grid way] Grid data file path (string)\n" <<
		"# [Only grid way] Grid size (int)"
    << std::endl;
}


void runXmeansSearch(
	const Eigen::MatrixXd& data,
	const Eigen::VectorXd& query) {
	
	// データ変換
	cv::Mat cv_data;
	cv::eigen2cv( data, cv_data );
	cv_data.convertTo( cv_data, CV_32F );
	cv_data *= 100; // OpenCV の kmeans ライブラリに適用するためのスケーリング

	// 検索
	std::cout << "Starting xmeans DNNH search..." << std::endl;
	Xmeans xmeans(cv_data, 2, 1);
	xmeans.run();
	int n_clus	= xmeans._clusSet.size();
	std::vector<std::vector<int>> clusSet = xmeans._clusSet;
	Eigen::MatrixXd _centers = Eigen::MatrixXd(cv_data.rows, cv_data.cols);
	_centers << xmeans._centers;
	double min_sim = DBL_MAX;
	int num_clus = -1;
	std::vector<int> clus;
	for (int i=0; i < clusSet.size(); i++) {
		double tmp_sim = Xmeans::sim(cv_data, query, clusSet[i], _centers.row(i));
		if (tmp_sim < min_sim) {
			num_clus	= i;
			clus		= clusSet[i];
			min_sim 	= tmp_sim;
		}
	}
	std::cout << "Ended xmeans DNNH search." << std::endl << std::endl;

	// 出力
	std::cout << "Result:" << std::endl;
	std::cout << "* Number of clusters: " << n_clus << std::endl;
	std::cout << "* DNNH (" << clus.size() << " pts): " << std::endl;
	HS::printVector(clus);
}


void runBasicSearch(
	const Eigen::MatrixXd& data,
	const Eigen::VectorXd& query,
	const double&          alpha) {
	
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
	const Eigen::MatrixXd& data,
	const Eigen::VectorXd& query,
	const double&          alpha,
	const int&             gridSize,
	const int              queryId) {
	std::chrono::system_clock::time_point    s, e;
	std::vector<std::pair<std::string, int>> time;

	// 所属セルの計算
	s = std::chrono::system_clock::now();
	std::vector<std::vector<int>> belongGrid( data.rows(), std::vector<int>(data.cols()) );
	for ( int i=0; i<data.rows(); ++i ) belongGrid[i] = HS::DNNHS::Grid::belongCell( data.row(i), gridSize );
	e = std::chrono::system_clock::now(); 
	time.emplace_back( std::make_pair("Read grid data", std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count()) );

	// 検索
	std::cout << "Starting grid DNNH search..." << std::endl;

	s = std::chrono::system_clock::now(); 
	HS::DNNHS::Grid dsg(data, query, alpha, gridSize, belongGrid);
	e = std::chrono::system_clock::now(); 
	time.emplace_back( std::make_pair("Create object", std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count()) );
	
	s = std::chrono::system_clock::now(); 
	if (dsg.run() != 0) {
		std::cerr << "[E] Failed to run grid DNNH search." << std::endl;
		return;
	}
	e = std::chrono::system_clock::now(); 
	time.emplace_back( std::make_pair("Run search", std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count()) );

	std::cout << "Ended grid DNNH search." << std::endl << std::endl;

	g_end   = std::chrono::system_clock::now();
	g_total = std::chrono::duration_cast<std::chrono::milliseconds>(g_end-g_start).count();

	// 出力
	std::cout << "Result:" << std::endl;
	std::cout << "* Lower/upper cluster size: " << dsg.LOWER_CLUSTER_SIZE << "/" << dsg.UPPER_CLUSTER_SIZE << std::endl;
	std::cout << "* Expansion count (total: " << std::accumulate( dsg.epCount().begin(), dsg.epCount().end(), 0 )  << "): \n";
	HS::printVector(dsg.epCount()); std::cout << std::endl;
	std::cout << "* Elapsed time (total: " << g_total << " ms): " << std::endl;
	for ( const auto& t : time ) {
		std::cout << t.first << ": " << t.second << " ms (" << ((t.second / g_total) * 100) << " %)" << std::endl;;
	} 
	std::cout << "* DNNH (" << dsg.result().size() << " pts): " << std::endl;
	HS::printVector(dsg.result().ids());
	std::cout << std::endl;

	
}


void runSplitSearch() { /***/ }



