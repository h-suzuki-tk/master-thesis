#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=grid;
EXE_NUM=10;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# パラメータ設定
ALPHA=2;
GRID_SIZE=100;
cat <<- EOS >> ${TIME_LOG_FILE}
	Alpha:     ${ALPHA}
	Grid size: ${GRID_SIZE}

EOS

# 実行
run CAS 196902 2 ${ALPHA} ${GRID_SIZE}
run NE 123593 2 ${ALPHA} ${GRID_SIZE}
run RR 257942 2 ${ALPHA} ${GRID_SIZE}
