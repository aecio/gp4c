#!/bin/sh
if [ ! $# -eq 1 ] ;
then
  echo "Usage: ${0} seed_value"
  exit
fi

RANDOM_SEED=$1
DATASET=../datasets/brdc12.cut2.changes
GP2C_EXE=../build/src/crawling
/usr/bin/time -f "time: total %E\nmemory: max %M kb" ${GP2C_EXE} ${RANDOM_SEED} ${DATASET}
