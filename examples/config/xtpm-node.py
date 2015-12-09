#!/usr/bin/python

from pyConfig import * 

plugins['CrayXTPM'] = 'libpwr_xtpmdev' 

devices['XTPM-node'] = ['CrayXTPM','']

platform = Object( Platform, 'plat' )
platform.setAttrOp( Energy, Sum )
platform.setAttrOp( Power, Avg )
platform.setAttrOp( MaxPower, Avg )

cabinet = Object( Cabinet, 'cab', platform )
cabinet.setAttrOp( Energy, Sum )
cabinet.setAttrOp( Power, Avg )
cabinet.setAttrOp( MaxPower, Avg )


board = Object( Board, 'board', cabinet )
board.setAttrOp( Energy, Sum )
board.setAttrOp( Power, Avg )
board.setAttrOp( MaxPower, Avg )

device = [ "XTPM-node", "" ] 

node = Object( Node, 'node', board )
node.setAttrOp( Energy, Sum )
node.setAttrOp( Power, Avg )
node.setAttrOp( MaxPower, Avg )
node.addAttrDevice( Energy, device ) 
node.addAttrDevice( Power, device )
node.addAttrDevice( MaxPower, device )

platform.setChild( cabinet, 1 )
cabinet.setChild( board, 1 )
board.setChild( node, 2 )

objectMap['plat'] = platform
objectMap['cab'] = cabinet
objectMap['board'] = board
objectMap['node'] = node

locationMap['plat.cab0.board0'] = 'plat.cab0.board0-router'
#locationMap['plat.cab0.board1'] = 'plat.cab0.board0-router'

#print findObjLocation( "plat.cab0.board0.node0" )

#print findAttrChildren( "plat", "ENERGY" )
#print findAttrChildren( "plat.cab0", "ENERGY" )

#tmp = findAttrChildren( "plat.cab0.board0.node1", "ENERGY" )
#if not tmp:
#	print findAttrDevs( "plat.cab0.board0.node1", "ENERGY" )

#print findAttrOp( "plat", "ENERGY" )
#print findAttrOp( "plat.cab0", "ENERGY" )
#print findAttrOp( "plat.cab0.board0", "ENERGY" )
#print findAttrOp( "plat.cab0.board0.node0", "ENERGY" )

#print findSysDevs()

#print findPlugins()
#print getObjType( "plat.node0" )
#print findObjDevs( "plat.node0", "Energy" )
#print findChildren( "plat" )

#print getObjType( "plat" )
