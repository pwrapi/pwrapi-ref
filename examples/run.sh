#!/bin/sh
PWD=`pwd`

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$PWD/..:$PWD/../plugins"
export POWERAPI_CONFIG="$PWD/pg-node.xml"
export POWERAPI_ROOT="plat.cab0.board0.node0"

./example
