#! /usr/bin/python

import os, sys

#print "daemon.py start"

if not os.environ.has_key('POWERRT_MACHINE'):
    print 'POWERRT_MACHINE is not set'
    sys.exit()

machineName = os.environ['POWERRT_MACHINE'];

machine = __import__(machineName,fromlist=[''])

def calcTree( myNode, nidMap, nodesPerBoard ):

	routerID = myNode/nodesPerBoard
	#print "calcTree( {0} {1} {2})".format( myNode, len(nidMap), routerID )

	ret = '' 

	if len(nidMap) <= nodesPerBoard:
		return ""

	if 0 == routerID:
		otherHostPort = 16000
		#print 'len nidMap {0} nodesPerBoard {1}'.format( len(nidMap), nodesPerBoard) 
		numLinks = len(nidMap) / nodesPerBoard 
		if len(nidMap) % nodesPerBoard: 
			numLinks += 1

		for i in xrange( 1, numLinks ):
			myListenPort = 16000 + i
			other = i * nodesPerBoard
			otherHost = nidMap[other]
			tmp = ' --rtr.routerInfo={0}:{1}:{2}:{3}:{4}:{5}'.\
					format( i, myListenPort, otherHost, otherHostPort, i, i)
			ret += tmp
	else:
		
		myListenPort = 16000
		otherHost = nidMap[0]
		otherHostPort = 16000 + routerID 
	
		ret += ' --rtr.routerInfo={0}:{1}:{2}:{3}'.\
					format( 0, myListenPort, otherHost, otherHostPort )

	#print 'calcTree(',myNode,')', ret

	return ret

def calcBoardRoot( myNode, numRanks, nodesPerBoard, boardsPerCab ):

	cab = (myNode / (nodesPerBoard * boardsPerCab ) )

	board = (myNode / nodesPerBoard ) % boardsPerCab 

	#print "{0} cab={1} board={2}".format(myNode,cab,board)

	tmp = 'plat.cab{0}.board{1}'.format(cab,board)
	#print 'calcBoardRoot myNode={0} {1}'.format( myNode, tmp)
	return tmp 

def calcNodeRoot( myNode, numRanks, nodesPerBoard, boardsPerCab ):
	ret = calcBoardRoot( myNode, numRanks, nodesPerBoard, boardsPerCab ) 

	node = myNode % nodesPerBoard 

	tmp = ret + '.node{0}'.format(node)

	#print 'calcNodedRoot myNode={0} {1}'.format(myNode, tmp)
	return tmp 


def configRtr( myNode, nidMap, config, routeFile, nodesPerBoard ):
	tmp = ""

	if not myNode % nodesPerBoard:
		if 0 == myNode % nodesPerBoard:
			tmp += ' --rtr.clientPort=15000'
		tmp += ' --rtr.serverPort=15001'
		tmp += ' --rtr.routerType=tree'
		tmp += calcTree( myNode, nidMap, nodesPerBoard  ) 
		tmp += ' --rtr.routerId=' + str( myNode/nodesPerBoard )
		tmp += ' --rtr.routeTable=' + routeFile
		tmp += ' --rtr.pwrApiConfig=' + config


	return tmp

def configBoard( myNode, nidMap, config, nodesPerBoard, boardsPerCab  ):
	tmp = ""
	if not myNode % nodesPerBoard:
		tmp += ' --srvr0.name=srv0'	
		tmp += ' --srvr0.rtrHost=' + nidMap[ myNode/nodesPerBoard * nodesPerBoard  ]
		tmp += ' --srvr0.rtrPort=15001'
		tmp += ' --srvr0.pwrApiConfig=' + config 
		tmp += ' --srvr0.pwrApiRoot=' + calcBoardRoot( myNode, len(nidMap), nodesPerBoard, boardsPerCab )
		tmp += ' --srvr0.pwrApiServer=' + nidMap[ myNode/nodesPerBoard * nodesPerBoard]
		tmp += ' --srvr0.pwrApiServerPort=15000'

	#print 'configBoard(',myNode,')',tmp 

	return tmp


def configNode( myNode, nidMap, config, nodesPerBoard, boardsPerCab ):
	tmp = ""
	tmp += ' --srvr.name=srvr'	
	tmp += ' --srvr.rtrHost=' + nidMap[ myNode/nodesPerBoard * nodesPerBoard ]
	tmp += ' --srvr.rtrPort=15001'
	tmp += ' --srvr.pwrApiConfig=' + config
	tmp += ' --srvr.pwrApiRoot=' + calcNodeRoot( myNode, len(nidMap), nodesPerBoard, boardsPerCab )
	#print 'configNode(',myNode,')',tmp 
	return  tmp

def initDaemon( exe, myNode, nidMap, config, routeFile ):
	#print "nidmap={0} myNode={1}".format( nidMap, myNode)

	nodesPerBoard = machine.nodesPerBoard 
	boardsPerCab = machine.boardsPerCab 

	tmp = exe + ' '

	tmp += configRtr( myNode, nidMap, config, routeFile,nodesPerBoard )
	tmp += configNode( myNode, nidMap, config, nodesPerBoard, boardsPerCab )
	tmp += configBoard( myNode, nidMap, config, nodesPerBoard, boardsPerCab )

	#print 'initDaemon(',myNode,')',tmp
	return tmp	


def initClient( exe, object, logfile ):
	tmp = exe
	tmp += ' --object=' + object
	tmp += ' --count=0'
	tmp += ' --delay=2'
	if logfile:
		tmp += ' --logfile=' + logfile
	#print tmp
	return tmp

#print "daemon.py end"
