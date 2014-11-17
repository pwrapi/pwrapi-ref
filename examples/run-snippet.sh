#!/bin/sh
ROOT=${PWD}/..

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins"
export POWERAPI_CONFIG="${ROOT}/examples/pg-node.xml"
export POWERAPI_ROOT="plat.node"

SAMPLES=100
FREQ=10

for ATTR in P
do
    echo "-----------------------------------------------"
    echo "Collecting ${SAMPLES} at ${FREQ} Hz for ${ATTR}"
    echo "-----------------------------------------------"
    ./snippet -s ${SAMPLES} -f ${FREQ}
done
