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

numBoards = numNodes / nodesPerBoard
if numNodes % nodesPerBoard:
	numBoards += 1 

numCabs = numBoards / boardsPerCab
if numBoards % boardsPerCab:
	numCabs += 1

#print numNodes, nodesPerBoard, boardsPerCab

tmp = 0
if os.environ.has_key('POWERAPI_DEBUG'):
	tmp = int(os.environ['POWERAPI_DEBUG'])

if tmp & 2 :
	logging.basicConfig( format='%(levelname)s: %(message)s', level=logging.DEBUG )

#setupDebug()

Debug( 'PYTHONPATH=\'{0}\''.format(os.environ['PYTHONPATH']) )
Debug( 'POWERAPI_DEBUG=\'{0}\''.format( tmp ) )

Debug( 'nodesPerBoard={0} boardsPerCab={1}'.\
				format(nodesPerBoard, boardsPerCab) )
Debug( 'numCabs={0} numBoards={1} numNodes={2}'.\
				format(numCabs, numBoards, numNodes) )

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

objMap = {} 

objMap['node']  = Foo( 'node', numNodes, 0 )
objMap['board'] = Foo( 'board', numBoards, nodesPerBoard, objMap['node'] )
objMap['cab']   = Foo( 'cab', numCabs, boardsPerCab, objMap['board'] )
objMap['plat']  = Foo( 'plat', 1, numCabs, objMap['cab']  )

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

def splitUp( name ):
    pre = ''
    post = name
    pos = name.rfind('.')	

    if pos > -1:
        pre = name[0:pos]
        post = name[pos+1:]

    y = re.findall( r'[^0123456789]+|[0123456789]+',post)
    type = y[0]
    num = 0	
    if 2 == len(y): 
        num = int( y[1] )

    return pre, type, num 

def calcTypePos( map, name, wantType ):
    vals = name.split('.')
    Debug( 'calcTypePos() name={0} wantType={1}'.format(name, wantType) )

    ret = 0 
    for val in vals:
    	y = re.findall( r'[^0123456789]+|[0123456789]+',val)
    	type = y[0] 
        if wantType == type:
            break;
        pos = 0
        if 2 == len(y):		
        	pos = int(y[1])
        
        Debug( 'calcTypePos() type={0} pos={1}'.format(type, pos) ) 
    	numLeaves = map[type].getLeafCount( wantType )

        ret += numLeaves * pos 
        Debug( 'calcTypePos() numLeaves={0} ret={1}'.format(numLeaves,ret) )

    return ret 

def calcNumChildren( name ):

    pre, type, num = splitUp(name);

    Debug("calcNumChildren() name=\'{0} type={1} num={2}\'".\
			format(name,type,num)) 

    myNum = calcTypePos( objMap, name, type ) + num	

    totalChildren = objMap[type].getTotalChildren()
    per = objMap[type].getChildrenPer()

    Debug('calcNumChildren() totalChildren={0} myNum={1} childrenPer={2}'\
			.format(totalChildren,myNum,per))

    ret = 0

    if (myNum + 1) * per <= totalChildren:
        ret = per
    else:
        if (myNum + 1) * per < totalChildren + per:
            ret = totalChildren % per 
        else:
            print 'ERROR: {0} is out of range'.format(name)
            sys.exit(-1)

    Debug("calcNumChildren() name=\'{0} numChildren={1}\'".\
			format( name, ret ) )
    return ret


platform.setChild( cabinet, calcNumChildren )
cabinet.setChild( board, calcNumChildren )
board.setChild( node, calcNumChildren )
	
def genRouteFile( fileName ):
	#setupDebug()

	Debug( 'genRouteFile() numBoards={0}'.format( numBoards) )

	f = open(fileName, 'w' )
	for i in xrange( numBoards ):
        
		if i * objMap['board'].getChildrenPer() >= numNodes:
			break;	

		pos = 0
		cab = i / objMap['cab'].getChildrenPer()  
		brd = i % objMap['cab'].getChildrenPer() 

		f.write('plat.cab{0}.board{1}:{2}:{3}\n'.format( cab, brd, i, pos ) );
		pos += 1 

		for j in xrange( nodesPerBoard ):
			if i * nodesPerBoard + j < numNodes:
				f.write('plat.cab{0}.board{1}.node{2}:{3}:{4}\n'.\
										format( cab, brd, j, i, pos ) );
				pos += 1


	f.close()

def setLocations():
		
	for cab in xrange(0,numCabs):

		for brd in xrange(0,boardsPerCab):

			if cab * boardsPerCab + brd < numBoards:
				tmp = 'plat.cab%d.board%d' % ( cab, brd ) 
				setLocation( tmp, tmp + '-daemon' )

				for node in xrange(0,nodesPerBoard): 

					x = cab * boardsPerCab * nodesPerBoard + brd * nodesPerBoard + node

					if x < numNodes:
						tmp = 'plat.cab%d.board%d.node%d' % (cab, brd, node )
						setLocation( tmp, tmp + '-daemon' )

setLocations()

#print 'volta.py end' 
