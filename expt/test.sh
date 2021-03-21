#! /bin/bash
EXE=../dnnhs;
DATA_DIR=../data;
DATA_INF_DIR=${DATA_DIR}/inf;
LOG_DIR=log/time;
EXETIME=exetime.sh

METHOD=basic;
ALPHA=2;

mkdir -p ${LOG_DIR}
LOG_FILE=test.txt;
: > ${LOG_DIR}/${LOG_FILE}

DATA=SN_01;
CMD="${EXE} ${METHOD} ${DATA_DIR}/$(<${DATA_INF_DIR}/${DATA}) ${ALPHA}";
./${EXETIME} "${CMD}" ${LOG_FILE} 3 &> ${LOG_DIR}/${LOG_FILE};
