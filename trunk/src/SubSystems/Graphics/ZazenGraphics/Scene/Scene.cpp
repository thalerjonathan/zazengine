#include "Scene.h"

#ifdef OCCLUSION_CULLING
	#include "../Renderer/OCRenderer.h"
#elif SFX_RENDERING
	#include "../Renderer/SFXRenderer.h"
#elif DR_RENDERING
	#include "../Renderer/DRRenderer.h"
#else
	#include "../Renderer/StandardRenderer.h"
#endif

#include "../Geometry/GeomSphere.h"
#include "../Geometry/GeometryFactory.h"

#include "../../../../Core/Utils/Math/Transform.h"

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

bool Scene::load(bool randomizeInstances, int rows, int columns, int density)
{
	if (this->sceneRoot)
		delete this->sceneRoot;
	
	if (this->renderer)
		delete this->renderer;

#ifdef OCCLUSION_CULLING
	this->renderer = new OCRenderer(*this->camera, this->skyBoxFolder);
#elif SFX_RENDERING
	this->renderer = new SFXRenderer(*this->camera, this->skyBoxFolder);
#elif DR_RENDERING
	this->renderer = new DRRenderer(*this->camera, this->skyBoxFolder);
#else
	this->renderer = new StandardRenderer(*this->camera, this->skyBoxFolder);
#endif
	
	if ( false == this->renderer->initialize() )
	{
		cout << "ERROR ... initializing renderer failed - exit" << endl;
		return false;
	}

	GeomType* sceneGeom = new GeomType();
	this->sceneRoot = new GeomInstance(sceneGeom);

	map<string, Entity>::iterator iter = this->entities.begin();
	while (iter != this->entities.end()) {
		Entity& entity = iter->second;

		if (entity.modelFile != "")
			GeometryFactory::loadMesh(entity.name, entity.modelFile);

		iter++;
	}

	for (unsigned int i = 0; i < this->instances.size(); i++) {
		EntityInstance* instance = this->instances[i];
		Entity entity = this->entities[instance->entity];

		GeomType* model = GeometryFactory::get(instance->entity);
		if (model == 0) {
			cout << "ERROR ... instance " << instance->entity << " has no model - ignoring instance" << endl;
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

		geomInstance->transforms.push_back(instance->transform);

		geomInstance->parent = this->sceneRoot;
		this->sceneRoot->children.push_back(geomInstance);
	}
	
	if (randomizeInstances)
		this->randomizeInstances(rows, columns, density);
		
	this->sceneMeasures.data[0] = this->sceneBBMax[0] - this->sceneBBMin[0];
	this->sceneMeasures.data[1] = this->sceneBBMax[1] - this->sceneBBMin[1];
	this->sceneMeasures.data[2] = this->sceneBBMax[2] - this->sceneBBMin[2];
	
	sceneGeom->setBB(this->sceneBBMin, this->sceneBBMax);

	return true;
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
			GeomType* model = GeometryFactory::get(entity.name);
			if (model == 0) {
				cout << "ERROR ... instance " << entity.name << " has no model - ignoring instance" << endl;
				continue;
			}

			model->material = Material::get(entity.material);
			totalFaces += model->getFaceCount();

			GeomInstance* geomInstance = this->buildChildren(model);

			Transform* instancePosition = new Transform();
			instancePosition->setPosition(position);

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
