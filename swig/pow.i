%module pow

#include "typemaps.i"
#moudle inarg
#module outarg

%rename( "%(strip:[PWR_])s", 
		notmatch$name="PWR_ObjAttrSetValue",
		notmatch$name="PWR_ObjAttrGetValue",
		notmatch$name="PWR_ObjAttrGetValues",
		notmatch$name="PWR_ObjAttrSetValues",
		notmatch$name="PWR_GrpAttrSetValue",
		notmatch$name="PWR_StatGetValue",
		notmatch$name="PWR_TimeConvert",
		notmatch$name="PWR_Time"
		) "";

%pythoncode %{

def Time( val ):
	return val 

def ObjAttrGetValues( self, names, status ):
	return _ObjAttrGetValues( self, len(names), names, status )

def ObjAttrSetValues( self, names, values, status ):
	return _ObjAttrSetValues( self, len(names), names, values, status )

%}

%{
#include "pow.h"
#include "types.h"
%}

%include "pow.h"
%include "types.h"


//
// time_t
//

%typemap(argout) time_t *OutValue {

    PyObject *o;
    o = PyInt_FromLong(*$1);

    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {
		PyObject *o2, *o3;
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,o);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
}

%typemap(in,numinputs=0) time_t *OutValue(time_t tmp) {
	$1= &tmp;
}

//
// PWR_Time
//

%typemap(in) PWR_Time {
	$1 = PyLong_AsLong($input);
}

%typemap(argout) PWR_Time *OutValue {

    PyObject *o;
    o = PyInt_FromLong(*$1);

    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {
		PyObject *o2, *o3;
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,o);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
}

%typemap(in,numinputs=0) PWR_Time *OutValue(PWR_Time tmp  ) {
	$1= &tmp;
}

//
// PWR_StatTimes
//

%typemap(argout) PWR_StatTimes *OutValue {

	printf("start=%llu\n",$1->start);
	printf("stop=%llu\n",$1->stop);
	printf("instant=%llu\n",$1->instant);
/*
    PyObject *o;
    o = PyInt_FromLong(*$1);

    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {
		PyObject *o2, *o3;
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,o);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
*/
}

%typemap(in,numinputs=0) PWR_StatTimes *OutValue(PWR_StatTimes tmp  ) {
	$1= &tmp;
}

//
// double
//

%typemap(argout) double *OutValue {

    PyObject *o;
    o = PyFloat_FromDouble(*$1);

    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {
		PyObject *o2, *o3;
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,o);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
}

%typemap(in,numinputs=0) double *OutValue( double tmp  ) {
	$1= &tmp;
}

//
// PWR_AttrName 
//

%typemap(in) PWR_AttrName *InValue {
   	int nitems = PyList_Size($input);    
	int i;
   	$1 = (PWR_AttrName *) malloc( sizeof(PWR_AttrName) * nitems );

	for ( i = 0; i < nitems; i++ ) {
		PyObject* o = PyList_GetItem( $input, i);
		$1[i] = PyInt_AsLong( o );
	}
}

%typemap(freearg) PWR_AttrName *InValue {
  	free($1);
}

%typemap(in) double *InValue {
   	int nitems = PyList_Size($input);    
	int i;
   	$1 = (double *) malloc( sizeof(PWR_AttrName) * nitems );

	for ( i = 0; i < nitems; i++ ) {
		PyObject* o = PyList_GetItem( $input, i);
		$1[i] = PyFloat_AsDouble(o);
	}
}

%typemap(freearg) double *InValue {
  	free($1);
}


int ObjAttrSetValue( PWR_Obj obj, PWR_AttrName name,
                    double *INPUT);
int ObjAttrGetValue( PWR_Obj obj, PWR_AttrName name,
                    double *OutValue, PWR_Time *OutValue  );

int _ObjAttrGetValues( PWR_Obj, int, PWR_AttrName *InValue,
                    double *OutValue, PWR_Time *OutValue, PWR_Status );
int _ObjAttrSetValues( PWR_Obj, int, PWR_AttrName *InValue,
                    double *InValue, PWR_Status );

int  GrpAttrSetValue( PWR_Grp, PWR_AttrName name, double *INPUT, PWR_Status );

int StatGetValue( PWR_Stat, double *OUTPUT, PWR_StatTimes *OutValue );

int TimeConvert( PWR_Time time, time_t *OutValue );

%inline %{

	int TimeConvert( PWR_Time ts, time_t *time  ) {
		return PWR_TimeConvert( ts, time );
	}

	int ObjAttrSetValue( PWR_Obj obj, PWR_AttrName name, 
					double* buf ) {	
		return PWR_ObjAttrSetValue( obj, name, buf );
	}

	int ObjAttrGetValue( PWR_Obj obj, PWR_AttrName name, 
					double* buf, PWR_Time *ts  ) {	
		return PWR_ObjAttrGetValue( obj, name, buf, ts );
	}

	int _ObjAttrGetValues( PWR_Obj obj, int num, PWR_AttrName *names,
                    double *buf, PWR_Time *ts, PWR_Status status ) {
		return PWR_ObjAttrGetValues( obj, num, names, buf, ts, status );  
	}

	int _ObjAttrSetValues( PWR_Obj obj, int num, PWR_AttrName *names,
                    double *buf, PWR_Status status ) {
		return PWR_ObjAttrSetValues( obj, num, names, buf, status );  
	}
	int  GrpAttrSetValue( PWR_Grp grp, PWR_AttrName name, double *value, 
										PWR_Status status )
	{
		return PWR_GrpAttrSetValue( grp, name, value, status );  
	}
	int StatGetValue( PWR_Stat stat, double *value, PWR_StatTimes *statTimes ) {
		return PWR_StatGetValue( stat, value, statTimes );
	}
%}
