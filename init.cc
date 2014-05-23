#include <stdio.h>
#include <sstream>
#include "./object.h"
#include "./types.h"

#define NUM_CABINETS 1 
#define NUM_BOARDS 1 
#define NUM_NODES 1 
#define NUM_SOCKETS 1 
#define NUM_CORES 4 

using namespace std;

namespace PWR {

#if 0
static _Obj* createPlatform( _Cntxt*, string );
static _Obj* createCabinet( _Cntxt*, string );
static _Obj* createBoard( _Cntxt*, string );
static _Obj* createNode( _Cntxt*, string );
static _Obj* createSocket( _Cntxt*, string );
#endif

_Cntxt* init( PWR_CntxtType type, PWR_Role role, const char* name )
{
    _Cntxt* ctx = new _Cntxt( type, role, name ); 
//    ctx->init( createPlatform( ctx, "SystemX" ), role, name );
    return ctx;
}

int destroy( _Cntxt* ctx )
{
    delete ctx; 
    return PWR_ERR_SUCCESS;
}

#if 0
static _Obj* createPlatform( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_PLATFORM );

    _AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                             PWR_ATTR_DATA_STRING, "", prefix );
    top->attributeAdd( attrString );

    for ( int i = 0; i < NUM_CABINETS; i++ ) { 
        ostringstream tmp;
        tmp << i;
        _Obj* obj = createCabinet( ctx, "cab" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    } 
    return top;
}


static _Obj* createCabinet( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_CABINET );

    _AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                             PWR_ATTR_DATA_STRING, "", prefix );
    top->attributeAdd( attrString );

    for ( int i = 0; i < NUM_BOARDS; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Obj* obj = createBoard( ctx, prefix + ".brd" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Obj* createBoard( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_BOARD );
    _AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                             PWR_ATTR_DATA_STRING, "", prefix );
    top->attributeAdd( attrString );


    for ( int i = 0; i < NUM_NODES; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Obj* obj = createNode( ctx, prefix + ".node" + tmp.str() ); 
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Obj* createNode( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_NODE );
    _AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                             PWR_ATTR_DATA_STRING, "", prefix );
    top->attributeAdd( attrString );


    for ( int i = 0; i < NUM_SOCKETS; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Obj* obj = createSocket( ctx, prefix + ".sock" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Obj* createSocket( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_SOCKET );
    _AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                             PWR_ATTR_DATA_STRING, "", prefix );
    top->attributeAdd( attrString );


    float freq[4] = {1.2,1.2, 3.0, 3.0}; 

    for ( int i = 0; i < NUM_CORES; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Obj* obj = new _Obj( ctx, prefix + ".core" + tmp.str(), PWR_OBJ_CORE);
        top->children().add( obj ); 
        obj->setParent( top );

        _AttrNumTemplate< float >* attrFloat = 
                new _AttrNumTemplate< float >( PWR_ATTR_FREQ, 
                         PWR_ATTR_DATA_FLOAT, PWR_ATTR_UNITS_MEGA, 1, 10, freq[i] );
        obj->attributeAdd( attrFloat );

#if 0
        _AttrNumTemplate< int >* attrInt = 
                new _AttrNumTemplate< int >( PWR_ATTR_PSTATE, 
                             PWR_ATTR_INT, PWR_ATTR_UNITS_1, 0,3, 1 );
        obj->attributeAdd( attrInt );
#endif

    	_AttrStringTemplate< std::string >* attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_NAME, 
                         PWR_ATTR_DATA_STRING, "", prefix + ".core" + tmp.str() );
    	obj->attributeAdd( attrString );

    	attrString =
                new _AttrStringTemplate< std::string >( PWR_ATTR_PSTATE, 
                         PWR_ATTR_DATA_STRING, "P0,P1,P2", "P2" );
    	obj->attributeAdd( attrString );
    }
    return top;
}
#endif

}

