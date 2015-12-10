#!/usr/bin/python

from pyConfig import * 

logging.basicConfig( format='%(levelname)s: %(message)s', level=logging.INFO )

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

setLocation( 'plat.cab0.board0.node0', 'plat.cab0.board0.node0-router' )
setLocation( 'plat.cab0.board0.node1', 'plat.cab0.board0.node1-router' )
#setLocation( 'plat.cab0.board0', 'plat.cab0.board0-router' )
