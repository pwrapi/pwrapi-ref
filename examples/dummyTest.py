#!/usr/bin/env python

import pow 

print "hello"
print pow.PWR_CNTXT_DEFAULT

cntxt = pow.PWR_CntxtInit( pow.PWR_CNTXT_DEFAULT, pow.PWR_ROLE_APP, "App" )

self = pow.PWR_CntxtGetEntryPoint( cntxt )

objName = pow.PWR_ObjGetTypeString( pow.PWR_ObjGetType( self ) )

print "I'm a {}".format( objName )

pow.PWR_CntxtDestroy( cntxt )
