// ----------------------------------------
//  DNNH を検索するプログラム
// ----------------------------------------
/* 引数
 * argv[1]: 適用するアルゴリズム
 *  - 'basic': ナイーブベーシック手法
 *  - 'grid': グリッド手法
 *  - 'split': 分割フィルタリング手法
 * argv[2]: データセットのファイルパス
 * argv[3]: データセットのサイズ
 */

#include <Eigen/Core>

#include "calc.hpp"
#include "data.hpp"
#include "dnnhs.hpp"

#define OK 1
#define NG 0

const std::vector<std::string> _clustWays = {BASIC, GRID, SPLIT};

void printArgError();

int main (int argc, char *argv[]) {

    // コマンドライン引数のチェック
    std::vector<std::string> args(argv, argv+argc);
    if (args.size() != 5 
        || !std::any_of(_clustWays.begin(), _clustWays.end(), [&](std::string way){return way==args[1];})
        || !std::all_of(args[3].begin(), args[3].end(), isdigit)
        || !std::all_of(args[4].begin(), args[4].end(), isdigit)
        ) {
        printArgError();
        return 1;
    }
    const std::string clustWay = args[1];
    const std::string dataPath = args[2];
    const int         dataSize = stoi(args[3]);
    const int         dataDim  = stoi(args[4]);

    // データセットの読み込み
    Eigen::MatrixXd data(dataSize, dataDim);
    if (HS::readData(&data, dataPath, dataSize, dataDim) == NG) {
        std::cerr << "!Cannot read \"" << dataPath << "\"!" << std::endl;
        return 1;
    }

    // DNNH 検索
    DNNHSearch ds(data);
    
    std::cout << std::endl;
    std::cout << "# Clustering way: " << clustWay << std::endl;
    std::cout << "# Dataset       : " << dataPath << " (" << dataSize << " pts, " << dataDim << " dims)" << std::endl;
    std::cout << "# Query         : \n" << ds.query().transpose() << std::endl;
    std::cout << std::endl;

    std::cout << "Starting DNNH search ..." << std::endl;
    ds.run(clustWay);
    std::cout << "... Ended." << std::endl;

    /***** 結果の出力 *****/
    /*
    std::cout << std::endl;
    std::cout << "# Result: \n" << ds.result() << std::endl;
    std::cout << std::endl;
    */

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
        "# Dataset size (int)" 
        "# Dataset dimension (int)" 
        << std::endl;
}
