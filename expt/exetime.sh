#! /bin/bash
LOG_DIR=log/search;
NEED_ARG_NUM=3;

if [ $# -ne ${NEED_ARG_NUM} ] 
then

	echo "Arguments:"
	echo "* Command"        # $1
	echo "* Log file name"  # $2
	echo "* Execution time" # $3

else

	CMD=$1
	LOG_FILE=$2
	EXE_NUM=$3

	mkdir -p ${LOG_DIR}
	: > ${LOG_DIR}/${LOG_FILE}

	echo "Arguments:"
	echo "* Command: ${CMD}"
	echo "* Execution time: ${EXE_NUM}"

	echo "Starting execitions ..."
	time ( i=1; while [ ${i} -le ${EXE_NUM} ]; 
		do 
		${CMD} &>> ${LOG_DIR}/${LOG_FILE}
		echo -e -n "\n\n\n" &>> ${LOG_DIR}/${LOG_FILE}; 
		i=$(expr ${i} + 1); done
	);
	echo -e "\n... ended."
	
	echo "The log was written in ${LOG_DIR}/${LOG_FILE}"
	echo -e -n "\n"

fi
