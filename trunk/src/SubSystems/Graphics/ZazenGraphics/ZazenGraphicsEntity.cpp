/*
 * PlayGroundRendererEntity.cpp
 *
 *  Created on: 09.07.2010
 *      Author: joni
 */

#include "ZazenGraphicsEntity.h"

#include "../../../Core/SubSystems/IFaces/IPhysicsEntity.h"

#include <iostream>

using namespace std;

ZazenGraphicsEntity::ZazenGraphicsEntity( IGameObject* p )
	: IGraphicsEntity( p ),
	type( "graphics" )
{
	this->instance = 0;
}

ZazenGraphicsEntity::~ZazenGraphicsEntity()
{
	delete this->instance;
}

bool
ZazenGraphicsEntity::sendEvent( Event& e )
{
	// process immediately because no call to graphics api, just coping of data (yet)
	if ( e == "updatePhysics" )
	{
		Value& pos = e.getValue( "pos" );
		Value& rot = e.getValue( "rot" );

		this->setOrientation( pos.data, rot.data );

		return true;
	}

	return false;
}

void
ZazenGraphicsEntity::setOrientation( const float* pos, const float* rot)
{
	memcpy( this->instance->transform->matrix.data, rot, 11 * sizeof( float ) );
	memcpy( &this->instance->transform->matrix.data[12], pos, 3 * sizeof( float ) );

	//cout << "Graphics: " << this->getParent()->getName() << " has position of (" << pos[0] << "/" << pos[1] << "/" << pos[2] << ")" << endl;
	//cout << "Graphics: resulting matrix: " << endl;
	//this->instance->transform->matrix.print();
}
