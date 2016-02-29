#!/usr/bin/python

import re, sys, logging

Debug = logging.debug

Platform = "Platform" 
Cabinet = "Cabinet" 
Board = "Board" 
Node = "Node" 

Sum = 'SUM'
Avg = 'AVG'

Float = 'Float'
Integer = 'Integer'

Voltage='VOLTAGE'
Energy ='ENERGY'
Power ='POWER'
MaxPower ='MAX_POWER'
Current ='CURRENT'

plugins = {}
devices = {}
objectMap = {}
locationMap = {}

def funcName():
	return 'PY::'+sys._getframe().f_back.f_code.co_name

class Foo:
    def __init__(self, name, total, per, child = None ):
        #print 'name={0} total={1} per={2}'.format(name,total,per)
        self.name = name
        self.total = total 
        self.per = per 
        self.child = child 

    def getTotal(self):
        return self.total		

    def getChildrenPer(self):
		return self.per

    def getLeafCount(self, stop):
        #print 'getLeafCount(): name={0} per={1} stop={2}'.format(self.name, self.per, stop)
        if not self.child or self.name == stop:
            return -1
             
        if self.child.name == stop:
            return self.per
        else:
            return self.child.getLeafCount( stop ) * self.per

    def getTotalChildren( self ):
        if self.child:
            return self.child.getTotal() 
        return 0

class Attr:
	def __init__(self, op = None, type = None, hz = None ):
		self.op = op 
		self.hz = str(hz)
		self.type = type 
		self.src = None 

	def getDevs(self,name):
		if self.src:
			return self.src(name)
		else:
			return []

	def setOp(self,op, type):
		self.op = op
		self.type = type 

	def getOp(self):
		return self.op

	def getHz(self):
		return self.hz

	def getType(self):
		return self.type

	def addDevSrc( self, src ):
		if not self.src == None:
			sys.exit( "ERROR: {0}() Attribute source allready set:".\
					format( funcName() )  )
		self.src = src	

class Object:
	def __init__(self, type, name, parent = None ):
		self.type = type 
		self.name = name
		self.parent = parent
		self.calcNumChildren = self.foo 
		self.child = None
		self.attrs = {}

	def foo(self,x):
		return -1

	def getNumChildren(self,name):
		Debug('{0}() type={1} name={2}'.format( funcName(), self.name, name ) )
		return self.calcNumChildren(name)

	def setChild(self, childObj, calcNumChildren ):
		Debug( "{0}(): obj={1}".format( funcName(), self.name ) ) 
		self.child = childObj
		self.calcNumChildren = calcNumChildren 

	def getType(self):
		return self.type

	def getName(self):
		return self.name

	def getParent(self):
		return self.parent 

	def findChildren(self, myName):
		children = []
		numChildren = self.calcNumChildren(myName)
		Debug( "{0}(): \'{1}\' numChildren={2}".\
						format( funcName(), myName, numChildren ) ) 

		if -1 == numChildren:
			return []

		for i in range( numChildren ):
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def findAttrOp(self, myName, attr):
		Debug( "{0}(): name={1} attr={2}".format( funcName(), myName, attr ) ) 

		if not self.attrs.has_key( attr ):
			Debug( "{0}(): attr={1} not found".format( funcName() , attr ))
			return None

		return self.attrs[attr].getOp() 

	def findAttrHz(self, myName, attr):
		Debug( "{0}(): name={1} attr={2}".format( funcName(), myName, attr ) ) 

		if not self.attrs.has_key( attr ):
			Debug( "{0}(): attr={1} not found".format( funcName() , attr ))
			return None


		tmp = self.attrs[attr].getHz() 
		Debug( "{0}(): name={1} tmp={2}".format( funcName(), myName, tmp ) ) 
		return tmp 

	def findAttrType(self, myName, attr):
		Debug( "{0}(): attr={1}".format( funcName(), attr ) ) 

		if not self.attrs.has_key( attr ):
			Debug( "{0}(): attr={1} not found".format( funcName() , attr ))
			return None

		return self.attrs[attr].getType() 

	def findAttrChildren(self, myName, attr):
		Debug( "{0}(): attr={1}".format( funcName(), attr ) ) 
		children = []

		if not self.attrs.has_key( attr ):
			Debug( "{0}(): attr={1} not found".format( funcName() , attr ))
			return None

		for i in range(self.calcNumChildren(myName)):
			Debug( "{0}(): child={1}".format( funcName(),self.child.getName() ))
			children = children + \
				[ myName + '.' + self.child.getName() + str(i) ]	

		return children

	def setAttr( self, attr, x ):
		Debug( "{0}(): attr={1}".format( funcName(), attr ) ) 
		if not self.attrs.has_key( attr ):
			self.attrs[attr] = x 

	def setAttrOp( self, attr, op, type ):
		Debug( "{0}(): attr={1} op={2}".format( funcName(), attr, op ) ) 
		if not self.attrs.has_key( attr ):
			self.attrs[attr] = Attr()
		self.attrs[attr].setOp( op, type ) 

	def addAttrDevice( self, attr, src ):
		Debug( "{0}(): attr={1} src={2}".format( funcName(), attr, src ) ) 
		self.attrs[attr].addDevSrc( src ) 

	def getDevs( self, name, attr ):
		Debug( "{0}(): attr={1}".format( funcName(), attr ) ) 
		if not self.attrs.has_key( attr ):
			Debug( "{0}(): attr={1} not found".format( funcName() , attr ))
			return None

		return self.attrs[attr].getDevs(name)

	def traverse( self, func, prefix ='', num = None ):
		tmp = prefix + self.getName()  
		if num != None:
			tmp = tmp + str(num)
		else:
			tmp = tmp
	
		for i in range(self.calcNumChildren(tmp)):
			self.child.traverse( func, tmp + '.', i )
				
