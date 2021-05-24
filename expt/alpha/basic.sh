#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=basic;
EXE_NUM=10;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# 実行
run UN_100K 100000 2 1
run UN_100K 100000 2 1.5
run UN_100K 100000 2 2
run UN_100K 100000 2 3
run UN_100K 100000 2 5

run RN_50P_100K_1S 100000 2 1 
run RN_50P_100K_1S 100000 2 1.5
run RN_50P_100K_1S 100000 2 2
run RN_50P_100K_1S 100000 2 3
run RN_50P_100K_1S 100000 2 5 
