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

static _Obj* createPlatform( _Cntxt*, string );
static _Obj* createCabinet( _Cntxt*, string );
static _Obj* createBoard( _Cntxt*, string );
static _Obj* createNode( _Cntxt*, string );
static _Obj* createSocket( _Cntxt*, string );

_Cntxt* init( PWR_CntxtType type )
{
    _Cntxt* ctx = new _Cntxt; 
    ctx->init( createPlatform( ctx, "SystemX" ) );
    return ctx;
}

int destroy( _Cntxt* ctx )
{
    delete ctx; 
    return PWR_SUCCESS;
}

static _Obj* createPlatform( _Cntxt* ctx, string prefix )
{
    _Obj* top = new _Obj( ctx, prefix, PWR_OBJ_PLATFORM );

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

    float freq[4] = {1.2,1.2, 3.0, 3.0}; 

    for ( int i = 0; i < NUM_CORES; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Obj* obj = new _Obj( ctx, prefix + ".core" + tmp.str(), PWR_OBJ_CORE);
        top->children().add( obj ); 
        obj->setParent( top );

        _AttrNumTemplate< float >* attrFloat = 
                new _AttrNumTemplate< float >( PWR_ATTR_FREQ, 
                         PWR_ATTR_FLOAT, PWR_ATTR_UNITS_MEGA, 1, 10, freq[i] );
        obj->attributeAdd( attrFloat );

        _AttrStringTemplate< string >* attrString = 
                new _AttrStringTemplate< string >( PWR_ATTR_STATE, 
                             PWR_ATTR_STRING, "ON|OFF", "ON" );
        obj->attributeAdd( attrString );

        _AttrNumTemplate< int >* attrInt = 
                new _AttrNumTemplate< int >( PWR_ATTR_ID, 
                             PWR_ATTR_INT, PWR_ATTR_UNITS_1, 0, 4, i );
        obj->attributeAdd( attrInt );
    }
    return top;
}

}

