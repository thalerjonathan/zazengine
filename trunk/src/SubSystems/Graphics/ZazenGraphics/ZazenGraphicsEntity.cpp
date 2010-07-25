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

ZazenGraphicsEntity::ZazenGraphicsEntity()
	: type( "graphics" )
{

}

ZazenGraphicsEntity::~ZazenGraphicsEntity()
{

}

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
			// TODO: do update of this
		}
		else
		{
			cout << "internal error: producer typename is physics but not of type IPhysicsEntity" << endl;
		}
	}
}