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
run RN_0.0S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_0.5S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_1.0S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_1.5S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_2.0S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_2.5S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_3.0S_100K_50P 100000 2 ${ALPHA} ${GRID_SIZE}
