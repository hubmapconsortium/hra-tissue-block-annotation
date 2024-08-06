#!/bin/bash
set -ev

# if [ ! -e server2 ]; then
#   echo "Please build and place server2 binary in this directory:" `pwd`
#   exit -1
# fi

export PORT=${PORT:="8080"}
export HOST=${HOST:="0.0.0.0"}

./server2 model/all_preprocessed_off_models_cgal/ model/asct-b-grlc.csv model/reference-organ-grlc.csv 0.0.0.0 8080