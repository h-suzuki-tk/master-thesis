#! /bin/bash

# 初期化用関数の定義
# Params: 
#     $1: method
#     $2: execusion time
#     ($3: log directory)
init () {
	# 基本設定
	ROOT=`git rev-parse --show-toplevel`
	EXE=${ROOT}/dnnhs;
	DATA_DIR=${ROOT}/data;
	EXETIME=${ROOT}/expt/exetime.sh;

	# パラメータの取得
	METHOD=$1;
	EXE_NUM=$2;

	# ログディレクトリの設定・初期化
	if [ $# -eq 3 ]; then
		LOG_DIR=$3;
	else
		LOG_DIR=log/${METHOD};
	fi
	TIME_LOG_DIR=${LOG_DIR};
	SEARCH_LOG_DIR=${LOG_DIR}/$(date '+%y%m%d-%H%M%S');
	mkdir -p ${TIME_LOG_DIR}
	mkdir -p ${SEARCH_LOG_DIR}

	# 実行時間記録ファイルの設定・初期化
	TIME_LOG_FILE=${TIME_LOG_DIR}/$(date '+%y%m%d-%H%M%S').txt;
	cat <<- EOS > ${TIME_LOG_FILE}
		Date:           $(date)
		Machine:        $(hostname)
		Method:         ${METHOD}
		Execusion time: ${EXE_NUM}

	EOS
}

# 実行用関数の定義
# Params: 
#     $1: data name
#     $2: data size
#     $3: data dims
#     $4: alpha
#     $5: grid size (for grid method)
run () {
	# パラメータの取得
    DATA=$1;
    SIZE=$2;
	DIMS=$3;
	ALPHA=$4;
	if [ ${METHOD} = grid ]; then
		GRID_SIZE=$5;
	fi

    # データファイルの設定
    DATA_FILE=${DATA_DIR}/${DATA}.csv;
	if [ ${METHOD} = grid ]; then
		GRID_DATA_FILE=${DATA_DIR}/grid/${DATA}_${GRID_SIZE}G.csv;
	fi

    # 検索結果記録ファイルの設定・初期化
    SEARCH_LOG_FILE=${SEARCH_LOG_DIR}/${DATA}.txt;
    : > ${SEARCH_LOG_FILE}

    # 実行
	if [ ${METHOD} = basic ]; then
    	CMD="${EXE} ${METHOD} ${DATA_FILE} ${SIZE} ${DIMS} ${ALPHA}";
	elif [ ${METHOD} = grid ]; then
		CMD="${EXE} ${METHOD} ${DATA_FILE} ${SIZE} ${DIMS} ${ALPHA} ${GRID_DATA_FILE} ${GRID_SIZE}";	
	fi
    ${EXETIME} "${CMD}" ${SEARCH_LOG_FILE} ${EXE_NUM} &>> ${TIME_LOG_FILE}
}
