#!/bin/sh
PWD=`pwd`

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$PWD/..:$PWD/../plugins"
export POWERAPI_CONFIG="$PWD/teller.xml"
export POWERAPI_ROOT="plat.cab2.board2.node1"

./example
