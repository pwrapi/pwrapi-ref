#! /usr/bin/python

import os, sys

#print "daemon.py start"

if not os.environ.has_key('POWERRT_MACHINE'):
    print 'POWERRT_MACHINE is not set'
    sys.exit()

machineName = os.environ['POWERRT_MACHINE'];

machine = __import__(machineName,fromlist=[''])

def calcTree( myRank, nidMap, nodesPerBoard ):
	#print "calcTree( {0} {1})".format( myRank, len(nidMap) )

	routerID = myRank/nodesPerBoard
	#print 'routerID={0}'.format( routerID )

	ret = '' 

	if len(nidMap) <= nodesPerBoard:
		return ""

	if 0 == routerID:
		otherHostPort = 16000
		numLinks = len(nidMap) / nodesPerBoard + len(nidMap) % nodesPerBoard
		for i in xrange( 1, numLinks ):
			#print 'link', i
			myListenPort = 16000 + i
			other = i * nodesPerBoard
			otherHost = nidMap[other]
			ret += ' --rtr.routerInfo={0}:{1}:{2}:{3}:{4}:{5}'.\
					format( i, myListenPort, otherHost, otherHostPort, i, i)
	else:
		
		myListenPort = 16000
		otherHost = nidMap[0]
		otherHostPort = 16000 + routerID 
	
		ret += ' --rtr.routerInfo={0}:{1}:{2}:{3}'.\
					format( 0, myListenPort, otherHost, otherHostPort )

	#print 'calcTree', ret

	return ret

def calcNodeRoot( myRank, nodesPerBoard, boardsPerCab ):
	cab = myRank / ( nodesPerBoard * boardsPerCab )
	board = myRank / nodesPerBoard  
	node = myRank % nodesPerBoard 
	return 'plat.cab{0}.board{1}.node{2}'.format(cab,board,node)

def calcBoardRoot( myRank, nodesPerBoard ):
	return 'plat.cab0.board'+str(myRank/nodesPerBoard)

def configRtr( myRank, nidMap, config, routeFile, nodesPerBoard ):
	tmp = ""

	if not myRank % nodesPerBoard:
		if 0 == myRank % nodesPerBoard:
			tmp += ' --rtr.clientPort=15000'
		tmp += ' --rtr.serverPort=15001'
		tmp += ' --rtr.routerType=tree'
		tmp += calcTree( myRank, nidMap, nodesPerBoard  ) 
		tmp += ' --rtr.routerId=' + str( myRank/nodesPerBoard )
		tmp += ' --rtr.routeTable=' + routeFile
		tmp += ' --rtr.pwrApiConfig=' + config
	return tmp

def configBoard( myRank, nidMap, config, nodesPerBoard  ):
	tmp = ""
	if not myRank % nodesPerBoard:
		tmp += ' --srvr0.name=srv0'	
		tmp += ' --srvr0.rtrHost=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard  ]
		tmp += ' --srvr0.rtrPort=15001'
		tmp += ' --srvr0.pwrApiConfig=' + config 
		tmp += ' --srvr0.pwrApiRoot=' + calcBoardRoot( myRank, nodesPerBoard )
		tmp += ' --srvr0.pwrApiServer=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard]
		tmp += ' --srvr0.pwrApiServerPort=15000'
	return tmp


def configNode( myRank, nidMap, config, nodesPerBoard, boardsPerCab ):
	tmp = ""
	tmp += ' --srvr.name=srvr'	
	tmp += ' --srvr.rtrHost=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard ]
	tmp += ' --srvr.rtrPort=15001'
	tmp += ' --srvr.pwrApiConfig=' + config
	tmp += ' --srvr.pwrApiRoot=' + calcNodeRoot( myRank, nodesPerBoard, boardsPerCab )
	return  tmp

def initDaemon( myRank, nidMap, config, routeFile ):
	#print "nidmap={0} myRank={1}".format( nidMap, myRank)

	nodesPerBoard = machine.nodesPerBoard 
	boardsPerCab = machine.boardsPerCab 

	tmp = '/home/mjleven/pwrGIT/working/build/install/bin/pwrdaemon'

	tmp += configRtr( myRank, nidMap, config, routeFile,nodesPerBoard )
	tmp += configNode( myRank, nidMap, config, nodesPerBoard, boardsPerCab )
	tmp += configBoard( myRank, nidMap, config, nodesPerBoard )

	return tmp	


def initClient( myRank, nidMap, config, routeFile ):
	tmp = "/home/mjleven/pwrGIT/working/build/examples/simpleTest"
	tmp += " plat"
	#tmp += " plat.cab0"
	#tmp += " plat.cab0.board0"
	#tmp += " plat.cab0.board0.node0"
	tmp += " 0"
	return tmp

#print "daemon.py end"
