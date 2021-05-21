#! /bin/bash
cd $(dirname $0)

EXE=../../dnnhs;
DATA_DIR=../../data/test;
TIME_LOG_DIR=../log/test/time;
SEARCH_LOG_DIR=../log/test/search;
EXETIME=../exetime.sh

TIME_LOG_FILE=${TIME_LOG_DIR}/grid.txt;
SEARCH_LOG_FILE=${SEARCH_LOG_DIR}/grid.txt;

mkdir -p ${TIME_LOG_DIR}
mkdir -p ${SEARCH_LOG_DIR}
: > ${TIME_LOG_FILE}
: > ${SEARCH_LOG_FILE}

# パラメータ
# --------------------------------------------------
METHOD=grid;
DATA_FILE=${DATA_DIR}/RN_100K_2KC.csv;
SIZE=100000;
DIMS=2;
ALPHA=2;
GRID_DATA_FILE=${DATA_DIR}/RN_100K_2KC_g100.csv;
GRID_SIZE=100;
EXE_NUM=100;

# 実行
# --------------------------------------------------
CMD="${EXE} ${METHOD} ${DATA_FILE} ${SIZE} ${DIMS} ${ALPHA} ${GRID_DATA_FILE} ${GRID_SIZE}";
./${EXETIME} "${CMD}" ${SEARCH_LOG_FILE} ${EXE_NUM} &> ${TIME_LOG_FILE}
