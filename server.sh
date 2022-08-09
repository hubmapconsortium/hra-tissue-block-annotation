#!/bin/bash
set -ev

if [ ! -e server2 ]; then
  echo "Please build and place server2 binary in this directory:" `pwd`
  exit -1
fi

export PORT=${PORT:="8080"}
export HOST=${HOST:="0.0.0.0"}

./server2 data/organ_origins_meter.csv data/ASCT-B_3D_Models_Mapping.csv data/plain_filling_hole/ $HOST $PORT
