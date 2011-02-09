#ifndef SCENE_H_
#define SCENE_H_

#include "Instance.h"
#include "../Renderer/Renderer.h"

#include <string>
#include <list>
#include <vector>
#include <map>

class Scene
{
 public:
	 typedef struct {
		 std::string name;
		 std::string modelFile;
	 } EntityDefinition;
 
	 typedef struct {
		 std::string entity;
		 float size;
		 glm::mat4* modelMatrix; // is contained within a graphics entity and allows the instance to be moved externally
	 } InstanceDefinition;

	 Scene( const std::string&, Viewer* );
	 ~Scene();
	 
	 bool processFrame( double );
	 bool load();
	 
	 void addEntity( EntityDefinition& e ) { this->entitiesDef[ e.name ] = e; };
	 void addInstance( InstanceDefinition* i ) { this->instanceDef.push_back( i ); };
	 
	 void printInfo();
	 
 private:
	 const std::string name;
	 
	 Viewer* camera;
	 Renderer* renderer;

	 std::map<std::string, EntityDefinition> entitiesDef;
	 std::vector<InstanceDefinition*> instanceDef;

	 std::list<Instance*> instances;

};

#endif /*SCENE_H_*/
