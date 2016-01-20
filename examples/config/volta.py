#!/usr/bin/python

from machine import * 

#print 'volta.py start' 

plugins['CrayXTPM'] = 'libpwr_xtpmdev'
devices['XTPM-node'] = ['CrayXTPM','']

energy = Attr( Sum, Float )

platform = Object( Platform, 'plat' )
platform.setAttr( Energy, energy )

cabinet = Object( Cabinet, 'cab', platform )
cabinet.setAttr( Energy, energy )

board = Object( Board, 'board', cabinet )
board.setAttr( Energy, energy )

node = Object( Node, 'node', board )

nodeVoltage = Attr( None, Float )
nodeCurrent = Attr( None, Float )
nodeEnergy = Attr( Sum, Float )

node.setAttr( Energy, nodeEnergy )
node.setAttr( Voltage, nodeVoltage )
node.setAttr( Current, nodeCurrent )

def calcAttrSrc( name ):
	Debug("calcAttrSrc() name=\'{1}\'".format( "", name ) )
	return [ [ "XTPM-node", "" ] ] 
			
node.addAttrDevice( Voltage, calcAttrSrc ) 
node.addAttrDevice( Current, calcAttrSrc )
node.addAttrDevice( Energy, calcAttrSrc )

objectMap['plat'] = platform
objectMap['cab'] = cabinet
objectMap['board'] = board
objectMap['node'] = node

platform.setChild( cabinet, calcNumChildren )
cabinet.setChild( board, calcNumChildren )
board.setChild( node, calcNumChildren )
	
#print 'volta.py end' 
