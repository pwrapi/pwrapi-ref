/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
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
 */

#include "tinyxml2.h"
#include <string>

using namespace tinyxml2;

// Translates the hwloc XML to powerapi terminology.
int translate( XMLDocument* doc, XMLElement* e, XMLElement* parent, int index )
{
    int initial = index;
    XMLElement * hwloc_object = e->FirstChildElement( "object" );

    while( hwloc_object != NULL ) {
        char name[256];

        if( hwloc_object->Attribute( "type", "Machine" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            snprintf( name, 256, "node%d", index++ );
            child->SetAttribute( "name", name ); 
            child->SetAttribute( "type", "Node" ); 
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else if( hwloc_object->Attribute( "type", "Socket" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            snprintf( name, 256, "socket%d", index++ );
            child->SetAttribute( "name", name ); 
            child->SetAttribute( "type", "Socket" ); 
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else if( hwloc_object->Attribute( "type", "Core" ) ) {
            XMLElement * child = doc->NewElement( "obj" );
            snprintf( name, 256, "core%d", index++ );
            child->SetAttribute( "name", name ); 
            child->SetAttribute( "type", "Core" ); 
            parent->InsertEndChild( child );
            translate( doc, hwloc_object, child, 0 );
        } else {
            index += translate( doc, hwloc_object, parent, index );
        }

        hwloc_object = hwloc_object->NextSiblingElement( "object" );
    }

    return index - initial;
}

// Flattens the XML objects so that children are not XML children :(
void flatten( XMLDocument* doc, XMLElement* e, XMLElement* objs, char* prefix )
{
    XMLElement * children = doc->NewElement( "children" );
    XMLElement * child = e->FirstChildElement( "obj" );
    char name[256];

    while( child != NULL ) {
        XMLElement * c = doc->NewElement( "child" );
        c->SetAttribute( "name", child->Attribute( "name" ) );
        children->InsertEndChild( c );
        snprintf( name, 256, "%s.%s", prefix, child->Attribute( "name" ) );
        child->SetAttribute( "name", name );
        XMLElement * newchild = child->NextSiblingElement( "obj" );
        objs->InsertEndChild( child );
        flatten( doc, child, objs, name );
        child = newchild;
    }

    e->InsertEndChild( children );
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
    translate( &newdoc, doc.FirstChildElement(), obj, nodecount );
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
