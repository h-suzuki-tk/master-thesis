#include <Eigen/Core>

#include "calc.hpp"
#include "data.hpp"
#include "basic-dnnhs.hpp"
#include "grid-dnnhs.hpp"

#define BASIC "basic"
#define GRID  "grid"
#define SPLIT "split"

static const std::vector<std::string> _clustWays = {
	BASIC, GRID, SPLIT
};

void printArgError();


int main (int argc, char *argv[]) {

    // コマンドライン引数のチェック
    std::vector<std::string> args(argv, argv+argc);
    if (args.size() <= 5 || args.size() >= 8
        || !std::any_of(_clustWays.begin(), _clustWays.end(), [&](std::string way){return way==args[1];})
        || !std::all_of(args[3].begin(), args[3].end(), isdigit)
        || !std::all_of(args[4].begin(), args[4].end(), isdigit)
		|| !std::all_of(args[5].begin(), args[5].end(), isdigit)
        ) {
        printArgError();
        return 1;
    }
	if (args[1] == GRID && (args.size() != 7 
		|| !std::all_of(args[6].begin(), args[6].end(), isdigit)
		|| stoi(args[6]) <= 0)) { 
		printArgError();
		return 1;
	}
    const std::string clustWay = args[1];
    const std::string dataPath = args[2];
    const int         dataSize = stoi(args[3]);
    const int         dataDim  = stoi(args[4]);
	const double      alpha    = stod(args[5]);
	const int         gridSize   = args[1] == GRID ? stoi(args[6]) : 0;


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
	if (clustWay == BASIC) {

		HS::BasicDNNHSearch bds;

		bds.setData(dataPath, dataSize, dataDim);
		bds.setQuery(query);
		
		std::cout << "Starting basic DNNH search ..." << std::endl;
		bds.run(alpha);
		std::cout << "... Ended." << std::endl << std::endl;
		
		bds.printResult();

	} else if (clustWay == GRID) {
		
		// グリッド手法
		HS::GridDNNHSearch gds; /******/
		gds.setData(dataPath, dataSize, dataDim, gridSize); /******/
		//gds.setQuery(query); /******/
		std::cout << "Starting grid DNNH search ..." << std::endl;
		gds.run(alpha); /*****/
		std::cout << "... Ended." << std::endl << std::endl;
		//gds.printResult(); /******/

	} else if (clustWay == SPLIT) {
		
		// 分割フィルタリング手法
		std::cout << "スプリット手法です。" << std::endl;

	} else { assert(!"clustWay should be one of _clustWays."); }


	
	

    return 0;
}

void printArgError() {
    std::cerr << 
        "Params:\n" <<
        "# Way of search (string)\n" <<
            "\t- " << BASIC << ": Naive basic\n" <<
            "\t- " << GRID << " : Grid\n" <<
            "\t- " << SPLIT << ": Split filtering\n" <<
        "# Dataset file path (string)\n" <<
        "# Dataset size (int)\n" <<
        "# Dataset dimension (int)\n" <<
		"# [Only grid way] Grid size (int)"
        << std::endl;
}
