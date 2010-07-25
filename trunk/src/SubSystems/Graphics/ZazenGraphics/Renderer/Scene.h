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
	 } Entity;
 
	 typedef struct {
		 std::string entity;
		 float size;
		 Transform* transform;
	 } EntityInstance;

	 Scene(const std::string&, Camera*);
	 ~Scene();
	 
	 void setSkyBoxFolder(std::string& f) { this->skyBoxFolder = f; };
	 void setSceneBB(const Vector& sceneBBMin, const Vector& sceneBBMax);

	 void processFrame(double);
	 void load(bool, int, int, int);
	 
	 void addEntity(Entity& e) { this->entities[e.name] = e; };
	 void addInstance(EntityInstance* i) { this->instances.push_back(i); };
	 
	 void printInfo();
	 
 private:
	 const std::string name;
	 
	 Vector sceneBBMin;
	 Vector sceneBBMax;
	 Vector sceneMeasures;

	 std::string skyBoxFolder;

	 std::map<std::string, Entity> entities;
	 std::vector<EntityInstance*> instances;
	 
	 Camera* camera;
	 Renderer* renderer;

	 GeomInstance* sceneRoot;
	 GeomInstance* skyBoxInstance;

	 void processTransforms(GeomInstance*);
	 
	 GeomInstance* buildChildren(GeomType*);
	 
	 void randomizeInstances(int, int, int);

	Entity getRandomEntity();

};

#endif /*SCENE_H_*/
