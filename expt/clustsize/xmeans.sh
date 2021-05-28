#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=xmeans;
EXE_NUM=10;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# 実行
run RN_10P_100K_1S 100000 2
run RN_50P_100K_1S 100000 2
run RN_200P_100K_1S 100000 2
run RN_500P_100K_1S 100000 2
run RN_1KP_100K_1S 100000 2
