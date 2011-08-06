/*
 * ODEPhysics.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */
#include "ODEPhysics.h"

#include "../../../Core/Core.h"

#include "types/PhysicPlane.h"
#include "types/PhysicSphere.h"
#include "types/PhysicBox.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#define MAX_CONTACTS 3
#define GRAVITY_FACTOR 1

using namespace std;

ODEPhysics::ODEPhysics()
	: id("ODEPhysics"),
	  type("physics")
{
	this->doProcessing = false;
	this->runThread = false;
}

ODEPhysics::~ODEPhysics()
{

}

bool
ODEPhysics::initialize( TiXmlElement* )
{
	cout << endl << "=============== ODEPhysics initializing... ===============" << endl;

	dInitODE();

	this->worldID = dWorldCreate();
	this->spaceID = dHashSpaceCreate(0);
	dWorldSetGravity(this->worldID, 0.0f, -9.8f, 0.0f);
	dWorldSetCFM(this->worldID,1e-5);
	this->contactGroupID = dJointGroupCreate(0);

	//this->semA->grab();

	this->thread = make_thread();
	//this->thread->start( ODEPhysics::threadFunc, this );

	cout << "================ ODEPhysics initialized =================" << endl;

	return true;
}

bool
ODEPhysics::shutdown()
{
	cout << endl << "=============== ODEPhysics shutting down... ===============" << endl;

	this->doProcessing = false;
	this->runThread = false;
	this->semB->release();
	this->semA->release();

	//this->thread->join();
	delete this->thread;
	this->thread = 0;

	delete this->semB;
	delete this->semA;

	this->semB = 0;
	this->semA = 0;

	std::list<ODEPhysicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ODEPhysicsEntity* entity = *iter++;

		delete entity;
	}

	dWorldDestroy(this->worldID);
	dSpaceDestroy(this->spaceID);
	dCloseODE();

	cout << "================ ODEPhysics shutdown =================" << endl;

	return true;
}

bool
ODEPhysics::start()
{
	return true;
}

bool
ODEPhysics::stop()
{
	return true;
}

bool
ODEPhysics::pause()
{
	return true;
}

bool
ODEPhysics::process(double factor)
{
	//cout << "ODEPhysics::process enter" << endl;

	this->doSimulation();

	//cout << "ODEPhysics::process leave" << endl;

	return true;
}

bool
ODEPhysics::finalizeProcess()
{
	//cout << "ODEPhysics::finalizeProcess enter" << endl;

	this->generateEvents();

	//cout << "ODEPhysics::finalizeProcess leave" << endl;

	return true;
}

void
ODEPhysics::doSimulation()
{
	this->processEvents();

	dSpaceCollide(this->spaceID, this, ODEPhysics::collisionCallback);
	dWorldQuickStep(this->worldID, 0.001);
	dJointGroupEmpty(this->contactGroupID);

	this->updateEntities();
}

void
ODEPhysics::generateEvents()
{
	// update of new positions
	std::list<ODEPhysicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ODEPhysicsEntity* entity = *iter++;

		if ( false == entity->isStatic() )
		{
			//cout << "enqueued setOrientation in ODEPhysics targeted at GO '" << entity->getParent()->getName() << endl;

			const float* pos = entity->getPos();
			const float* rot = entity->getRot();
			const float* vel = entity->getVel();

			Event e( "updatePhysics" );
			e.setTarget( entity->getParent() );

			e.addValue( "pos", Value( pos ) );
			e.addValue( "rot", Value( rot ) );
			e.addValue( "vel", Value( vel ) );

			Core::getInstance().getEventManager().postEvent( e );
		}
	}
}

// this call should be guaranteed to come just from the main-thread during a sync-point
// when the event-manager dispatches the queue. never should one do a direct call
// to sendEvent, instead all events should be queued in the manager and be dispatched
// during the sync-point.
// => it is save to process receivedEvents in the thread and it is save to not protect
// it by semaphore because the thread does not process the queue during this call
bool
ODEPhysics::sendEvent( Event& e )
{
	this->receivedEvents.push_back( e );

	return true;
}

