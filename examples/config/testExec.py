#! /usr/bin/python


numnodes =  2 
npb = 2 
nidlist = 'chama[0-'+str(numnodes-1)+']'
#nidlist = 'chama0'

import os
os.environ['POWERRT_MACHINE']='dummy'
os.environ['POWERRT_NUMNODES']=str(numnodes)
os.environ['POWERRT_NODES_PER_BOARD']=str(npb)
os.environ['POWERRT_BOARDS_PER_CAB']='1'
os.environ['PYTHONPATH']='pwrGIT/chama/examples/config'

print 'numnodes', numnodes
print 'board_per_node', npb

import yy

for i in xrange(0,numnodes):
	print yy.GetApps( i, 'config', nidlist, 'routefile', '', 'logfile','daemon', ''  )


#print yy.GetApps( 5, 'config', nidlist, 'routefile', '', 'logfile','daemon', ''  )
#print yy.GetApps( numnodes-1, 'config', nidlist, 'routefile', 'plat.cab0', 'logfile','daemon', ''  )
