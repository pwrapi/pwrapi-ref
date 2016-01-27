#! /usr/bin/python


import re,sys

#print "createNidMap.py start"

def createNidMap( nidlist):
	
    nidlist = ''.join( nidlist.split() )
    #print nidlist
    #obj = re.findall( r'.*?\[.*?\],*|.+[0123456789]+,*', nidlist )
    #obj = re.findall( r'.+\[[^\]]+\],*', nidlist )
    #obj = re.findall( r'[^\,]+\[[^\]]+\]', nidlist )
    obj = re.findall( r'[^\[,]+\[[^\]]+\]|[^,]+[0123456789]+', nidlist )

    #print 'obj', obj
     
    nidMap = []
    for x in obj:

        #print 'x', x
        pos1 = x.find('[')
            
        if -1 == pos1:
			nidMap += [x]
        else:
            pos2 = x.find(']')
            #print pos1, pos2
            pre = x[0:pos1]
            post = x[pos1+1:pos2].split(',')
            #print pre, post 
            for y in post: 
                if -1 == y.find('-'):
                    nidMap += [pre + y]
                else:                 
                    z = y.split('-')
                    for i in range( int(z[0]), int(z[1]) + 1):
                        frmt = '%%s%%0%dd' % len(z[1])
                        nidMap += [frmt % (pre , i)] 

    return nidMap


#print createNidMap( 'nid000[60-63,64],nid00007,volta0[100-101]' )
#print createNidMap( 'nid000[60-63,67], nid88[100-102], nid00007' )
#print ''
#print createNidMap( 'nid00007,volta0[100-101]' )
#print ''
#print createNidMap( 'nid000[08-11]' )
#print createNidMap( 'nid000[8-11]' )
#print createNidMap( 'chama[47-48,1171-1172]' )

#print "createNidMap.py end"
