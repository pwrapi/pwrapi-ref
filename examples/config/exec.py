#! /usr/bin/python

import os, daemon, batchUtil, sys

daemonDebug = 0 
clientDebug = 0 
apiroot= 'plat'

home='/home/mjleven'
root = home + '/pwrGIT/working'

nidList = os.environ['SLURM_NODELIST']
#print "nidlist \'{0}\'".format( nidList)

os.environ['PYTHONPATH'] = root + '/examples/config'
os.environ['POWERAPI_DEBUG'] = str(0)

#print os.environ['PYTHONPATH']

import volta

sys.path += [ os.environ['PYTHONPATH'] ]

nidMap = batchUtil.createNidMap2( nidList )
#print "nidMap", nidMap

config = root + '/examples/config/volta.py'
routeFile = home + '/pwrTest/routeTable.txt'

volta.genRouteFile( routeFile )

clientEnv = ''
clientEnv += 'POWERAPI_DEBUG={0} '.format(clientDebug) 
clientEnv += 'SLURM_NODELIST={0} '.format(nidList)
clientEnv += 'POWERAPI_CONFIG={0} '.format(config) 
clientEnv += 'POWERAPI_ROOT={0} '.format(apiroot)
clientEnv += 'POWERAPI_SERVER={0} '.format(nidMap[0])
clientEnv += 'POWERAPI_SERVER_PORT={0} '.format(15000)
clientEnv += 'WAIT={0} '.format(2)

daemonEnv = ''
daemonEnv += 'POWERAPI_DEBUG={0} '.format(daemonDebug) 
daemonEnv += 'SLURM_NODELIST={0} '.format(nidList)

def GetApps( rank ):
	print "GetApps ", rank	
	ret = []
	exe = daemon.initDaemon( rank, nidMap, config, routeFile ).split(' ')
	env = daemonEnv.split(' ') 
	ret += [ [ exe, env ]  ] 

	if 0 == int(rank):
		exe = daemon.initClient( rank, nidMap, config, routeFile ).split(' ')
		env = clientEnv.split(' ') 
		ret += [ [ exe, env ]  ] 

	return ret 
