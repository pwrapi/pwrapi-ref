#! /usr/bin/python

import re

def expandNidList( tmp ):
      
    ret = []
    x = tmp.split( "[")
    x = [x[0]] + x[1].split('-'); 
    x = x[0:2] + x[2].split(']'); 

    for i in range( int(x[1]), int(x[2]) + 1 ):
        z = "%%s%%0%dd" % len(x[1])
        y = z % (x[0],i)  
        ret += [y]
    
    return ret 

def createNidMap( nidList ):
    nidMap = []
    for tmp in nidList:
        if not -1 == tmp.find(']'): 
            nidMap += expandNidList(tmp)
        else:
            nidMap += [tmp];
    return nidMap

def createNidMap2( nidlist):
    obj = re.findall( r'.*?\[.*?\],*', nidlist )


    ret=[]
    for x in obj:
        a,b = x.split('[')
        ret = ret + [ [ a , b.split(']')[0].split(',') ] ]

    nidMap = []
    for x in ret:
        a,b = x
        #print a,b
        for y in b:
            tmp = y.split('-')
            if 2 == len(tmp):
                #print 'iii'  , tmp
                for i in range( int(tmp[0]), int(tmp[1]) + 1):
                    nidMap +=[a + str(i)]
            else :
                nidMap += [a + y]

    return nidMap
