#ifndef SCENE_H_
#define SCENE_H_

#include "Geominstance.h"
#include "Renderer.h"

#include <string>
#include <list>
#include <vector>

class Scene
{
 public:
	 typedef struct {
		 std::string name;
		 std::string modelFile;
		 std::string material;

#ifdef USE_PHYSICS
		PhysicType* physic;
#endif
	 } Entity;
 
	 typedef struct {
		 std::string entity;
		 Vector position;
		 float size;
	 } EntityInstance;

	 Scene(const std::string&, Camera*);
	 ~Scene();
	
#ifdef USE_PHYSICS
	 void doRain() { this->rainFlag = !this->rainFlag; this->lastRainTick = SDL_GetTicks(); };
#endif
	 
	 void setSkyBoxFolder(std::string& f) { this->skyBoxFolder = f; };
	 void setSceneBB(const Vector& sceneBBMin, const Vector& sceneBBMax);

	 void processFrame(double);
	 void load(bool, int, int, int);

#ifdef USE_PHYSICS
	 void addPhysics(PhysicType* p) { this->additionalPhysics.push_back(p); };
#endif
	 
	 void addEntity(Entity& e) { this->entities[e.name] = e; };
	 void addInstance(EntityInstance& i) { this->instances.push_back(i); };
	 
	 void printInfo();
	 
 private:
	 const std::string name;
	 
	 Vector sceneBBMin;
	 Vector sceneBBMax;
	 Vector sceneMeasures;

	 std::string skyBoxFolder;

#ifdef USE_PHYSICS
	 std::vector<PhysicType*> additionalPhysics;
#endif
	 
	 std::map<std::string, Entity> entities;
	 std::vector<EntityInstance> instances;
	 
	 Camera* camera;
	 Renderer* renderer;
#ifdef USE_PHYSICS
	 World* world;
#endif
	 GeomInstance* sceneRoot;
	 GeomInstance* skyBoxInstance;

	 void processTransforms(GeomInstance*);
	 
	 GeomInstance* buildChildren(GeomType*);
	 
	 void randomizeInstances(int, int, int);

#ifdef USE_PHYSICS
	bool rainFlag;
	int lastRainTick;
#endif

	Entity getRandomEntity();

};

#endif /*SCENE_H_*/
