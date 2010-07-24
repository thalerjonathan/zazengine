#include "Scene.h"

#ifdef OCCLUSION_CULLING
	#include "graphic/OCRenderer.h"
#else
	#ifdef SFX_RENDERING
		#include "graphic/SFXRenderer.h"
	#else
		#include "StandardRenderer.h"
	#endif
#endif

#include "geom/GeomSphere.h"
#include "Model.h"

#include "../../../../Core/Utils/Math/Transform.h"

#ifdef USE_PHYSICS
#include "physic/PhysicSphere.h"
#endif

#include <iostream>

using namespace std;

Scene::Scene(const string& name, Camera* camera)
	: name(name)
{
	this->camera = camera;
	this->renderer = 0;
	this->sceneRoot = 0;
}

Scene::~Scene()
{	
	delete this->renderer;
	delete this->camera;
	delete this->sceneRoot;
}

GeomInstance* Scene::buildChildren(GeomType* model)
{
	GeomInstance* subInstance = new GeomInstance(model);
	
	for (unsigned int i = 0; i < model->children.size(); i++) {
		GeomInstance* childInstance = this->buildChildren(model->children[i]);
		childInstance->parent = subInstance;
		subInstance->children.push_back(childInstance);
	}
	
	return subInstance;
}

void Scene::load(bool randomizeInstances, int rows, int columns, int density)
{
	if (this->sceneRoot)
		delete this->sceneRoot;
	
	if (this->renderer)
		delete this->renderer;

#ifdef OCCLUSION_CULLING
	this->renderer = new OCRenderer(*this->camera, this->skyBoxFolder);
#else
#ifdef SFX_RENDERING
	this->renderer = new SFXRenderer(*this->camera, this->skyBoxFolder);
#else
	this->renderer = new StandardRenderer(*this->camera, this->skyBoxFolder);
#endif
#endif
	
	GeomType* sceneGeom = new GeomType();
	this->sceneRoot = new GeomInstance(sceneGeom);

	map<string, Entity>::iterator iter = this->entities.begin();
	while (iter != this->entities.end()) {
		Entity& entity = iter->second;

		if (entity.modelFile != "")
			Model::loadMesh(entity.name, entity.modelFile);

		iter++;
	}

	for (unsigned int i = 0; i < this->instances.size(); i++) {
		EntityInstance& instance = this->instances[i];
		Entity& entity = this->entities[instance.entity];

		GeomType* model = Model::get(instance.entity);
		if (model == 0) {
			cout << "ERROR ... instance " << instance.entity << " has no model - ignoring instance" << endl;
			continue;
		}

		model->material = Material::get(entity.material);

		GeomInstance* geomInstance = this->buildChildren(model);
/*
		if (model->getBBMin()[0] < this->sceneBBMin[0])
			this->sceneBBMin.data[0] = model->getBBMin()[0];

		if (model->getBBMin()[1] < this->sceneBBMin[1])
			this->sceneBBMin.data[1] = model->getBBMin()[1];

		if (model->getBBMin()[2] < this->sceneBBMin[2])
			this->sceneBBMin.data[2] = model->getBBMin()[2];

		if (model->getBBMax()[0] > this->sceneBBMax[0])
			this->sceneBBMax.data[0] = model->getBBMax()[0];

		if (model->getBBMax()[1] > this->sceneBBMax[1])
			this->sceneBBMax.data[1] = model->getBBMax()[1];

		if (model->getBBMax()[2] > this->sceneBBMax[2])
			this->sceneBBMax.data[2] = model->getBBMax()[2];
*/
		Transform* instancePosition = new Transform();
		instancePosition->setPosition(instance.position);

#ifdef USE_PHYSICS
		if (entity.physic) {
			Transform* instancePhysic = this->world->addPhysicEntity(entity.physic);
			if (instancePhysic) {
				geomInstance->transforms.push_back(instancePosition);
				geomInstance->transforms.push_back(instancePhysic);
				
				this->world->setPosition(instancePhysic, instance.position);
			} else {
				geomInstance->transforms.push_back(instancePosition);
			}
		} else {
			geomInstance->transforms.push_back(instancePosition);
		}
#else
		geomInstance->transforms.push_back(instancePosition);
#endif

		geomInstance->parent = this->sceneRoot;
		this->sceneRoot->children.push_back(geomInstance);
	}
	
	if (randomizeInstances)
		this->randomizeInstances(rows, columns, density);

#ifdef USE_PHYSICS
	for (unsigned int i = 0; i < this->additionalPhysics.size(); i++)
		this->world->addPhysicEntity(this->additionalPhysics[i]);
	
#ifdef PHYSIC_THREADED
	this->world->startPhysicThread();
#endif
#endif
		
	this->sceneMeasures.data[0] = this->sceneBBMax[0] - this->sceneBBMin[0];
	this->sceneMeasures.data[1] = this->sceneBBMax[1] - this->sceneBBMin[1];
	this->sceneMeasures.data[2] = this->sceneBBMax[2] - this->sceneBBMin[2];
	
	sceneGeom->setBB(this->sceneBBMin, this->sceneBBMax);
}

void Scene::setSceneBB(const Vector& sceneBBMin, const Vector& sceneBBMax)
{
	this->sceneBBMax = sceneBBMax;
	this->sceneBBMin = sceneBBMin;

	this->sceneMeasures.data[0] = this->sceneBBMax[0] - this->sceneBBMin[0];
	this->sceneMeasures.data[1] = this->sceneBBMax[1] - this->sceneBBMin[1];
	this->sceneMeasures.data[2] = this->sceneBBMax[2] - this->sceneBBMin[2];
}

