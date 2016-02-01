#!/usr/bin/env python

#
# Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
# DE-AC04-94AL85000, there is a non-exclusive license for use of this work
# by or on behalf of the U.S. Government. Export of this program may require
# a license from the United States Government.
#
# This file is part of the Power API Prototype software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.
#

import pwr as pow 
import time

cntxt = pow.CntxtInit( pow.CNTXT_DEFAULT, pow.ROLE_APP, "App" )
assert( cntxt )

self = pow.CntxtGetEntryPoint( cntxt )
assert( self )

objName = pow.ObjGetTypeString( pow.ObjGetType( self ) )

print "I'm a {0}".format( objName )

parent = pow.ObjGetParent( self )
assert( 0 != parent  )

children = pow.ObjGetChildren( self );
assert( children )

for i in range(0,pow.GrpGetNumObjs(children)): 
	print "child {0}".format( pow.ObjGetName( pow.GrpGetObjByIndx( children, i ) ) ) 

retval,value,ts = pow.ObjAttrGetValue( self, pow.ATTR_VOLTAGE )
assert( retval == pow.RET_INVALID )

retval,value,ts = pow.ObjAttrGetValue( self, pow.ATTR_POWER )
assert( retval == pow.RET_SUCCESS )

retval, timevalue = pow.TimeConvert( ts )

print "ObjAttrGetValue(ATTR_POWER) value={0} ts={1}".format(value,time.ctime(timevalue))

value = 25.812;
print "ObjAttrSetValue(ATTR_POWER) value={0}".format(value)
retval = pow.ObjAttrSetValue( self, pow.ATTR_POWER, value );
assert( retval == pow.RET_SUCCESS );

name = [pow.ATTR_POWER]

status = pow.StatusCreate();

retval, value, ts = pow.ObjAttrGetValues( self, name, status )
assert( retval == pow.RET_SUCCESS )

retval, timevalue = pow.TimeConvert( ts );

print "ObjAttrGetValues(ATTR_POWER) value={0} ts={1}".format(value,time.ctime(timevalue))

value = [100.10]
print "ObjAttrSetValues(ATTR_POWER) value={0}".format(value)
retval = pow.ObjAttrSetValues( self, name, value, status )
assert( retval == pow.RET_SUCCESS );

retval, value, ts = pow.ObjAttrGetValue( self, pow.ATTR_POWER )
assert( retval == pow.RET_SUCCESS )

assert( value == 200.20 );

retval, timevalue = pow.TimeConvert( ts );
print "ObjAttrGetValue(ATTR_POWER) value={0} ts={1}".format(value,time.ctime(timevalue));

grp = pow.CntxtGetGrpByType( cntxt, pow.OBJ_CORE )
assert( grp );
assert( pow.GrpGetNumObjs( grp ) )

value = 0.1
print "GrpAttrSetValue(ATTR_POWER) value={0}\n".format(value)
retval = pow.GrpAttrSetValue( grp, pow.ATTR_POWER, value, status )
assert( retval == pow.RET_SUCCESS )

retval, value, ts = pow.ObjAttrGetValue( self, pow.ATTR_POWER )
assert( retval == pow.RET_SUCCESS )
assert( value == 0.2 );

retval, timevalue = pow.TimeConvert( ts )
print "ObjAttrGetValue(ATTR_POWER) value={0} ts={1}".format( value,time.ctime(timevalue))

core = pow.GrpGetObjByIndx( grp, 0 );
assert( core )

coreStat = pow.ObjCreateStat( core, pow.ATTR_POWER, pow.ATTR_STAT_AVG )
assert( coreStat )

retval = pow.StatStart( coreStat )
assert( retval == pow.RET_SUCCESS )

time.sleep( 0.00001 ) 

retval, value, statTimes = pow.StatGetValue( coreStat )
assert( retval == pow.RET_SUCCESS )

print "StatGetValue(ATTR_POWER) value={0}".format( value )
print "StatGetValue(ATTR_POWER) start={0}".format( statTimes.start )

print "StatGetValue(ATTR_POWER) stop={0}".format( statTimes.stop )

if ( statTimes.instant != pow.TIME_NOT_SET ):
    print "StatGetValue(ATTR_POWER) instant={0}".format( statTimes.instant )

pow.StatDestroy( coreStat )

pow.CntxtDestroy( cntxt )
