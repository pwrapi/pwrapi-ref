/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

/* Simple utility for generating a topology for use with the powerapi prototype 
 * Creates a file hwloc.xml formatted appropriately.
 * Each object gets a "<name><id>" where id is a unique identifier accross 
 * all object types.
 */

#include "tinyxml2.h"
#include <string>
#include <stdlib.h>
#include <unistd.h>

using namespace tinyxml2;

// Translates the hwloc XML to powerapi terminology.
int translate( XMLDocument* doc, XMLElement* e, XMLElement* parent, int index )
{
    XMLElement * hwloc_object = e->FirstChildElement( "object" );
    while( hwloc_object != NULL ) {
        if( hwloc_object->Attribute( "type", "Machine" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            child->SetAttribute( "name", "node" ); 
            child->SetAttribute( "type", "Node" ); 
            child->SetAttribute( "index", index++ );
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else if( hwloc_object->Attribute( "type", "Socket" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            child->SetAttribute( "name", "socket" ); 
            child->SetAttribute( "type", "Socket" ); 
            child->SetAttribute( "index", index++ );
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else if( hwloc_object->Attribute( "type", "Core" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            child->SetAttribute( "name", "core" ); 
            child->SetAttribute( "type", "Core" ); 
            child->SetAttribute( "index", index++ );
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else {
            index = translate( doc, hwloc_object, parent, index );
        }

        hwloc_object = hwloc_object->NextSiblingElement( "object" );
    }
    return index;
}

// Flattens the XML objects so that children are not XML children :(
void flatten( XMLDocument* doc, XMLElement* e, XMLElement* objs, char* prefix )
{
    XMLElement * children = doc->NewElement( "children" );
    XMLElement * child = e->FirstChildElement( "obj" );
    char name[256];

    while( child != NULL ) {
        XMLElement * c = doc->NewElement( "child" );
        snprintf( name, 256, "%s%d", child->Attribute( "name" ), atoi(child->Attribute( "index" )));
        c->SetAttribute( "name", name );
        children->InsertEndChild( c );
        snprintf( name, 256, "%s.%s%d", prefix, child->Attribute( "name" ), atoi(child->Attribute( "index" )) );
        child->SetAttribute( "name", name );
        child->DeleteAttribute( "index" );
        XMLElement * newchild = child->NextSiblingElement( "obj" );
        objs->InsertEndChild( child );
        flatten( doc, child, objs, name );
        child = newchild;
    }

    e->InsertEndChild( children );
}


XMLElement* DeepClone( XMLNode* input, XMLDocument* document ) 
{
    XMLElement* output = input->ShallowClone( document )->ToElement();
    XMLElement* child = input->FirstChildElement();
    while( child != NULL ){
        output->InsertEndChild( DeepClone( child, document ) );
        child = child->NextSiblingElement( );
    }
    return output;
}

// Duplicate the nodes, adds unique index to each object
void duplicate( XMLDocument* doc, XMLElement* plat, int nodecount )
{
    XMLElement * node = plat->FirstChildElement( "obj" );
    for( int i = 1; i < nodecount; i++ ){
        XMLElement * newdup = DeepClone( node, doc );
        newdup->SetAttribute( "index", i ); 
        plat->InsertEndChild( newdup );
    }
} 

int main( int argc, char** argv )
{
    const char USAGE[] = "usage: %s [-f filename] [-n nodecount] [-h]\n";
    const char FILENAME[] = "hwloc.xml";

    char syscall[256];
    char *filename = (char *)FILENAME;
    int option, nodecount = 0;

    while( (option=getopt( argc, argv, "f:n:h" )) != -1 )
        switch( option ) {
            case 'f':
                filename = optarg;
                break;
            case 'n':
                nodecount = atoi( optarg );
                break;
            case 'h':
            default:
                fprintf( stderr, USAGE, argv[0] );
                return -1;
        }

    // Load hwloc document, create new doc
    unsigned int err;
    snprintf( syscall, 256, "lstopo %s", filename );
    if( (err=system( syscall )) ) {
        printf( "lstopo failed, is hwloc installed?\n" );
        return err;
    }

    XMLDocument doc, newdoc;
    doc.LoadFile( filename );

    // System
    XMLNode * system = newdoc.NewElement( "System" ); 
    newdoc.InsertEndChild( system );
  
    // Plugins
    XMLElement * plugins = newdoc.NewElement( "Plugins" );
    system->InsertEndChild( plugins ); 

    // Devices
    XMLElement * devices = newdoc.NewElement( "Devices" );
    system->InsertEndChild( devices );

    // Objects
    XMLElement * objects = newdoc.NewElement( "Objects" );
    system->InsertEndChild( objects );
  
    // Initialize with platform
    XMLElement * obj = newdoc.NewElement( "obj" );
    obj->SetAttribute( "name", "plat" );
    obj->SetAttribute( "type", "Platform" );
    objects->InsertEndChild( obj );
  
    // Iterate through hwloc xml, translating to powerapi topology
    translate( &newdoc, doc.FirstChildElement(), obj , 0 );
    duplicate( &newdoc, obj, nodecount );
    flatten( &newdoc, obj, objects, (char*)"plat" );

    // Save output
    if( (err=newdoc.SaveFile( filename )) ) {
        printf( "Error writing to %s\n", filename );
        return err;
    } else {
        printf( "Output written to %s\n", filename );
    }

    return 0;
}
