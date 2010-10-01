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
	this->semA = 0;
	this->semB = 0;

	this->thread = 0;

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
	dWorldSetGravity(this->worldID, 0.0f, -9.8f * GRAVITY_FACTOR, 0.0f);
	this->spaceID = dHashSpaceCreate(0);
	this->contactGroupID = dJointGroupCreate(0);

	this->semA = new Semaphore();
	this->semB = new Semaphore();

	this->semA->grab();

	this->thread = new Thread();
	this->thread->start( ODEPhysics::threadFunc, this );

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

	this->thread->join();
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
	cout << "ODEPhysics::process enter" << endl;

	this->doProcessing = true;

	this->semB->release();
	this->semA->release();

	cout << "ODEPhysics::process leave" << endl;

	return true;
}

bool
ODEPhysics::finalizeProcess()
{
	cout << "ODEPhysics::finalizeProcess enter" << endl;

	this->semB->grab();

	this->doProcessing = false;

	this->semA->grab();

	this->semB->release();

	cout << "ODEPhysics::finalizeProcess leave" << endl;

	//this->generateEvents();

	return true;
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
			cout << "enqueued setOrientation in ODEPhysics targeted at GO '" << entity->getParent()->getName() << endl;

			Event e( "setOrientation" );
			e.target = entity->getParent();

			Core::getInstance().getEventManager().postEvent( e );
		}
	}
}

// this call should be guaranteed to come just from the main-thread during a sync-point
// when the event-manager dispatches the queue. never should one do a direct call
// to sendEvent, instead all events should be queued in the manager and be dispatched
// during the sync-point.
// => it is save to process receivedEvents in the thread
bool
ODEPhysics::sendEvent(const Event& e)
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
	str = typeNode->Attribute( "x" );
	if ( 0 != str )
	{
		v.data[0] = atof( str );
	}

	str = typeNode->Attribute( "y" );
	if ( 0 != str )
	{
		v.data[1] = atof( str );
	}

	str = typeNode->Attribute( "z" );
	if ( 0 != str )
	{
		v.data[2] = atof( str );
	}

	ODEPhysicsEntity* entity = new ODEPhysicsEntity( parent );

	if ( "SPHERE" == typeID )
	{
		entity->physicType = new PhysicSphere( false, 1, 1 );
	}
	else if ( "BOX" == typeID )
	{
		entity->physicType = new PhysicBox( false, 1, 1, 1, 1 );
	}
	else if ( "PLANE" == typeID )
	{
		entity->physicType = new PhysicPlane( true, 1, 0, 0, 0, 1 );
	}

	entity->physicType->create( this->worldID, this->spaceID );

	if ( false == entity->physicType->isStatic() )
		entity->physicType->setPosition( v );

	this->entities.push_back( entity );

	return entity;
}

void
ODEPhysics::processEvents()
{
	std::list<Event>::iterator eventsIter = this->receivedEvents.begin();
	while ( eventsIter != this->receivedEvents.end() )
	{
		Event& e = *eventsIter++;

		cout << "received Event " << e.id << " in ODEPhysics as target" << endl;
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

			cout << "received Event " << e.id << " in ODEPhysics from GO '" << entity->getParent()->getName() << endl;

			if ( e == "moveForward" )
			{
				// TODO: alter position
			}
		}

		entity->queuedEvents.clear();
	}
}

void
ODEPhysics::doSimulation()
{
	this->semA->grab();

	this->processEvents();

	while ( this->doProcessing )
	{
		for (int i = 0; i < 10; i++) {
			dSpaceCollide(this->spaceID, this, ODEPhysics::collisionCallback);
			dWorldQuickStep(this->worldID, 0.001);
			dJointGroupEmpty(this->contactGroupID);
		}
	}

	this->generateEvents();

	this->semA->release();

	this->semB->grab();
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
