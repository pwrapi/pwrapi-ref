#!/usr/bin/python

from pyConfig import * 

logging.basicConfig( format='%(levelname)s: %(message)s', level=logging.INFO )

plugins['CrayXTPM'] = 'libpwr_xtpmdev' 

devices['XTPM-node'] = ['CrayXTPM','']

platform = Object( Platform, 'plat' )
platform.setAttrOp( Energy, Sum, Float )
platform.setAttrOp( Power, Avg, Float )
platform.setAttrOp( MaxPower, Avg, Float )

cabinet = Object( Cabinet, 'cab', platform )
cabinet.setAttrOp( Energy, Sum, Float )
cabinet.setAttrOp( Power, Avg, Float )
cabinet.setAttrOp( MaxPower, Avg, Float )


board = Object( Board, 'board', cabinet )
board.setAttrOp( Energy, Sum, Float )
board.setAttrOp( Power, Avg, Float )
board.setAttrOp( MaxPower, Avg, Float )

device = [ "XTPM-node", "" ] 

node = Object( Node, 'node', board )
node.setAttrOp( Energy, Sum, Float )
node.setAttrOp( Power, Avg, Float )
node.setAttrOp( MaxPower, Avg, Float )
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

setLocation( 'plat.cab0.board0', 'plat.cab0.board0-router' )
setLocation( 'plat.cab0.board0.node0', 'plat.cab0.board0.node0-router' )
setLocation( 'plat.cab0.board0.node1', 'plat.cab0.board0.node1-router' )
