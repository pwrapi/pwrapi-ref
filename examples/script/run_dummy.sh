#!/bin/sh

#
# Copyright 2014-2015-2015 Sandia Corporation. Under the terms of Contract
# DE-AC04-94AL85000, there is a non-exclusive license for use of this work
# by or on behalf of the U.S. Government. Export of this program may require
# a license from the United States Government.
#
# This file is part of the Power API Prototype software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.
#

ROOT=${HOME}/pwrGIT/working


export LD_LIBRARY_PATH="${ROOT}/build/install/lib:${LD_LIBRARY_PATH}"
export DYLD_LIBRARY_PATH="${ROOT}/build/install/lib:${DYLD_LIBRARY_PATH}"

export POWERAPI_DEBUG=0
export POWERAPI_ROOT="plat.cab0.board0"
#export POWERAPI_CONFIG="${ROOT}/examples/config/dummySystem.xml"

export POWERRT_MACHINE=dummy 
export POWERAPI_CONFIG=${ROOT}/examples/config/$POWERRT_MACHINE.py

export PYTHONPATH=${ROOT}/examples/config/
export POWERRT_NUMNODES=4
export POWERRT_NODES_PER_BOARD=5
export POWERRT_BOARDS_PER_CAB=2

#export POWERAPI_ROOT="plat"
#export POWERAPI_SERVER="localhost"
#export POWERAPI_SERVER_PORT="5000"

$ROOT/build/examples/dummyTest
