#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=grid;
EXE_NUM=100;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# 実行
testRun SN_01 230 2 2 10
testRun RN_100K_2KC 100000 2 2 100
testRun RN_200K_50P 200000 2 2 100
