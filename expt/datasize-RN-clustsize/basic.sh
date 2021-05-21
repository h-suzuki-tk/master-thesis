#! /bin/bash
# 基本設定
ROOT=`git rev-parse --show-toplevel`
EXE=${ROOT}/dnnhs;
DATA_DIR=${ROOT}/data;
EXETIME=${ROOT}/expt/exetime.sh;

# パラメータ
METHOD=basic;
DIMS=2;
ALPHA=2;
EXE_NUM=100;

# ログディレクトリの設定・初期化
cd $(dirname $0)
TIME_LOG_DIR=log/time;
SEARCH_LOG_DIR=log/search/${METHOD};
mkdir -p ${TIME_LOG_DIR}
mkdir -p ${SEARCH_LOG_DIR}

# 実行時間記録ファイルの設定・初期化
TIME_LOG_FILE=${TIME_LOG_DIR}/${METHOD}.txt;
echo -e "$(date)\n" > ${TIME_LOG_FILE}

# 実行用関数の定義
# Params: 
#     $1: data name
#     $2: data size
run () {
    # 対象データの更新
    DATA=$1;
    SIZE=$2;

    # データファイルの設定
    DATA_FILE=${DATA_DIR}/${DATA}.csv;

    # 検索結果記録ファイルの設定・初期化
    SEARCH_LOG_FILE=${SEARCH_LOG_DIR}/${DATA}.txt;
    : > ${SEARCH_LOG_FILE}

    # 実行
    CMD="${EXE} ${METHOD} ${DATA_FILE} ${SIZE} ${DIMS} ${ALPHA}";
    ${EXETIME} "${CMD}" ${SEARCH_LOG_FILE} ${EXE_NUM} &>> ${TIME_LOG_FILE}
}

# 実行
run RN_10K_50P_1S 10000
run RN_50K_50P_1S 50000
run RN_100K_50P_1S 100000
run RN_150K_50P_1S 150000
run RN_200K_50P_1S 200000
