#! /usr/bin/python

nodesPerBoard = 4
boardsPerCab = 4
def calcTree( myRank, nidMap ):
	print "calcTree", len(nidMap)

	if 1 == len(nidMap):
		return ""

	if 0 == myRank:
		return ' --rtr.routerInfo={0}:{1}:{2}:{3}:{4}:{5}'.\
					format( 1, 16000, nidMap[4],16000, 1, 1)
	else:
		return ' --rtr.routerInfo={0}:{1}:{2}:{3}'.\
					format( 0, 16000, nidMap[0], 16000 )
		


def configRtr( myRank, nidMap, config, routeFile ):
	tmp = ""

	if not myRank % nodesPerBoard:
		if 0 == myRank%nodesPerBoard:
			tmp += ' --rtr.clientPort=15000'
		tmp += ' --rtr.serverPort=15001'
		tmp += ' --rtr.routerType=tree'
		tmp += calcTree( myRank, nidMap ) 
		tmp += ' --rtr.routerId=' + str( myRank/nodesPerBoard )
		tmp += ' --rtr.routeTable=' + routeFile
		tmp += ' --rtr.pwrApiConfig=' + config
	return tmp

def configBoard( myRank, nidMap, config  ):
	tmp = ""
	if not myRank % nodesPerBoard:
		tmp += ' --srvr0.name=srv0'	
		tmp += ' --srvr0.rtrHost=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard  ]
		tmp += ' --srvr0.rtrPort=15001'
		tmp += ' --srvr0.pwrApiConfig=' + config 
		tmp += ' --srvr0.pwrApiRoot=plat.cab0.board'+str(myRank/nodesPerBoard)
		tmp += ' --srvr0.pwrApiServer=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard]
		tmp += ' --srvr0.pwrApiServerPort=15000'
	return tmp

def configNode( myRank, nidMap, config ):
	tmp = ""
	tmp += ' --srvr.name=srvr'	
	tmp += ' --srvr.rtrHost=' + nidMap[ myRank/nodesPerBoard * nodesPerBoard ]
	tmp += ' --srvr.rtrPort=15001'
	tmp += ' --srvr.pwrApiConfig=' + config 
	cab = myRank / ( nodesPerBoard * boardsPerCab )
	board = myRank / nodesPerBoard  
	node = myRank % nodesPerBoard 
	tmp += ' --srvr.pwrApiRoot=plat.cab{0}.board{1}.node{2}'.format(cab,board,node)
	return  tmp

def initDaemon( myRank, nidMap, config, routeFile ):
	#print "nidmap={0} myRank={1}".format( nidMap, myRank)

	tmp = '/home/mjleven/pwrGIT/working/build/install/bin/pwrdaemon'

	tmp += configRtr( myRank, nidMap, config, routeFile )
	tmp += configNode( myRank, nidMap, config )
	tmp += configBoard( myRank, nidMap, config )

	return tmp	


def initClient( myRank, nidMap, config, routeFile ):
	tmp = "/home/mjleven/pwrGIT/working/build/examples/simpleTest"
	tmp += " plat.cab0"
	tmp += " 1"
	return tmp
