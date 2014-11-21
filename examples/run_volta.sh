#!/bin/sh
ROOT=${PWD}/..

ULXMLRPC_INSTALL=${HOME}/ulxmlrpcpp-1.7.5/install

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins:${ULXMLRPC_INSTALL}/lib"
export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins:${ULXMLRPC_INSTALL}/lib"

export POWERAPI_CONFIG="${ROOT}/examples/volta.xml"
#export POWERAPI_ROOT="plat.cab.board0.node0"
export POWERAPI_ROOT="plat.cab.board0"

export POWERAPI_STANDALONE="no"

SAMPLES=100
FREQ=10

for ATTR in E P M
do
    echo "-----------------------------------------------"
    echo "Collecting ${SAMPLES} at ${FREQ} Hz for ${ATTR}"
    echo "-----------------------------------------------"
    ./powerapi -s ${SAMPLES} -f ${FREQ} -a ${ATTR}
done

