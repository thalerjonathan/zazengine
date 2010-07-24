/*
 * GameObject.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "GameObject.h"

#include "../../Core.h"
#include "../../ScriptSystem/ScriptSystem.h"

#include <iostream>

using namespace std;

GameObject::GameObjectID GameObject::nextID = 0;

GameObject::GameObject( )
	: id ( GameObject::nextID++ )
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

bool
GameObject::initialize( TiXmlElement* objectNode )
{
	string objectName;
	string scriptFile;

	const char* str = objectNode->Attribute( "name" );
	if ( 0 == str )
	{
		cout << "WARNING: no id defined for object - will be ignored" << endl;
		return false;
	}
	else
	{
		this->name = str;
	}

	str = objectNode->Attribute( "script" );
	if ( 0 == str )
	{
		cout << "INFO: no script defined for object " << objectName << endl;
	}
	else
	{
		scriptFile = str;
	}

	if ( false == scriptFile.empty() )
	{
		if ( false == ScriptSystem::getInstance().loadFile( scriptFile ) )
		{
			return false;
		}
	}

	for (TiXmlElement* subSystemEntityNode = objectNode->FirstChildElement(); subSystemEntityNode != 0; subSystemEntityNode = subSystemEntityNode->NextSiblingElement())
	{
		const char* str = subSystemEntityNode->Value();
		if ( 0 == str )
			continue;

		ISubSystemEntity* subSystemEntity = 0;

		/*
		list<ISubSystem*>::iterator iter = this->subSystems.begin();
		while ( iter != this->subSystems.end() )
		{
			ISubSystem* subSys = *iter++;
			if ( subSys->getType() == str )
			{
				subSystemEntity = subSys->createEntity( subSystemEntityNode );
				if ( 0 == subSystemEntity )
				{
					cout << "ERROR ... failed creating instance for subsystem \"" << str << "\"" << endl;
					return false;
				}

				break;
			}
		}
		*/

		if ( 0 == subSystemEntity )
		{
			cout << "ERROR ... no according SubSystem for definition \"" << str << "\" found - object will be ignored" << endl;
			return false;
		}

		this->subSystemEntities.insert( make_pair( subSystemEntity->getType(), subSystemEntity ) );
	}

	return true;
}
