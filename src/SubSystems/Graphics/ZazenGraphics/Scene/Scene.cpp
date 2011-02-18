#include "Scene.h"

#include "../Renderer/DRRenderer.h"
#include "../Geometry/GeometryFactory.h"

#include <iostream>

using namespace std;

Scene::Scene( const string& name, Viewer* camera )
	: name( name )
{
	this->camera = camera;
	this->renderer = 0;
}

Scene::~Scene()
{	
	delete this->renderer;
	delete this->camera;
}

bool
Scene::load()
{
	if ( this->renderer )
		delete this->renderer;

	this->renderer = new DRRenderer( this->camera );
	if ( false == this->renderer->initialize() )
	{
		cout << "ERROR ... initializing renderer failed - exit" << endl;
		return false;
	}

	map<string, EntityDefinition>::iterator iter = this->entitiesDef.begin();
	while ( iter != this->entitiesDef.end() )
	{
		EntityDefinition& entity = iter->second;

		if ( entity.modelFile != "" )
			GeometryFactory::loadMesh( entity.name, entity.modelFile );

		iter++;
	}

	for (unsigned int i = 0; i < this->instanceDef.size(); i++)
	{
		InstanceDefinition* instanceDef = this->instanceDef[ i ];
		EntityDefinition entityDef = this->entitiesDef[ instanceDef->entity ];

		GeomType* model = GeometryFactory::get( instanceDef->entity );
		if ( 0 == model )
		{
			cout << "ERROR ... instance \"" << instanceDef->entity << "\" has no model - ignoring instance" << endl;
			continue;
		}

		Instance* newInstance = new Instance( model );
		newInstance->m_modelMatrix = instanceDef->modelMatrix;
		newInstance->id = instanceDef->id;

		this->instances.push_back( newInstance );
	}
		
	return true;
}

bool
Scene::processFrame( double loopFactor )
{	
	return this->renderer->renderFrame( this->instances );
}

Instance*
Scene::getInstanceByID( const std::string& id )
{
	std::list<Instance*>::iterator iter = this->instances.begin();
	while ( iter != this->instances.end() )
	{
		Instance* instance = *iter;

		if ( instance->id == id )
		{
			return instance;
		}
	}

	return 0;
}

void
Scene::printInfo()
{
}
