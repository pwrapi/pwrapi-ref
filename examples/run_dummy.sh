#!/bin/sh
ROOT=${PWD}/..

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins"
export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins"
export POWERAPI_CONFIG="${ROOT}/examples/dummySystem.xml"
export POWERAPI_ROOT="plat.cab0.board0.node0"

./dummyTest
