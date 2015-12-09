#!/usr/bin/python

import re, sys

Platform = "Platform" 
Cabinet = "Cabinet" 
Board = "Board" 
Node = "Node" 

Sum = 'SUM'
Avg = 'AVG'

Energy ='ENERGY'
Power ='POWER'
MaxPower ='MAX_POWER'

plugins = {}
devices = {}
objectMap = {}
locationMap = {}

def funcName():
	return 'PY::'+sys._getframe().f_back.f_code.co_name + '(): '

class Attr:
	def __init__(self ):
		self.op = None 
		self.src = []

	def getDevs(self):
		return self.src

	def setOp(self,op):
		self.op = op
	def getOp(self):
		return self.op

	def addSrc( self, src ):
		self.src += [src]	

class Object:
	def __init__(self, type, name, parent = None ):
		self.type = type 
		self.name = name
		self.parent = parent
		self.numChildren = 0
		self.child = None
		self.attrs = {}

	def getNumChildren(self):
		#print funcName() + self.name , self.numChildren 
		return self.numChildren

	def setChild(self, childObj, numChildren ):
		print funcName() + self.name , numChildren 
		self.child = childObj
		self.numChildren = numChildren 

	def getType(self):
		return self.type

	def getName(self):
		return self.name

	def getParent(self):
		return self.parent 

	def findChildren(self, myName):
		children = []
		for i in range(self.numChildren):
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def findAttrOp(self, myName, attr):
		print funcName() + 'attr', attr

		if not self.attrs.has_key( attr ):
			print funcName() + 'attr', attr, 'not found'
			return None

		return self.attrs[attr].getOp() 

	def findAttrChildren(self, myName, attr):
		print funcName(), 'obj', myName,' attr', attr
		children = []

		if not self.attrs.has_key( attr ):
			print funcName() + 'attr', attr, 'not found'
			return None

		for i in range(self.numChildren):
			print funcName() + 'child ', self.child.getName()
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def setAttrOp( self, attr, op ):
		print funcName() + 'attr', attr
		if not self.attrs.has_key( attr ):
			self.attrs[attr] = Attr()
		self.attrs[attr].setOp( op ) 

	def addAttrDevice( self, attr, src ):
		print funcName() + 'attr', attr
		self.attrs[attr].addSrc( src ) 

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
				
def checkName( tmp ):	
	print funcName(), tmp 
	for index in range(len(tmp)):

		val = tmp[index]
		#print funcName() + val
		objType = ''.join(ch for ch in val if ch.isalpha() )
		number = ''.join(ch for ch in val if ch.isdigit() )
		if not objectMap.has_key( objType ):
			print "invalid object type", objType
			return False	

		parent = objectMap[objType].getParent()
		
		if parent: 
			#print val, number, parent.getNumChildren()		
			if int(number) >= parent.getNumChildren():   
				print "invalid: object out of range", val 
				return False	
		else:
			if number:
				print "invalid: object out of range", val 
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

def findParent( name ):
	print funcName() + 'object', name
	return getObject(name).getParent( name )

def findObjLocation( name ):

	print funcName(), name 

	tmp = name.split('.')
	checkName( tmp )

	while len(tmp):
		tmp2 = '.'.join(tmp)	
		print tmp2
		if locationMap.has_key( tmp2 ):
			return locationMap[ tmp2 ] 

		tmp.pop()	

	return ''   

	print funcName() + 'object', name
	return getObject(name).findObjLocation( name )

def findAttrChildren( name, attr ):
	print funcName() + 'object', name, 'attr', attr 
	return getObject(name).findAttrChildren( name, attr )

def findAttrOp( name, attr ):
	print funcName() + 'object', name, 'attr', attr 
	return getObject(name).findAttrOp( name, attr )

def findAttrDevs( name, attr ):
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
