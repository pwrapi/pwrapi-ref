#!/bin/sh
ROOT=${PWD}/..

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins"
export POWERAPI_CONFIG="${ROOT}/examples/xtpm-platform.xml"
export POWERAPI_ROOT="plat.cab0"

SAMPLES=100
FREQ=10

for ATTR in E P M
do
    echo "-----------------------------------------------"
    echo "Collecting ${SAMPLES} at ${FREQ} Hz for ${ATTR}"
    echo "-----------------------------------------------"
    ./powerapi -s ${SAMPLES} -f ${FREQ} -a ${ATTR}
done
