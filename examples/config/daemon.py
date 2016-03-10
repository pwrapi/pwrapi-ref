#! /usr/bin/python

import os, sys

#print "daemon.py start"

if not os.environ.has_key('POWERRT_MACHINE'):
    print 'POWERRT_MACHINE is not set'
    sys.exit()

machineName = os.environ['POWERRT_MACHINE'];

machine = __import__(machineName,fromlist=[''])


def calcRtrRoot( myNode, nidMap, nodesPerBoard ):

	if len(nidMap) == 1:
		return ''
	ret = ''
	
	otherHostPort = 16000
	#print 'len nidMap {0} nodesPerBoard {1}'.format( len(nidMap), nodesPerBoard) 

	numLinks = len(nidMap) / nodesPerBoard 
	if len(nidMap) % nodesPerBoard: 
		numLinks += 1

	for i in xrange( 0, numLinks):
		myListenPort = 16000 + i
		otherHost = nidMap[ i * nodesPerBoard ]
		tmp = ' --rtr.routerInfo={0}:{1}:{2}:{3}:{4}:{5}'.\
					format( i, myListenPort, otherHost, otherHostPort, i, i)
		ret += tmp

	return ret
		

def calcRtrLeaf( myNode, nidMap, nodesPerBoard, rootRtr ):

	routerID = myNode/nodesPerBoard
	#print "calcTree( {0} {1} {2})".format( myNode, len(nidMap), routerID )

	ret = '' 

	#if len(nidMap) <= nodesPerBoard:
	#	return '' 

	myListenPort = 16000
	otherHost = nidMap[rootRtr]
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


def im_a_rtr( myNode, nodesPerBoard, rootRtr):
	if myNode == rootRtr:
		return True
	
	if not myNode % nodesPerBoard:
		return True 

	return False
 
def configRtr( myNode, nidMap, config, routeFile, nodesPerBoard, rootRtr ):

	if not im_a_rtr( myNode, nodesPerBoard, rootRtr):   
		return ''

	tmp = '' 

	tmp += ' --rtr.routerType=tree'
	tmp += ' --rtr.clientPort=15000'
	tmp += ' --rtr.serverPort=15001'

	if myNode == rootRtr: 
		rtrID = len(nidMap) / nodesPerBoard

		if len(nidMap) > 1 and len(nidMap) % nodesPerBoard:
			rtrID += 1 

		tmp += ' --rtr.routerId=' + str( rtrID )
		tmp += calcRtrRoot( myNode, nidMap, nodesPerBoard ) 
	else:
		tmp += ' --rtr.routerId=' + str( myNode/nodesPerBoard )
		tmp += calcRtrLeaf( myNode, nidMap, nodesPerBoard,rootRtr  ) 

	tmp += ' --rtr.routeTable=' + routeFile

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

def initDaemon( exe, myNode, nidMap, config, routeFile, rootRtr ):
	#print "nidmap={0} myNode={1}".format( nidMap, myNode)

	nodesPerBoard = machine.nodesPerBoard 
	boardsPerCab = machine.boardsPerCab 

	#print 'rootRtr', rootRtr

	tmp = exe + ' '

	tmp += configRtr( myNode, nidMap, config, routeFile,nodesPerBoard, rootRtr )
	tmp += configNode( myNode, nidMap, config, nodesPerBoard, boardsPerCab )
	tmp += configBoard( myNode, nidMap, config, nodesPerBoard, boardsPerCab )

	#print 'initDaemon(',myNode,')',tmp
	return tmp	

def initLogger( myNode, nidMap, config, apiroot, object, attr, logfile ):

	serverHost = nidMap[myNode]
	serverPort = 15000
	nodesPerBoard = machine.nodesPerBoard 
	boardsPerCab = machine.boardsPerCab 

	tmp = ''
	tmp += ' --lgr.name=lgr'
	tmp += ' --lgr.pwrApiServer=' + serverHost
	tmp += ' --lgr.pwrApiServerPort=' + str(serverPort)
	tmp += ' --lgr.pwrApiConfig=' + config
	tmp += ' --lgr.pwrApiRoot=' + apiroot
	tmp += ' --lgr.object=' + object
	tmp += ' --lgr.count=0'
	tmp += ' --lgr.delay=0'
	if attr:
		tmp += ' --lgr.attr=' + attr 
	if logfile:
		tmp += ' --lgr.logfile=' + logfile
	#print tmp
	return tmp

def initClient( exe, object, logfile ):
	sys.exit('initClient fix me')
	tmp = exe
	tmp += ' --object=' + object
	tmp += ' --count=0'
	tmp += ' --delay=10'
	if logfile:
		tmp += ' --logfile=' + logfile
	#print tmp
	return tmp

#print "daemon.py end"
