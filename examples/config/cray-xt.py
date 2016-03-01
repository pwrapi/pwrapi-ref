#!/usr/bin/python

from machine import * 

#print 'cray-xt.py start' 

plugins['CrayXTPM'] = 'libpwr_xtpmdev'
devices['XTPM-node'] = ['CrayXTPM','']

energy = Attr( Sum, Float, 10.0 )

platform = Object( Platform, 'plat' )
platform.setAttr( Energy, energy )

cabinet = Object( Cabinet, 'cab', platform )
cabinet.setAttr( Energy, energy )

board = Object( Board, 'board', cabinet )
board.setAttr( Energy, energy )

node = Object( Node, 'node', board )

node.setAttr( Energy, Attr( Sum, Float, 10.0 ) )
node.setAttr( Power, Attr( Sum, Float, 10.0 ) )
node.setAttr( MaxPower, Attr( Sum, Float, 10.0 ) )

def calcAttrSrc( name ):
	Debug("calcAttrSrc() name=\'{1}\'".format( "", name ) )
	return [ [ "XTPM-node", "" ] ] 
			
node.addAttrDevice( Energy, calcAttrSrc ) 
node.addAttrDevice( Power, calcAttrSrc )
node.addAttrDevice( MaxPower, calcAttrSrc )

objectMap['plat'] = platform
objectMap['cab'] = cabinet
objectMap['board'] = board
objectMap['node'] = node

platform.setChild( cabinet, calcNumChildren )
cabinet.setChild( board, calcNumChildren )
board.setChild( node, calcNumChildren )
	
#print 'cray-xt.py end' 
