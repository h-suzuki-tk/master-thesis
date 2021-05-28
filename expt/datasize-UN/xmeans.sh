#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=xmeans;
EXE_NUM=100;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# 実行
run UN_10K 10000 2
run UN_50K 50000 2
run UN_100K 100000 2
run UN_150K 150000 2
run UN_200K 200000 2
