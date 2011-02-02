#ifndef SCENE_H_
#define SCENE_H_

#include "Instance.h"
#include "../Renderer/Renderer.h"

#include <string>
#include <list>
#include <vector>

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
		 Transform* transform;
	 } InstanceDefinition;

	 Scene( const std::string&, Camera* );
	 ~Scene();
	 
	 void setSkyBoxFolder( std::string& f ) { this->skyBoxFolder = f; };
	 void setSceneBB( const Vector& sceneBBMin, const Vector& sceneBBMax );

	 bool processFrame( double );
	 bool load( bool, int, int, int );
	 
	 void addEntity( EntityDefinition& e ) { this->entitiesDef[e.name] = e; };
	 void addInstance( InstanceDefinition* i ) { this->instanceDef.push_back(i); };
	 
	 void printInfo();
	 
 private:
	 const std::string name;
	 
	 Vector sceneBBMin;
	 Vector sceneBBMax;
	 Vector sceneMeasures;

	 std::string skyBoxFolder;

	 std::map<std::string, EntityDefinition> entitiesDef;
	 std::vector<InstanceDefinition*> instanceDef;
	 
	 Camera* camera;
	 Renderer* renderer;

	 std::list<Instance*> instances;

};

#endif /*SCENE_H_*/
