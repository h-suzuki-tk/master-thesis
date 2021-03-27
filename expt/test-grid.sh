#! /bin/bash
cd $(dirname $0)

EXE=../dnnhs;
DATA_DIR=../data;
TIME_LOG_DIR=log/time;
SEARCH_LOG_DIR=log/search;
EXETIME=exetime.sh

TIME_LOG_FILE=${TIME_LOG_DIR}/test.txt;
SEARCH_LOG_FILE=${SEARCH_LOG_DIR}/test.txt;

mkdir -p ${TIME_LOG_DIR}
mkdir -p ${SEARCH_LOG_DIR}
: > ${TIME_LOG_FILE}
: > ${SEARCH_LOG_FILE}

# パラメータ
# --------------------------------------------------
METHOD=grid;
DATA_FILE=${DATA_DIR}/SN_01.csv;
SIZE=230;
DIMS=2;
ALPHA=2;
GRID_DATA_FILE=${DATA_DIR}/SN_01_g10.csv;
GRID_SIZE=10;
EXE_NUM=5;

# 実行
# --------------------------------------------------
CMD="${EXE} ${METHOD} ${DATA_FILE} ${SIZE} ${DIMS} ${ALPHA} ${GRID_DATA_FILE} ${GRID_SIZE}";
./${EXETIME} "${CMD}" ${SEARCH_LOG_FILE} ${EXE_NUM} &> ${TIME_LOG_FILE}
