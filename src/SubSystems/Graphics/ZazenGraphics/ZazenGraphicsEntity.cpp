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
ZazenGraphicsEntity::sendEvent(const Event& e)
{
	if ( e == "setOrientation" )
	{
		this->queuedEvents.push_back( e );
		return true;
	}

	return false;
}

/*
vector<string>
ZazenGraphicsEntity::getDependencies() const
{
	vector<string> dep;
	dep.push_back( "physics" );

	return dep;
}

void
ZazenGraphicsEntity::consume( ISubSystemEntity* producer )
{
	if ( "physics" == producer->getType() )
	{
		IPhysicsEntity* physics = dynamic_cast<IPhysicsEntity*>( producer );
		if ( 0 != physics )
		{
			if ( false == physics->isStatic() )
			{
				const float* physicsPos = physics->getPos();
				const float* physicsRot = physics->getRot();

				memcpy(this->instance->transform->matrix.data, physicsRot, 11 * sizeof( float ) );
				memcpy(&this->instance->transform->matrix.data[12], physicsPos, 3 * sizeof( float ) );

				this->instance->transform->matrix.print();
			}
		}
		else
		{
			cout << "internal error: producer typename is physics but not of type IPhysicsEntity" << endl;
		}
	}
}
*/