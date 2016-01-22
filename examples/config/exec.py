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

def initClientEnv( config, nidlist, nidMap ):
	clientEnv = ''
	clientEnv += 'POWERRT_NUMNODES={0} '.format(machine.numNodes)
	clientEnv += 'POWERRT_NODES_PER_BOARD={0} '.format(machine.nodesPerBoard)
	clientEnv += 'POWERRT_BOARDS_PER_CAB={0} '.format(machine.boardsPerCab)
	clientEnv += 'POWERAPI_DEBUG={0} '.format(clientDebug) 
	clientEnv += 'SLURM_NODELIST={0} '.format(nidlist)
	clientEnv += 'POWERAPI_CONFIG={0} '.format(config) 
	clientEnv += 'POWERAPI_ROOT={0} '.format(apiroot)
	clientEnv += 'POWERAPI_SERVER={0} '.format(nidMap[0])
	clientEnv += 'POWERAPI_SERVER_PORT={0} '.format(15000)
	clientEnv += 'WAIT={0} '.format(2)
	return clientEnv

def initDaemonEnv( nidlist ):
	daemonEnv = ''
	daemonEnv += 'POWERRT_NUMNODES={0} '.format(machine.numNodes)
	daemonEnv += 'POWERRT_NODES_PER_BOARD={0} '.format(machine.nodesPerBoard)
	daemonEnv += 'POWERRT_BOARDS_PER_CAB={0} '.format(machine.boardsPerCab)
	daemonEnv += 'POWERAPI_DEBUG={0} '.format(daemonDebug) 
	daemonEnv += 'SLURM_NODELIST={0} '.format(nidlist)
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
		exe = daemon.initClient( node, nidMap, config, routeFile, \
										object,logfile ).split(' ')
		env = initClientEnv( config, nidlist, nidMap ).split(' ') 
		ret += [ [ exe, env ]  ] 

	return ret 

#GetApps( 0, "config", "nid[00061]", "routefile")
#print 'exec.py end'
