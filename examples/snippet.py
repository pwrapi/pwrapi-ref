#!/usr/bin/env python

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

import pow
import time
import getopt

samples = 1
freq = 1

opts, args = getopt.getopt( "s:f:h", [] )
for opt, arg in opts:
    if opt == "-h":
        print "usage: snippet [-s samples] [-f freq]"
        sys.exit()
    elif opt == "-s":
        samples = arg
    elif opt == "-f":
        freq = arg

cntxt = pow.CntxtInit( pow.CNTXT_DEFAULT, pow.ROLE_APP, "App" )
self = pow.CntxtGetEntryPoint( cntxt )

for sample in range( samples ):
    retval,value,ts = pow.ObjAttrGetValue( self, pow.ATTR_TEMP )
    print "{} {}".format(value,ts)

    time.sleep( 1 / freq )

pow.CntxtDestroy( cntxt )
