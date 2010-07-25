/*
 * FModAudioEntity.cpp
 *
 *  Created on: 09.07.2010
 *      Author: joni
 */

#include "FModAudioEntity.h"

#include "../../../Core/SubSystems/IFaces/IPhysicsEntity.h"

#include <iostream>

using namespace std;

FModAudioEntity::FModAudioEntity()
	:type( "audio" )
{
	this->sound = 0;
	this->channel = 0;
}

FModAudioEntity::~FModAudioEntity()
{

}

vector<string>
FModAudioEntity::getDependencies() const
{
	vector<string> dep;
	dep.push_back( "physics" );

	return dep;
}

void
FModAudioEntity::consume( ISubSystemEntity* producer )
{
	if ( "physics" == producer->getType() )
	{
		IPhysicsEntity* physics = dynamic_cast<IPhysicsEntity*>( producer );
		if ( 0 != physics )
		{
			if ( false == physics->isStatic() )
			{
				const float* physicsPos = physics->getPos();
				const float* physicsVel = physics->getVel();

				FMOD_VECTOR position;
				FMOD_VECTOR velocity;

				position.x = physicsPos[0];
				position.y = physicsPos[1];
				position.z = physicsPos[2];

				velocity.x = physicsVel[0];
				velocity.y = physicsVel[1];
				velocity.z = physicsVel[2];

				this->channel->set3DAttributes( &position, &velocity );
			}
		}
		else
		{
			cout << "internal error: producer typename is physics but not of type IPhysicsEntity" << endl;
		}
	}
}

