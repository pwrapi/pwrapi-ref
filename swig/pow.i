%module pow

%include "typemaps.i"

//
// Rename everything from PWR_XXX -> XXX
//

%rename( "%(strip:[PWR_])s",
        notmatch$name="PWR_ObjAttrGetValues",
        notmatch$name="PWR_ObjAttrSetValues",
        notmatch$name="PWR_StatGetValue"
	    ) "";


%{
#include "pow.h"
#include "types.h"
%}

%include "types.h"

typedef unsigned long long int64_t;

/*************************************************************************
* typemaps for "time_t"
*************************************************************************/

%typemap(in,numinputs=0) time_t *OutValue(time_t tmp) {
	$1= &tmp;
}

%typemap(argout) time_t *OutValue {

    PyObject *o = PyInt_FromLong(*$1);

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


/*************************************************************************
* typemaps for "double"
*************************************************************************/

%typemap(in,numinputs=0) double *OutValue( double tmp  ) {
	$1= &tmp;
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

/*************************************************************************
* typemaps for "PWR_Time"
*************************************************************************/

%typemap(in) PWR_Time {
	$1 = PyLong_AsLong($input);
}

%typemap(in,numinputs=0) PWR_Time *OutValue(PWR_Time tmp  ) {
	$1= &tmp;
}

%typemap(argout) PWR_Time *OutValue {

    PyObject *o = PyInt_FromLong(*$1);

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

/*************************************************************************
* typemaps for "PWR_StatTimes"
*************************************************************************/

%typemap(in,numinputs=0) PWR_StatTimes *OutValue( PWR_StatTimes tmp  ) {
	$1= &tmp;
}

%typemap(argout) PWR_StatTimes *OutValue {

	PyObject *oDict = PyDict_New();

	PyDict_SetItemString(oDict,"start",PyInt_FromLong($1->start));	
	PyDict_SetItemString(oDict,"stop",PyInt_FromLong($1->stop));	
	PyDict_SetItemString(oDict,"instant",PyInt_FromLong($1->instant));	

    if ((!$result) || ($result == Py_None)) {
        $result = oDict;
    } else {
		PyObject *o2, *o3;
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,oDict);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
}


/****************************************************************************
* typemaps for PWR_AttrName
****************************************************************************/

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

/****************************************************************************
*     Start of API
****************************************************************************/

int PWR_GetMajorVersion();
int PWR_GetMinorVersion();

/*
* Subset of API that works on Cntxt
*/

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType, PWR_Role, const char* name );
int       PWR_CntxtDestroy( PWR_Cntxt );
PWR_Obj   PWR_CntxtGetEntryPoint( PWR_Cntxt );
PWR_Grp   PWR_CntxtGetGrpByType( PWR_Cntxt, PWR_ObjType );
PWR_Grp   PWR_CntxtGetGrpByName( PWR_Cntxt, const char* name );

PWR_Obj PWR_CntxtGetObjByName( PWR_Cntxt, const char* name );

/*
* Subset of API that works on Obj
*/

PWR_ObjType PWR_ObjGetType( PWR_Obj );
const char* PWR_ObjGetName( PWR_Obj );
PWR_Obj     PWR_ObjGetParent( PWR_Obj );
PWR_Grp     PWR_ObjGetChildren( PWR_Obj );

int PWR_ObjAttrIsValid( PWR_Obj, PWR_AttrName name );

int PWR_ObjAttrGetValue( PWR_Obj, PWR_AttrName, double *OutValue, PWR_Time *OutValue);
int PWR_ObjAttrSetValue( PWR_Obj, PWR_AttrName name, double *INPUT );

int PWR_ObjAttrGetValues( PWR_Obj, int count, PWR_AttrName names[], void* buf, PWR_Time ts[], PWR_Status  );
int PWR_ObjAttrSetValues( PWR_Obj, int count, PWR_AttrName names[], void* buf, PWR_Status );

/*
* Subset of API that works on Grp
*/

// note that PWR_GrpDestroy() pairs with PWR_CntxtCreateGrp() but it
// does not take a context as an argument, should it?

PWR_Grp     PWR_GrpCreate( PWR_Cntxt, const char* name );
int         PWR_GrpDestroy( PWR_Grp );
const char* PWR_GrpGetName( PWR_Grp );
int         PWR_GrpGetNumObjs( PWR_Grp );
PWR_Obj     PWR_GrpGetObjByIndx( PWR_Grp, int );
int         PWR_GrpAddObj( PWR_Grp, PWR_Obj );
int         PWR_GrpRemoveObj( PWR_Grp, PWR_Obj );

int         PWR_GrpAttrSetValue( PWR_Grp, PWR_AttrName name, double *INPUT, PWR_Status );
int         PWR_GrpAttrGetValue( PWR_Grp, PWR_AttrName name, double* OutValue, PWR_Time ts[], PWR_Status );

int         PWR_GrpAttrSetValues( PWR_Grp, int count, PWR_AttrName names[], void* buf, PWR_Status );
int         PWR_GrpAttrGetValues( PWR_Grp, int count, PWR_AttrName names[], void* buf, PWR_Time ts[], PWR_Status );

PWR_Status PWR_StatusCreate();
int PWR_StatusDestroy( PWR_Status );
int PWR_StatusPopError( PWR_Status, PWR_AttrAccessError* );
int PWR_StatusClear( PWR_Status );

PWR_Stat PWR_ObjCreateStat( PWR_Obj, PWR_AttrName name, PWR_AttrStat stat );
PWR_Stat PWR_GrpCreateStat( PWR_Grp, PWR_AttrName name, PWR_AttrStat stat );

int PWR_StatDestroy( PWR_Stat );

int PWR_StatStart( PWR_Stat );
int PWR_StatStop( PWR_Stat );
int PWR_StatClear( PWR_Stat );


int PWR_StatGetValue( PWR_Stat, double *OutValue, PWR_StatTimes *OutValue );
/*
 * size of array passed in is derived by the function
 */
int PWR_StatGetValues( PWR_Stat, double values[],
                                            PWR_StatTimes statTimes[] );

/*
*  Utility Functions
*/

const char* PWR_ObjGetTypeString( PWR_ObjType );
const char* PWR_AttrGetTypeString( PWR_AttrName );
const char* PWR_ObjGetName( PWR_Obj );

int PWR_TimeConvert( PWR_Time time, time_t* OutValue );

/*
 * Operating System -> Application Interface
 */
int PWR_AppHint( PWR_Obj, PWR_RegionHint );


/****************************************************************************
*     End of API
****************************************************************************/


%pythoncode %{

def ObjAttrGetValues( self, names, status ):
	return _ObjAttrGetValues( self, len(names), names, status )

def ObjAttrSetValues( self, names, values, status ):
	return _ObjAttrSetValues( self, len(names), names, values, status )

def StatGetValue( coreStat ):
	retval, value, x = PWR_StatGetValue( coreStat )
	
	timeStat = StatTimes()
	timeStat.start = x['start'] 
	timeStat.stop = x['stop'] 
	timeStat.instant = x['instant'] 

	return retval, value, timeStat  

%}


int _ObjAttrGetValues( PWR_Obj, int, PWR_AttrName *InValue,
                    double *OutValue, PWR_Time *OutValue, PWR_Status );
int _ObjAttrSetValues( PWR_Obj, int, PWR_AttrName *InValue,
                    double *InValue, PWR_Status );

%inline %{

	int _ObjAttrGetValues( PWR_Obj obj, int num, PWR_AttrName *names,
                    double *buf, PWR_Time *ts, PWR_Status status ) {
		return PWR_ObjAttrGetValues( obj, num, names, buf, ts, status );  
	}

	int _ObjAttrSetValues( PWR_Obj obj, int num, PWR_AttrName *names,
                    double *buf, PWR_Status status ) {
		return PWR_ObjAttrSetValues( obj, num, names, buf, status );  
	}
%}