void Scene::processFrame(double loopFactor)
{	
#ifdef USE_PHYSICS
	if (this->rainFlag) {
		if (SDL_GetTicks() - this->lastRainTick > 10) {
			Vector position;
			srand(SDL_GetTicks());
			position.data[0] = rand() % (int) this->sceneMeasures[0];
			position.data[1] = this->sceneMeasures[1] + 50;
			position.data[2] = rand() % (int) this->sceneMeasures[2];
			
			position.data[0] += this->sceneBBMin[0];
			position.data[1] += this->sceneBBMin[1];
			position.data[2] += this->sceneBBMin[2];
			
			GeomSphere* sphere = new GeomSphere(1);
			GeomInstance* sphereInstance = new GeomInstance(sphere);
			
			PhysicSphere* physicSphere = new PhysicSphere(false, 10, 1);
			Transform* instancePhysic = this->world->addPhysicEntity(physicSphere);
			if (instancePhysic) {
				sphereInstance->transforms.push_back(instancePhysic);
				this->world->setPosition(instancePhysic, position);
			} else {
				delete physicSphere;
				
				Transform* instancePosition = new Transform();
				sphereInstance->transforms.push_back(instancePosition);
			}
			
			sphereInstance->parent = this->sceneRoot;
			this->sceneRoot->children.push_back(sphereInstance);

			this->lastRainTick = SDL_GetTicks();
		}
	}
	
#ifndef PHYSIC_THREADED
	this->world->process(loopFactor);
#endif
#endif
	
	this->sceneRoot->transform = this->camera->modelView;
	this->processTransforms(this->sceneRoot);
	
	this->renderer->renderFrame(this->sceneRoot);
}

void Scene::processTransforms(GeomInstance* instance)
{	
	if (instance->parent)
		instance->transform = instance->parent->transform;

	for (unsigned int i = 0; i < instance->transforms.size(); i++) {
		Transform* transform = instance->transforms[i];
		instance->transform.multiplyInv(transform->matrix.data);
	}
	
	for (unsigned int i = 0; i < instance->children.size(); i++)
		this->processTransforms(instance->children[i]);
}

void Scene::printInfo()
{
	if (this->renderer)
		this->renderer->printInfo();
}

void Scene::randomizeInstances(int rows, int columns, int density)
{
	cout << "==================== Generating Random Scene ====================" << endl;
	
	int totalFaces = 0;
	
	Vector position;
	position.data[2] = 0;
	srand(time(NULL));
	
	map<string, int> instanceCount;
	
	for (int i = 0; i < rows; i++) {
		int maxDepth = 0;

		position.data[0] = 0;
		
		for (int j = 0; j < columns; j++) {
			Scene::Entity entity = this->getRandomEntity();
			GeomType* model = Model::get(entity.name);
			if (model == 0) {
				cout << "ERROR ... instance " << entity.name << " has no model - ignoring instance" << endl;
				continue;
			}

			model->material = Material::get(entity.material);
			totalFaces += model->getFaceCount();

			GeomInstance* geomInstance = this->buildChildren(model);

			Transform* instancePosition = new Transform();
			instancePosition->setPosition(position);

#ifdef USE_PHYSICS
			/*
			if (entity.physic) {
				Transform* instancePhysic = this->world->addPhysicEntity(entity.physic);
				if (instancePhysic) {
					geomInstance->transforms.push_back(instancePosition);
					geomInstance->transforms.push_back(instancePhysic);

					this->world->setPosition(instancePhysic, instance.position);
				} else {
					geomInstance->transforms.push_back(instancePosition);
				}
			} else {
				geomInstance->transforms.push_back(instancePosition);
			}
			*/
#else
			geomInstance->transforms.push_back(instancePosition);
#endif
			geomInstance->transforms.push_back(instancePosition);

			geomInstance->parent = this->sceneRoot;
			this->sceneRoot->children.push_back(geomInstance);

			Vector lowMax(model->getBBMax().data[0], model->getBBMin().data[1], model->getBBMax().data[2]);
			Vector p(model->getBBMin().data[0], model->getBBMin().data[1], model->getBBMax().data[2]);
			Vector widthDist(lowMax - p);
			Vector depthDist(model->getBBMin() - p);
			
			int width = widthDist.length() + density;
			int depth = depthDist.length() + density;
			
			position.data[0] += width;
			
			if (depth > maxDepth)
				maxDepth = depth;
		}
		
		position.data[2] += maxDepth;
	}
	
	this->sceneMeasures = position;
	
	map<string, int>::iterator iter = instanceCount.begin();
	while (iter != instanceCount.end()) {
		cout << iter->second << "x " << iter->first << endl;
		
		iter++;
	}
	
	cout << rows * columns << " instances total with " << totalFaces << " faces" << endl;
	cout << "=================================================================" << endl;
}

Scene::Entity Scene::getRandomEntity()
{
	int index = 0;
	int entityIndex = rand() % this->entities.size();

	map<string, Entity>::iterator iter = this->entities.begin();
	while (iter != this->entities.end()) {
		if (index == entityIndex)
			return iter->second;

		index++;
		iter++;
	}

	Entity nullEntity;
	return nullEntity;
}
