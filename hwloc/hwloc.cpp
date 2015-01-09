/* Simple utility for generating a topology for use with the powerapi prototype 
 * Creates a file hwloc.xml formatted appropriately.
 */

#include "tinyxml2.h"
#include <string>

using namespace tinyxml2;

// Translates the hwloc XML to powerapi terminology.
int translate(XMLDocument* doc, XMLElement* e, XMLElement* parent, int index){
  int initial = index;
  XMLElement * hwloc_object = e->FirstChildElement("object");
  while(hwloc_object != NULL){
    char name[256];
    if(hwloc_object->Attribute("type", "Machine")){
      XMLElement * child = doc->NewElement("obj");
      snprintf(name, 256, "node%d", index++);
      child->SetAttribute("name", name); 
      child->SetAttribute("type", "Node"); 
      parent->InsertEndChild(child);
      translate(doc, hwloc_object, child, 0);
    } else if (hwloc_object->Attribute("type", "Socket")){
      XMLElement * child = doc->NewElement("obj");
      snprintf(name, 256, "socket%d", index++);
      child->SetAttribute("name", name); 
      child->SetAttribute("type", "Socket"); 
      parent->InsertEndChild(child);
      translate(doc, hwloc_object, child, 0);
    } else if (hwloc_object->Attribute("type", "Core")){
      XMLElement * child = doc->NewElement("obj");
      snprintf(name, 256, "core%d", index++);
      child->SetAttribute("name", name); 
      child->SetAttribute("type", "Core"); 
      parent->InsertEndChild(child);
      translate(doc, hwloc_object, child, 0);
    } else {
      index += translate(doc, hwloc_object, parent, index);
    }
    hwloc_object = hwloc_object->NextSiblingElement("object");
  }
  return index - initial;
}

// Flattens the XML objects so that children are not XML children :(
void flatten(XMLDocument* doc, XMLElement* e, XMLElement* objs, char* prefix){
  XMLElement * children = doc->NewElement("children");
  XMLElement * child = e->FirstChildElement("obj");
  char name[256];
  while(child != NULL){
    XMLElement * c = doc->NewElement("child");
    c->SetAttribute("name", child->Attribute("name"));
    children->InsertEndChild(c);
    snprintf(name, 256, "%s.%s", prefix, child->Attribute("name"));
    child->SetAttribute("name", name);
    XMLElement * newchild = child->NextSiblingElement("obj");
    objs->InsertEndChild(child);
    flatten(doc, child, objs, name);
    child = newchild;
  }
  e->InsertEndChild(children);
}

int main(){
  unsigned int err;

  // Load hwloc document, create new doc
  err = system("lstopo hwloc.xml -f");
  if(err){
    printf("lstopo failed, is hwloc installed?\n");
    return err;
  }

  XMLDocument doc;
  doc.LoadFile("hwloc.xml");
  XMLDocument newdoc;

  // System
  XMLNode * system = newdoc.NewElement("System"); 
  newdoc.InsertEndChild(system);
  
  // Plugins
  XMLElement * plugins = newdoc.NewElement("Plugins");
  system->InsertEndChild(plugins); 

  // Devices
  XMLElement * devices = newdoc.NewElement("Devices");
  system->InsertEndChild(devices);

  // Objects
  XMLElement * objects = newdoc.NewElement("Objects");
  system->InsertEndChild(objects);
  
  // Initialize with platform
  XMLElement * obj = newdoc.NewElement("obj");
  obj->SetAttribute("name", "plat");
  obj->SetAttribute("type", "Platform");
  objects->InsertEndChild(obj);
  
  // Iterate through hwloc xml, translating to powerapi topology
  translate(&newdoc, doc.FirstChildElement(), obj, 0);
  flatten(&newdoc, obj, objects, (char*)"plat");

  // Save output
  err = newdoc.SaveFile("hwloc.xml");
  if(err){
    printf("Error writing to hwloc.xml\n");
    return err;
  } else {
    printf("Output written to hwloc.xml\n");
  }
}
