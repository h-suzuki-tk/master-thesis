#! /bin/bash
NEED_ARG_NUM=3;

if [ $# -ne ${NEED_ARG_NUM} ] 
then

	echo "Arguments:"
	echo "* Command"        # $1
	echo "* Log file path"  # $2
	echo "* Execution time" # $3

else

	CMD=$1
	LOG_FILE=$2
	EXE_NUM=$3

	: > ${LOG_FILE}
	echo -e "$(date)\n" >> ${LOG_FILE}

	echo -e "$(date)\n"
	echo "Arguments:"
	echo "* Command: ${CMD}"
	echo "* Log file path: ${LOG_FILE}"
	echo "* Execution time: ${EXE_NUM}"

	time ( i=1; while [ ${i} -le ${EXE_NUM} ]; 
		do 
		${CMD} &>> ${LOG_FILE}
		echo -e -n "\n\n\n" &>> ${LOG_FILE}; 
		i=$(expr ${i} + 1); done
	);

	echo -e -n "\n"

fi
