#! /usr/bin/python

import sys, os, platform

#print 'exec.py start'

daemonDebug = 0#8 
clientDebug = 0#8 
apiroot= 'plat'

if not os.environ.has_key('POWERRT_MACHINE'):
	print 'POWERRT_MACHINE is not set'
	sys.exit()

machineName = os.environ['POWERRT_MACHINE'];

machine = __import__(machineName,fromlist=[''])
daemon = __import__('daemon',fromlist=[''])
xxx = __import__('createNidMap',fromlist=[''])

ldLibraryPath = os.environ['LD_LIBRARY_PATH']

def initClientEnv( config, nidMap, apiroot, serverHost, serverPort ):
	clientEnv = ''
	clientEnv += 'POWERRT_NUMNODES={0} '.format(machine.numNodes)
	clientEnv += 'POWERRT_NODES_PER_BOARD={0} '.format(machine.nodesPerBoard)
	clientEnv += 'POWERRT_BOARDS_PER_CAB={0} '.format(machine.boardsPerCab)
	clientEnv += 'POWERAPI_DEBUG={0} '.format(clientDebug) 
	clientEnv += 'POWERAPI_CONFIG={0} '.format(config) 
	clientEnv += 'POWERAPI_ROOT={0} '.format(apiroot)
	clientEnv += 'POWERAPI_SERVER={0} '.format(serverHost )
	clientEnv += 'POWERAPI_SERVER_PORT={0} '.format(serverPort)
	clientEnv += 'LD_LIBRARY_PATH={0} '.format(ldLibraryPath) 
	return clientEnv

def initDaemonEnv( nidlist ):
	daemonEnv = ''
	daemonEnv += 'POWERRT_NUMNODES={0} '.format(machine.numNodes)
	daemonEnv += 'POWERRT_NODES_PER_BOARD={0} '.format(machine.nodesPerBoard)
	daemonEnv += 'POWERRT_BOARDS_PER_CAB={0} '.format(machine.boardsPerCab)
	daemonEnv += 'POWERAPI_DEBUG={0} '.format(daemonDebug) 
	daemonEnv += 'LD_LIBRARY_PATH={0} '.format(ldLibraryPath) 
	return daemonEnv


def GetApps( node, config, nidlist, routeFile, object, logfile ):
	#print 'GetApps {0}, {1}, {2}, {3}'.format( node, config, nidlist, routeFile )

	nidMap = xxx.createNidMap( nidlist )
	if nidMap[node] != platform.node():
		print 'ERROR: exec.py, bad nidMap', nidMap[node], '!=', platform.node() 
		sys.exit(-1)

	machine.genRouteFile( routeFile )

	ret = []

	exe = daemon.initDaemon( node, nidMap, config, routeFile ).split(' ')
	env = initDaemonEnv( nidlist ).split(' ') 
	ret += [ [ exe, env ]  ] 

	if 0 == int(node):
		exe = daemon.initClient( object, logfile ).split(' ')
		env = initClientEnv( config, nidMap, apiroot, nidMap[0], 15000 ).split(' ') 
		ret += [ [ exe, env ]  ] 

	return ret 

#GetApps( 0, "config", "nid[00061]", "routefile")
#print 'exec.py end'
