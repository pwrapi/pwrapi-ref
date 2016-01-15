#!/usr/bin/python

from pyConfig import * 
import os,string

#print 'volta.py start' 

if not os.environ.has_key('POWERRT_NUMNODES'):
	print 'POWERRT_NUMNODES is not set'
	sys.exit() 

if not os.environ.has_key('POWERRT_NODES_PER_BOARD'):
	print 'POWERRT_NODES_PER_BOARD is not set'
	sys.exit()

if not os.environ.has_key('POWERRT_BOARDS_PER_CAB'):
	print 'POWERRT_BOARDS_PER_CAB is not set'
	sys.exit() 

numNodes = int( os.environ['POWERRT_NUMNODES'] )
nodesPerBoard = int( os.environ['POWERRT_NODES_PER_BOARD'] )
boardsPerCab = int( os.environ['POWERRT_BOARDS_PER_CAB'] )


tmp = 0
if os.environ.has_key('POWERAPI_DEBUG'):
    tmp = int(os.environ['POWERAPI_DEBUG'])

if tmp & 2 :
	logging.basicConfig( format='%(levelname)s: %(message)s', \
							level=logging.DEBUG )

Debug( 'PYTHONPATH=\'{0}\''.format(os.environ['PYTHONPATH']) )
Debug( 'POWERAPI_DEBUG=\'{0}\''.format( tmp ) )
Debug( 'numNodes={0} nodesPerBoard={1} boardsPerCab={2}'.\
				format(numNodes, nodesPerBoard, boardsPerCab) )

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

map = {} 

map['board'] = Foo( numNodes, nodesPerBoard )
map['cab'] = Foo( map['board'].foo(), boardsPerCab, map['board'] )
map['plat'] = Foo( map['cab'].foo(), map['cab'].foo(), map['cab'] )  

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

def calcNumChildren( name ):
    Debug("calcNumChildren() name=\'{1}\'".format( "", name ) )
    val = name.split('.')[-1]
			
    objType = ''.join(ch for ch in val if ch.isalpha() )

    num = ''.join(ch for ch in val if ch.isdigit() )

    if len(num) == 0: 
        num = int(0)
    else:
        num = int(num)

    if not map.has_key( objType): 
        print "ERROR: can't find objType \'{0}\'".format( objType )
        sys.exit()

    return map[objType].calc(num) 

platform.setChild( cabinet, calcNumChildren )
cabinet.setChild( board, calcNumChildren )
board.setChild( node, calcNumChildren )
	
Debug( "numCabs=%d numBoards=%d numNodes=%d" % \
			( map['cab'].foo(), map['board'].foo(), numNodes ) )

def genRouteFile( fileName ):
	numBoards = map['board'].foo()

	f = open(fileName, 'w' )
	for i in xrange( numBoards ):
		pos = 0

		f.write('plat.cab0.board{0}:{0}:{1}\n'.format( i, pos ) );
		pos += 1 

		for j in xrange( nodesPerBoard ):
			if i * nodesPerBoard + j < numNodes:
				f.write('plat.cab0.board{1}.node{0}:{1}:{2}\n'.\
										format( j, i, pos ) );
				pos += 1


	f.close()

for i in xrange( numNodes ):
    tmp = 'plat.cab0.board0.node%d' % i
    setLocation( tmp, tmp + '-daemon' )

for i in xrange( map['board'].foo() ):
    tmp = 'plat.cab0.board%d' % i
    setLocation( tmp, tmp + '-daemon' )

#print 'volta.py end' 
