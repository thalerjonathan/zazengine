/*
 * ODEPhysicsEntity.cpp
 *
 *  Created on: Sep 30, 2010
 *      Author: jonathan
 */

#include "ODEPhysicsEntity.h"

#include <iostream>

using namespace std;

ODEPhysicsEntity::ODEPhysicsEntity( IGameObject* p )
	: IPhysicsEntity( p ),
	entityType( "physics" )
{
	this->physicType = 0;
}

ODEPhysicsEntity::~ODEPhysicsEntity()
{

}

bool ODEPhysicsEntity::sendEvent(const Event& e)
{
	if ( e == "moveForward" )
	{
		this->queuedEvents.push_back( e );
		return true;
	}

	return false;
}

