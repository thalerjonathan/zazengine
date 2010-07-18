/*
 * GameObject.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "GameObject.h"

#include "../../Core.h"

#include <iostream>

using namespace std;

GameObject::GameObjectID GameObject::nextID = 0;

GameObject::GameObject( const std::string& name )
	: id ( GameObject::nextID++ ),
	  name ( name )
{
}

GameObject::~GameObject()
{
}

bool
GameObject::sendEvent( const Event& e )
{
	//cout << "Processing event " << e.id << " in EventProcessor " << this->id << endl;

	return true;
}
