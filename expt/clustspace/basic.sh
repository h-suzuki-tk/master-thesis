#! /bin/bash
# 必要ファイルの読み込み
ROOT=`git rev-parse --show-toplevel`
source ${ROOT}/expt/prep.sh

# 初期化
METHOD=basic;
EXE_NUM=10;
cd $(dirname $0)
init ${METHOD} ${EXE_NUM}

# パラメータ設定
ALPHA=2;
cat <<- EOS >> ${TIME_LOG_FILE}
	Alpha: ${ALPHA}

EOS

# 実行
run RN_0.0S_100K_50P 100000 2 ${ALPHA}
run RN_0.5S_100K_50P 100000 2 ${ALPHA}
run RN_1.0S_100K_50P 100000 2 ${ALPHA}
run RN_1.5S_100K_50P 100000 2 ${ALPHA}
run RN_2.0S_100K_50P 100000 2 ${ALPHA}
run RN_2.5S_100K_50P 100000 2 ${ALPHA}
run RN_3.0S_100K_50P 100000 2 ${ALPHA}