def _checkName( tmp ):	
	Debug( "{0}(): {1}".format( funcName(), tmp ) )

	pos = tmp.rfind('.')

	pre = '' 
	if pos > -1: 
		pre = tmp[0:pos]
	post = tmp[pos+1:]

	Debug( "{0}() pre=\'{1}\' post=\'{2}\'".format(funcName(), pre, post) )
	
	objType = ''.join(ch for ch in post if ch.isalpha() )

	if not objectMap.has_key(objType):
		sys.exit( "ERROR: mmisformed name \'{0}\', bad objType \'{1}\'".\
									format( tmp, objType))

	if '' == pre:
		return True 

	parent = objectMap[objType].getParent()
	if None == parent:
		print "no parent"
		sys.exit(0)

	number = ''.join(ch for ch in post if ch.isdigit() )

	numChildren = parent.getNumChildren( pre )   
	if -1 == numChildren:
		sys.exit("ERROR".format())

	if int(number) >= numChildren: 
		sys.exit( "invalid: object out of range {0} {1}".\
										format( number, numChildren  ) ) 
		return False	

	return _checkName(pre)

def _getObject( name ):
	Debug( "{0}() obj={1}".format( funcName(), name ) )

	if not _checkName( name ):
		return None 

	tmp = name.split('.')

	objType = ''.join(ch for ch in tmp[-1] if ch.isalpha() )

	return objectMap[objType]


# external
def getObjType( name ):
	Debug( "{0}(): obj={1}".format( funcName(), name ) )
	return _getObject(name).getType( )

# external
def findChildren( name ):
	Debug( "{0}(): obj={1}".format( funcName(), name ) )
	return _getObject(name).findChildren( name )

# external
def findParent( name ):
	Debug( "{0}(): obj={1}".format( funcName(), name ) )
	return _getObject(name).getParent( ).getName()

# external
def findObjLocation( name ):
	Debug( "{0}(): obj={1}".format( funcName(), name ) )

	_checkName( name )

	tmp = name.split('.')

	while len(tmp):
		tmp2 = '.'.join(tmp)	
		if locationMap.has_key( tmp2 ):
			Debug( "{0}(): obj={1} loc={2}".\
					format( funcName(), name, locationMap[ tmp2 ] ) )
			return locationMap[ tmp2 ] 

		tmp.pop()	

	Debug( "{0}(): obj={1} loc=``".format( funcName(), name ) )

	return ''   

	return _getObject(name).findObjLocation( name )

#external
def findAttrChildren( name, attr ):
	Debug( "{0}(): obj={1} attr={2}".format( funcName(), name, attr ) )
	return _getObject(name).findAttrChildren( name, attr )

#external
def findAttrOp( name, attr ):
	Debug( "{0}(): obj={1} attr={2}".format( funcName(), name, attr ) )
	ret = _getObject(name).findAttrOp( name, attr )
	if ret:
		return ret 
	else: 
		return '' 

#external
def findAttrHz( name, attr ):
	Debug( "{0}(): obj={1} attr={2}".format( funcName(), name, attr ) )
	ret = _getObject(name).findAttrHz( name, attr )
	if ret:
		return ret 
	else: 
		return '' 

#external
def findAttrType( name, attr ):
	Debug( "{0}(): obj={1} attr={2}".format( funcName(), name, attr ) )
	ret = _getObject(name).findAttrType( name, attr )
	if ret:
		return ret 
	else: 
		return '' 

# external
def findAttrDevs( name, attr ):
	Debug( "{0}(): obj={1} attr={2}".format( funcName(), name, attr ) )
	return _getObject(name).getDevs(name,attr)

# external
def findPlugins():
	Debug( "{0}():".format( funcName() ) )
	
	dictList = []
	for key, value in plugins.iteritems():
		temp = [key,value]
		dictList.append(temp)
	return dictList 

# external
def findSysDevs():
	Debug( "{0}():".format( funcName() ) )
	dictList = []
	for key, value  in devices.iteritems():
		temp = [key] + value
		dictList.append(temp)
	return dictList 


# external
def setLocation( obj, loc ):
	Debug( "{0}(): obj={1} loc={2}".format( funcName(), obj, loc ) )
	locationMap[obj] = loc 

# external
def printLocations():
	for key, value in locationMap.iteritems():
		print key, value
