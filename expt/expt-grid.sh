#!/bin/bash
CMD=../dnnhs;
DATA_DIR=../data;

DATA_FILE_NAME=SN_01;
DATA_SIZE=230;
DATA_DIMS=2;
DATA_PATH=${DATA_DIR}/${DATA_FILE_NAME}.csv;

GRID_SIZE=10;
GRID_DATA_PATH=${DATA_DIR}/${DATA_FILE_NAME}_g${GRID_SIZE}.csv;

ALPHA=2;

set -x
${CMD} grid ${DATA_PATH} ${DATA_SIZE} ${DATA_DIMS} ${ALPHA} ${GRID_DATA_PATH} ${GRID_SIZE}
set +x
