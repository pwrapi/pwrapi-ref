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

namespace PLI {

static _Object* createPlatform( _Context*, string );
static _Object* createCabinet( _Context*, string );
static _Object* createBoard( _Context*, string );
static _Object* createNode( _Context*, string );
static _Object* createSocket( _Context*, string );

_Context* init( PLI_ContextType type )
{
    _Context* ctx = new _Context; 
    ctx->init( createPlatform( ctx, "SystemX" ) );
    return ctx;
}

int destroy( _Context* ctx )
{
    delete ctx; 
    return PLI_SUCCESS;
}

static _Object* createPlatform( _Context* ctx, string prefix )
{
    _Object* top = new _Object( ctx, prefix, PLI_OBJ_PLATFORM );

    for ( int i = 0; i < NUM_CABINETS; i++ ) { 
        ostringstream tmp;
        tmp << i;
        _Object* obj = createCabinet( ctx, "cab" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    } 
    return top;
}


static _Object* createCabinet( _Context* ctx, string prefix )
{
    _Object* top = new _Object( ctx, prefix, PLI_OBJ_CABINET );

    for ( int i = 0; i < NUM_BOARDS; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Object* obj = createBoard( ctx, prefix + ".brd" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Object* createBoard( _Context* ctx, string prefix )
{
    _Object* top = new _Object( ctx, prefix, PLI_OBJ_BOARD );

    for ( int i = 0; i < NUM_NODES; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Object* obj = createNode( ctx, prefix + ".node" + tmp.str() ); 
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Object* createNode( _Context* ctx, string prefix )
{
    _Object* top = new _Object( ctx, prefix, PLI_OBJ_NODE );

    for ( int i = 0; i < NUM_SOCKETS; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Object* obj = createSocket( ctx, prefix + ".sock" + tmp.str() );
        top->children().add( obj );
        obj->setParent( top );
    }
    return top;
}

static _Object* createSocket( _Context* ctx, string prefix )
{
    _Object* top = new _Object( ctx, prefix, PLI_OBJ_SOCKET );

    float freq[4] = {1.2,1.2, 3.0, 3.0}; 

    for ( int i = 0; i < NUM_CORES; i++ ) {
        ostringstream tmp;
        tmp << i;
        _Object* obj = new _Object( ctx, prefix + ".core" + tmp.str(), PLI_OBJ_CORE);
        top->children().add( obj ); 
        obj->setParent( top );

        _AttributeNumTemplate< float >* attrFloat = 
                new _AttributeNumTemplate< float >( PLI_ATTR_FREQ, 
                         PLI_ATTR_FLOAT, PLI_ATTR_UNIT_MEGA, 1, 10, freq[i] );
        obj->attributeAdd( attrFloat );

        _AttributeStringTemplate< string >* attrString = 
                new _AttributeStringTemplate< string >( PLI_ATTR_STATE, 
                             PLI_ATTR_STRING, "ON|OFF", "ON" );
        obj->attributeAdd( attrString );

        _AttributeNumTemplate< int >* attrInt = 
                new _AttributeNumTemplate< int >( PLI_ATTR_ID, 
                             PLI_ATTR_INT, PLI_ATTR_UNIT_1, 0, 4, i );
        obj->attributeAdd( attrInt );
    }
    return top;
}

}

