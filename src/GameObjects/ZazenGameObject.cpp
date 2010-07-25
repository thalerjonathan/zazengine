/*
 * ZazenGameObject.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "ZazenGameObject.h"

#include "../Core/Core.h"
#include "../Core/ScriptSystem/ScriptSystem.h"

#include <iostream>

using namespace std;

ZazenGameObject::GameObjectID ZazenGameObject::nextID = 0;

ZazenGameObject::ZazenGameObject( )
	: id ( ZazenGameObject::nextID++ )
{
}

ZazenGameObject::~ZazenGameObject()
{
}

bool
ZazenGameObject::sendEvent( const Event& e )
{
	//cout << "Processing event " << e.id << " in EventProcessor " << this->id << endl;

	return true;
}

bool
ZazenGameObject::initialize( TiXmlElement* objectNode )
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

		ISubSystem* subSystem = Core::getInstance().getSubSystemByType( str );
		if ( 0 == subSystem )
		{
			cout << "ERROR ... no according SubSystem for definition \"" << str << "\" found - object will be ignored" << endl;
			return false;
		}
		else
		{
			ISubSystemEntity* subSystemEntity = subSystem->createEntity( subSystemEntityNode );
			if ( 0 == subSystemEntity )
			{
				cout << "ERROR ... failed creating instance for subsystem \"" << str << "\"" << endl;
				return false;
			}

			this->subSystemEntities.insert( make_pair( subSystemEntity->getType(), subSystemEntity ) );
		}
	}

	map<string, ISubSystemEntity*>::iterator iter = this->subSystemEntities.begin();
	while ( iter != this->subSystemEntities.end() )
	{
		ISubSystemEntity* entity = iter->second;
		iter++;

		vector<string> dependencies = entity->getDependencies();
		for ( unsigned int i = 0; i < dependencies.size(); i++ )
		{
			map<string, ISubSystemEntity*>::iterator findIter = this->subSystemEntities.find( dependencies[i] );
			if ( findIter != this->subSystemEntities.end() )
			{
				entity->addConsumer( findIter->second );
			}
			else
			{
				cout << "ERROR ... failed resolving dependency \"" << dependencies[i] << "\"" << endl;
				return false;
			}
		}
	}

	return true;
}
