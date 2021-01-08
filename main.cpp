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

#include "calc.hpp"
#include "data.hpp"
#include "basic.hpp"
#include "grid.hpp"
#include "split.hpp"

#define BASIC 'basic'
#define GRID 'grid'
#define SPLIT 'split'

int main (int argc, char *argv[]) {

    // 引数のチェック
    if (argc != 4) {
        std::cerr << 
            "Params:\n
            # Way of search (string)\n
                \t-" << BASIC << ": Naive basic
                \t-" << GRID << ": Grid
                \t-" << SPLIT << " 'split': Split filtering
            # Dataset file path (string)\n
            # Dataset size (int)" 
            << std::endl;
        return 1;
    }
    /***** !ここで, 各引数 (アルゴリズムとサイズ) が不正でないか確認! *****/
    std::string ClustWay    = (std::string)argv[1];
    std::string DataPath    = (std::string)argv[2];
    int DataSize            = atoi(argv[3]);

    // 変数宣言

    // 初期化
    //// データセットの読み込み
    //// クエリの設定

    // DNNH 検索
    /***** !switch が文字列で使えなさそうなら if を使う! *****/
    switch (ClustWay) {
    case BASIC:
        break;
    case GRID:
        break;
    case SPLIT:
        break;
    default:
        break;
    }

    // 出力
    std::cout << "Hello, world!" << std::endl;

    return 0;
}