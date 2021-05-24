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
run UN_10K 10000 2 ${ALPHA}
run UN_50K 50000 2 ${ALPHA}
run UN_100K 100000 2 ${ALPHA}
run UN_150K 150000 2 ${ALPHA}
run UN_200K 200000 2 ${ALPHA}
