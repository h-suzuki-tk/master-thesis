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
run RN_10P_100K_1S 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_50P_100K_1S 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_200P_100K_1S 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_500P_100K_1S 100000 2 ${ALPHA} ${GRID_SIZE}
run RN_1KP_100K_1S 100000 2 ${ALPHA} ${GRID_SIZE}
