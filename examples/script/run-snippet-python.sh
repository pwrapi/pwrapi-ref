#!/bin/sh

#
# Copyright 2014 Sandia Corporation. Under the terms of Contract
# DE-AC04-94AL85000, there is a non-exclusive license for use of this work
# by or on behalf of the U.S. Government. Export of this program may require
# a license from the United States Government.
#
# This file is part of the Power API Prototype software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.
#

ROOT="${PWD}/../../../../.."
EXAMPLE="${PWD}/.."

export LD_LIBRARY_PATH="${ROOT}/lib:${LD_LIBRARY_PATH}"
export DYLD_LIBRARY_PATH="${ROOT}/lib:${DYLD_LIBRARY_PATH}"
export POWERAPI_CONFIG="${EXAMPLE}/config/pg-node.xml"
export POWERAPI_ROOT="plat.node"
export PYTHONPATH="${ROOT}/lib"

SAMPLES=100
FREQ=10

echo "-----------------------------------------------"
echo "Collecting ${SAMPLES} at ${FREQ} Hz"
echo "-----------------------------------------------"
#./snippet.py -s ${SAMPLES} -f ${FREQ}
${EXAMPLE}/snippet.py -h
