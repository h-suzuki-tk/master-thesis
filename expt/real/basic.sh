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
run CAS 196902 2 ${ALPHA}
run NE 123593 2 ${ALPHA}
run RR 257942 2 ${ALPHA}
