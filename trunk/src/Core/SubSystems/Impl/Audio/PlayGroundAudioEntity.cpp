/*
 * PlayGroundAudioEntity.cpp
 *
 *  Created on: 09.07.2010
 *      Author: joni
 */

#include "PlayGroundAudioEntity.h"

#include "../../IFaces/IPhysicsEntity.h"

#include <iostream>

using namespace std;

PlayGroundAudioEntity::PlayGroundAudioEntity()
	:type( "audio" )
{

}

PlayGroundAudioEntity::~PlayGroundAudioEntity()
{

}

void
PlayGroundAudioEntity::consume( ISubSystemEntity* producer )
{
	if ( "physics" == producer->getType() )
	{
		IPhysicsEntity* physics = dynamic_cast<IPhysicsEntity*>( producer );
		if ( 0 != physics )
		{
			// TODO: do update of this
		}
		else
		{
			cout << "internal error: producer typename is physics but not of type IPhysicsEntity" << endl;
		}
	}
}

