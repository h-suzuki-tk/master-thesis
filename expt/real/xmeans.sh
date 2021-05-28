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
run CAS 196902 2
run NE 123593 2
run RR 257942 2
