/*
 * PlayGroundPhysics.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "PlayGroundPhysics.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#define MAX_CONTACTS 3
#define GRAVITY_FACTOR 1

using namespace std;

PlayGroundPhysics::PlayGroundPhysics()
	: id("PlayGroundPhysics"),
	  type("physics")
{
	this->sem = 0;
	this->thread = 0;

	this->doProcessing = false;
	this->runThread = false;
}

PlayGroundPhysics::~PlayGroundPhysics()
{

}

bool
PlayGroundPhysics::initialize( TiXmlElement* )
{
	cout << endl << "=============== PlayGroundPhysics initializing... ===============" << endl;

	dInitODE();

	this->worldID = dWorldCreate();
	dWorldSetGravity(this->worldID, 0.0f, -9.8f * GRAVITY_FACTOR, 0.0f);
	this->spaceID = dHashSpaceCreate(0);
	this->contactGroupID = dJointGroupCreate(0);

	this->sem = new Semaphore();
	this->sem->grab();

	this->thread = new Thread();
	this->thread->start( PlayGroundPhysics::threadFunc, this );

	cout << "================ PlayGroundPhysics initialized =================" << endl;

	return true;
}

bool
PlayGroundPhysics::shutdown()
{
	cout << endl << "=============== PlayGroundPhysics shutting down... ===============" << endl;

	this->doProcessing = false;
	this->runThread = false;
	this->sem->release();

	this->thread->join();
	delete this->thread;
	this->thread = 0;

	delete this->sem;
	this->sem = 0;

	dWorldDestroy(this->worldID);
	dSpaceDestroy(this->spaceID);
	dCloseODE();

	cout << "================ PlayGroundPhysics shutdown =================" << endl;

	return true;
}

bool
PlayGroundPhysics::start()
{
	return true;
}

bool
PlayGroundPhysics::stop()
{
	return true;
}

bool
PlayGroundPhysics::pause()
{
	return true;
}

bool
PlayGroundPhysics::process(double factor)
{
	//cout << "PlayGroundPhysics::process" << endl;

	this->doProcessing = true;
	this->sem->release();

	return true;
}

bool
PlayGroundPhysics::finalizeProcess()
{
	//cout << "PlayGroundPhysics::finalizeProcess" << endl;

	this->doProcessing = false;
	this->sem->grab();

	return true;
}

bool
PlayGroundPhysics::sendEvent(const Event& e)
{
	return true;
}

PlayGroundPhysicsEntity*
PlayGroundPhysics::createEntity( TiXmlElement* objectNode )
{
	PlayGroundPhysicsEntity* entity = new PlayGroundPhysicsEntity();

	TiXmlElement* instanceNode = objectNode->FirstChildElement( "type" );
	if ( 0 == instanceNode )
	{
		cout << "ERROR ... no type-node found for physics-instance - ignoring object" << endl;
		delete entity;
		return 0;
	}

	return entity;
}

void
PlayGroundPhysics::processInternal()
{
	this->sem->grab();

	while ( this->doProcessing )
	{
		for (int i = 0; i < 10; i++) {
			dSpaceCollide(this->spaceID, this, PlayGroundPhysics::collisionCallback);
			dWorldQuickStep(this->worldID, 0.001);
			dJointGroupEmpty(this->contactGroupID);
		}
	}

	/* TODO: update orientation matrices
	const dReal* pPos = dBodyGetPosition(type->getBodyID());
	const dReal* pRotMatrix = dBodyGetRotation(type->getBodyID());

	memcpy(node->matrix.data, pRotMatrix, 11 * sizeof(float));
	memcpy(&node->matrix.data[12], pPos, 3 * sizeof(float));
	*/

	this->sem->release();
}

void
PlayGroundPhysics::collisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	PlayGroundPhysics* instance = (PlayGroundPhysics*) data;

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
PlayGroundPhysics::threadFunc(void* args)
{
	PlayGroundPhysics* instance = (PlayGroundPhysics*) args;

	cout << "PlayGroundPhysics " << instance->id << " is up and processing" << endl;

	instance->runThread = true;
	while ( instance->runThread )
	{
		instance->processInternal();
	}

	cout << "PlayGroundPhysics " << instance->id << " finished thread" << endl;

	return 0;
}