ODEPhysicsEntity*
ODEPhysics::createEntity( TiXmlElement* objectNode, IGameObject* parent )
{
	TiXmlElement* typeNode = objectNode->FirstChildElement( "type" );
	if ( 0 == typeNode )
	{
		cout << "ERROR ... no type-node found for physics-instance - ignoring object" << endl;
		return 0;
	}

	string typeID;
	const char* str = typeNode->Attribute( "id" );
	if ( 0 == str)
	{
		cout << "ERROR ... no id attribute found in physic type - ignoring object" << endl;
		return 0;
	}
	else
	{
		typeID = str;
	}

	Vector v;
	float mass = 1.0f;
	bool staticFlag = false;

	str = typeNode->Attribute( "px" );
	if ( 0 != str )
	{
		v.data[0] = atof( str );
	}

	str = typeNode->Attribute( "py" );
	if ( 0 != str )
	{
		v.data[1] = atof( str );
	}

	str = typeNode->Attribute( "pz" );
	if ( 0 != str )
	{
		v.data[2] = atof( str );
	}

	str = typeNode->Attribute( "mass" );
	if ( 0 != str )
	{
		mass = atof( str );
	}

	str = typeNode->Attribute( "static" );
	if ( 0 != str )
	{
		if ( strcasecmp( "true", str ) == 0 )
		{
			staticFlag = true;
		}
	}

	ODEPhysicsEntity* entity = new ODEPhysicsEntity( parent );

	if ( "SPHERE" == typeID )
	{
		float r = 0;

		str = typeNode->Attribute( "r" );
		if ( 0 != str )
		{
			r = atof( str );
		}

		entity->physicType = new PhysicSphere( staticFlag, mass, r );
	}
	else if ( "BOX" == typeID )
	{
		float sx = 0;
		float sy = 0;
		float sz = 0;

		str = typeNode->Attribute( "sx" );
		if ( 0 != str )
		{
			sx = atof( str );
		}

		str = typeNode->Attribute( "sy" );
		if ( 0 != str )
		{
			sy = atof( str );
		}

		str = typeNode->Attribute( "sz" );
		if ( 0 != str )
		{
			sz = atof( str );
		}

		entity->physicType = new PhysicBox( staticFlag, mass, sx, sy, sz );
	}
	else if ( "PLANE" == typeID )
	{
		float dx = 0;
		float dy = 0;
		float dz = 0;
		float d = 0;

		str = typeNode->Attribute( "dx" );
		if ( 0 != str )
		{
			dx = atof( str );
		}

		str = typeNode->Attribute( "dy" );
		if ( 0 != str )
		{
			dy = atof( str );
		}

		str = typeNode->Attribute( "dz" );
		if ( 0 != str )
		{
			dz = atof( str );
		}

		str = typeNode->Attribute( "d" );
		if ( 0 != str )
		{
			d = atof( str );
		}

		entity->physicType = new PhysicPlane( true, mass, dx, dy, dz, d );
	}

	entity->physicType->create( this->worldID, this->spaceID );

	if ( false == entity->physicType->isStatic() )
		entity->physicType->setPosition( v );

	this->entities.push_back( entity );

	return entity;
}

void
ODEPhysics::updateEntities()
{
	std::list<ODEPhysicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ODEPhysicsEntity* entity = *iter++;

		if ( false == entity->isStatic() )
			entity->update();
	}
}

void
ODEPhysics::processEvents()
{
	std::list<Event>::iterator eventsIter = this->receivedEvents.begin();
	while ( eventsIter != this->receivedEvents.end() )
	{
		Event& e = *eventsIter++;

		cout << "received Event " << e.getID() << " in ODEPhysics as target" << endl;
	}

	this->receivedEvents.clear();

	// process events of entities
	std::list<ODEPhysicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ODEPhysicsEntity* entity = *iter++;

		eventsIter = entity->queuedEvents.begin();
		while ( eventsIter != entity->queuedEvents.end() )
		{
			Event& e = *eventsIter++;

			cout << "received Event " << e.getID() << " in ODEPhysics from GO '" << entity->getParent()->getName() << endl;

			if ( e == "moveForward" )
			{
				// TODO: alter position
			}
		}

		entity->queuedEvents.clear();
	}
}

void
ODEPhysics::collisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	ODEPhysics* instance = (ODEPhysics*) data;

	dContact contacts[MAX_CONTACTS];
	for (int i = 0; i < MAX_CONTACTS; i++) {
		contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
		contacts[i].surface.mu = dInfinity;
		contacts[i].surface.mu2 = 0;
		contacts[i].surface.bounce = 0.8;
		contacts[i].surface.bounce_vel = 0.1;
		contacts[i].surface.soft_cfm = 0.01;
    }

	if (int totalContacts = dCollide(o1, o2, MAX_CONTACTS, &contacts[0].geom, sizeof(dContactGeom))) {
		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);

		for (int i = 0; i < totalContacts; i++) {
			// dJointCreateContact needs to know which world and joint group to work with as well as the dContact
			// object itself. It returns a new dJointID which we then use with dJointAttach to finally create the
			// temporary contact joint between the two geom bodies.
			dJointID c = dJointCreateContact(instance->worldID, instance->contactGroupID, contacts + i);

			dJointAttach(c, b1, b2);
		}
	}
}

void*
ODEPhysics::threadFunc(void* args)
{
	ODEPhysics* instance = (ODEPhysics*) args;

	cout << "ODEPhysics " << instance->id << " is up and processing" << endl;

	instance->runThread = true;
	while ( instance->runThread )
	{
		instance->doSimulation();
	}

	cout << "ODEPhysics " << instance->id << " finished thread" << endl;

	return 0;
}
