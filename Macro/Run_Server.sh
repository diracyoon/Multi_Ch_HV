#!/bin/bash

source ${MULTI_CH_HV_SW_PATH}/Macro/TTY_Finder.sh $1

rm -f ${MULTI_CH_HV_SW_PATH}/FIFOs/FIFO
${MULTI_CH_HV_SW_PATH}/Bin/Server $tty $mode

