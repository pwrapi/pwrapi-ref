#!/usr/bin/python

import os
#os.environ['PYTHONPATH']='/home/mjleven/pwrTest'
#os.environ['POWERAPI_DEBUG']='15'
#os.environ['SLURM_NODELIST']='nid[0-7]'
#os.environ['SLURM_JOB_NUM_NODES']='1'


os.environ['POWERRT_NUMNODES']='29'
os.environ['POWERRT_NODES_PER_BOARD']='4'
os.environ['POWERRT_BOARDS_PER_CAB']='3'
os.environ['POWERAPI_DEBUG']='15'

machine = __import__('cray-xt',fromlist=[''])

#print volta.findChildren( "plat" )

#print volta.findPlugins()
#print volta.findSysDevs()

#print machine.calcNumChildren( 'plat' )
#print machine.calcNumChildren( 'plat.cab0' )
#print machine.calcNumChildren( 'plat.cab101' )
print machine.calcNumChildren( 'plat.cab2.board0' )
#print machine.calcNumChildren( 'plat.cab1.board3.node0' )

machine.genRouteFile( 'voltaTestRoute.txt' )
#print getObjType( "plat.cab0" )

#print findChildren( "plat.cab0" )
#print findParent("plat.cab0" )
#print findObjLocation("plat.cab0.board0" )
#print machine.findAttrDevs("plat.cab0.board0.node0",machine.Energy )
#print volta.findAttrChildren("plat.cab0",volta.Energy )
#print volta.findAttrChildren("plat.cab0.board0",volta.Energy )
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
