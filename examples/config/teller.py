#!/usr/bin/python

from pyConfig import * 
import os,string, batchUtil

logging.basicConfig( format='%(levelname)s: %(message)s', level=logging.INFO )

nidList = os.environ['SLURM_NODELIST'].split(',')

Debug( 'PYTHONPATH=\'{0}\''.format(os.environ['PYTHONPATH']) )
Debug( 'POWERAPI_DEBUG=\'{0}\''.format(os.environ['POWERAPI_DEBUG']) )

nidMap = batchUtil.createNidMap( nidList )

#print 'nidMap=', nidMap


numNodes = len(nidMap) #int(os.environ['SLURM_JOB_NUM_NODES'])

plugins['PowerInsight'] = 'libpwr_pidev' 

energy = Attr( Sum, Float )

platform = Object( Platform, 'plat' )
platform.setAttr( Energy, energy )

cabinet = Object( Cabinet, 'cab', platform )
cabinet.setAttr( Energy, energy )

board = Object( Board, 'board', cabinet )
board.setAttr( Energy, energy )

node = Object( Node, 'node', board )

nodeVoltage = Attr( None, Float )
nodeCurrent = nodeVoltage
nodeEnergy = Attr( Sum, Float )

node.setAttr( Energy, nodeEnergy )
node.setAttr( Voltage, nodeVoltage )
node.setAttr( Current, nodeCurrent )

nodesPerBoard = 4
boardsPerCab = 4

map = {} 

map['board'] = Foo( numNodes, nodesPerBoard )
map['cab'] = Foo( map['board'].foo(), boardsPerCab, map['board'] )
map['plat'] = Foo( 1, map['cab'].foo(), map['cab'] )  

def calcAttrSrc( name ):
	Debug("calcAttrSrc() name=\'{1}\'".format( "", name ) )
	val = name.split('.')
	tmp = ['PowerInsight-' + val[-1], '1']
	return [tmp] 
			
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
        sys.exit( "ERROR: can't find objType \'{0}\'".format( objType ) )

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

		f.write('plat.cab0.board{0}:{0}:1\n'.format( i, pos ) );
		pos += 1 

		for j in xrange( nodesPerBoard ):
			if i * nodesPerBoard + j < numNodes:
				f.write('plat.cab0.board0.node{0}:{1}:{2}\n'.format( j, i, pos ) );
				pos += 1


	f.close()

for i in xrange( numNodes ):
    tmp = 'plat.cab0.board0.node%d' % i
    #setLocation( tmp, tmp + '-daemon' )

for i in xrange( numNodes ):
    num = ''.join(ch for ch in nidMap[i] if ch.isdigit() )
    devices['PowerInsight-node'+str(i)] = ['PowerInsight','10.54.21.' + num + ':20201']

for i in xrange( map['board'].foo() ):
    tmp = 'plat.cab0.board%d' % i
    #if not i % nodesPerBoard:
    #    setLocation( tmp, tmp + '-daemon' )

