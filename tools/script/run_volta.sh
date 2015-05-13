#!/bin/sh
ROOT=${PWD}/..
ULXMLRPC_INSTALL=${HOME}/ulxmlrpcpp-1.7.5/install

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins:${ULXMLRPC_INSTALL}/lib"
export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${ROOT}/pow:${ROOT}/plugins:${ULXMLRPC_INSTALL}/lib"

export POWERAPI_CONFIG="${ROOT}/examples/volta.xml"
#export POWERAPI_ROOT="plat.cab.board0.node1"

./server $1
