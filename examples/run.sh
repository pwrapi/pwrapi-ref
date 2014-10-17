#!/bin/sh
PWD=`pwd`

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$PWD/..:$PWD/../plugins"
export POWERAPI_CONFIG="$PWD/pg-node.xml"
export POWERAPI_ROOT="plat.node0"

SAMPLES=100
FREQ=10

for ATTR in E P F T
do
    echo "-----------------------------------------------"
    echo "Collecting ${SAMPLES} at ${FREQ} Hz for ${ATTR}"
    echo "-----------------------------------------------"
    ./powerapi -s ${SAMPLES} -f ${FREQ} -a ${ATTR}
done
