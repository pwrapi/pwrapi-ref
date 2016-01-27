#!/usr/bin/python

from machine import * 

#print 'dummy.py start' 

plugins['DummyPlugin'] = 'libdummy_dev'
devices['DummyDev'] = ['DummyPlugin','']

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
	return [ [ "DummyDev", "core1" ] ] 
			
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
	
#print 'dummy.py end' 
