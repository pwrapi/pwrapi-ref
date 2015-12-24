#!/usr/bin/python

import os
os.environ['PYTHONPATH']='/home/mjleven/pwrTest'
os.environ['POWERAPI_DEBUG']='15'
import teller
	
#print teller.findChildren( "plat" )

#print teller.findPlugins()
#print teller.findSysDevs()

teller.genRouteFile( 'foobar' )
#print getObjType( "plat.cab0" )

#print findChildren( "plat.cab0" )
#print findParent("plat.cab0" )
#print findObjLocation("plat.cab0.board0" )
print teller.findAttrDevs("plat.cab0.board0.node0",teller.Energy )
#print teller.findAttrChildren("plat.cab0",teller.Energy )
#print teller.findAttrChildren("plat.cab0.board0",teller.Energy )
#print findAttrOp("plat.cab0",Energy )
#print findAttrType("plat.cab0",Energy )

#plat.cab0.board0 attr=ENERGY

#print findAttrDevs("plat.cab0.board0.node0",Energy )
#print findAttrDevs("plat.cab0.board0.node1",Energy )
#print findAttrDevs("plat.cab0.board0.node2",Energy )
#print findAttrDevs("plat.cab0.board0.node3",Energy )
#print findAttrDevs("plat.cab0.board2.node0",Energy )

#for i in xrange( numCabs ):
#    tmp = 'plat.cab%d' % i
#    print tmp, i
#    setLocation( tmp, tmp + '-daemon' )
