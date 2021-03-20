#!/bin/bash

path=$(dirname -- $(readlink -fn -- "$BASH_SOURCE"))
MULTI_CH_HV_SW_PATH=`dirname ${path}`

echo $MULTI_CH_HV_SW_PATH
export MULTI_CH_HV_SW_PATH
