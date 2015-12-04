#!/usr/bin/python

#platName = "plat"
#numCab  

#"plat"
#"plat.cab0.rack0.nodea"

import re, sys

Platform = "Platform" 
Cabinet = "Cabinet" 
Rack = "Rack" 
Node = "Node" 

Sum = 'Sum'
Avg = 'Avg'

Energy ='ENERGY'
Power ='POWER'
MaxPower ='MAX_POWER'

plugins = {} 
plugins['CrayXTPM'] = 'libpwr_xtpmdev' 

devices = {}
devices['XTPM-node'] = ['CrayXTPM','']

def funcName():
	return 'PY::'+sys._getframe().f_back.f_code.co_name + '(): '


class Attr:
	def __init__(self, op, device, openString):
		self.op = op
		self.device = device
		self.openString = openString
	def getDevs(self):
		return [ [self.device, self.openString] ] 

class Object:
	def __init__(self, type, name, parent = None ):
		self.type = type 
		self.name = name
		self.parent = parent
		self.numChildren = 0
		self.child = None
		self.attrs = {}

	def setChild(self, childObj, numChildren ):
		self.child = childObj
		self.numChildren = numChildren 

	def getType(self):
		return self.type

	def getName(self):
		return self.name

	def findChildren(self, myName):
		children = []
		for i in range(self.numChildren):
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def findAttrChildren(self, myName, attr):
		print funcName() + 'attr', attr
		children = []

		if not self.attrs.has_key( attr ):
			print funcName() + 'attr', attr, 'not found'
			return None

		for i in range(self.numChildren):
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def setAttrDevice( self, attrName, attr ):
		print funcName() + 'attr', attrName
		self.attrs[attrName] = attr

	def getDevs( self, attr ):
		print funcName() + 'attr', attr
		if not self.attrs.has_key( attr ):
			print funcName() + 'attr', attr, 'not found'
			return None

		return self.attrs[attr].getDevs()

	def traverse( self, func, prefix ='', num = None ):
		tmp = prefix + self.getName()  
		if num != None:
			tmp = tmp + str(num)
		else:
			tmp = tmp
	
		print tmp 
		for i in range(self.numChildren):
			self.child.traverse( func, tmp + '.', i )
				
platform = Object( Platform, 'plat' )
#cabinet = Object( Cabinet, platform )
#platform.setChild( cabinet, 2 )
#rack = Object( Rack, cabinet )
#cabinet.setChild( rack, 1 )
#node = Object( Node, rack )
node = Object( Node, 'node', platform )
platform.setChild( node, 1 )


attr = Attr( Sum, "XTPM-node", "" )
node.setAttrDevice( Energy, attr )

attr = Attr( Avg, "XTPM-node", "f" )
node.setAttrDevice( Power, attr )

attr = Attr( Avg, "XTPM-node", "" )
node.setAttrDevice( MaxPower, attr )

objectMap = {}
objectMap['plat'] = platform
#objectMap[Cabinet] = cabinet
#objectMap[Rack] = rack
objectMap['node'] = node

def checkName( tmp ):	
	for index in range(len(tmp)):

		val = tmp[index]
		objType = ''.join(ch for ch in val if ch.isalpha() )
		number = ''.join(ch for ch in val if ch.isdigit() )
		if not objectMap.has_key( objType ):
			print "invalid object type", objType
			return False	

	return True

def getObject( name ):
	print funcName() + 'object', name
	tmp = name.split('.')

	if not checkName( tmp ):
		return None 

	objType = ''.join(ch for ch in tmp[-1] if ch.isalpha() )
	
	return objectMap[objType]


def getObjType( name ):
	print funcName() + 'object', name
	return getObject(name).getType( )

def findChildren( name ):
	print funcName() + 'object', name
	return getObject(name).findChildren( name )

def findAttrChildren( name, attr ):
	print funcName() + 'object', name, 'attr', attr 
	return getObject(name).findAttrChildren( name, attr )

def findObjDevs( name, attr ):
	print funcName() + 'object', name
	return getObject(name).getDevs(attr)

def findPlugins():
	print funcName() 
	dictList = []
	for key, value in plugins.iteritems():
		temp = [key,value]
		dictList.append(temp)
	return dictList 

def findSysDevs():
	print funcName() 
	dictList = []
	for key, value  in devices.iteritems():
		temp = [key] + value
		dictList.append(temp)
	return dictList 


print findAttrChildren( "plat.node0", "ENERGY" )

#print findSysDevs()

#print findPlugins()
#print getObjType( "plat.node0" )
#print findObjDevs( "plat.node0", "Energy" )
#print findChildren( "plat" )

#print getObjType( "plat" )
