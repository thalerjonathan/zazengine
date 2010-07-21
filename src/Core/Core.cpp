/*
 * Core.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "Core.h"

#include "ObjectModel/Impl/GameObject.h"

#include "SubSystems/Impl/Ai/PlayGroundAI.h"
#include "SubSystems/Impl/Audio/PlayGroundAudio.h"
#include "SubSystems/Impl/Physics/PlayGroundPhysics.h"
#include "SubSystems/Impl/Graphics/PlayGroundGraphics.h"
#include "SubSystems/Impl/Input/PlayGroundInput.h"

extern "C"
{
	#include <lua/lauxlib.h>
}

#include <luabind/lua_include.hpp>

#include <sys/time.h>

#include <iostream>
#include <sstream>

using namespace std;

Core* Core::instance = 0;

bool
Core::initalize()
{
	if (Core::instance == 0)
		new Core();

	cout << "Initializing Core..." << endl;

	Core::instance->globalLuaState = luaL_newstate();

	if ( false == Core::instance->loadConfig() )
	{
		return false;
	}

	Core::instance->subSysEventManager = new EventManager();
	Core::instance->objectsEventManager = new EventManager();

	return true;
}

bool
Core::shutdown()
{
	list<ISubSystem*>::iterator iter;

	iter = Core::instance->subSystems.begin();
	while ( iter != Core::instance->subSystems.end() )
	{
		ISubSystem* subSys = *iter++;
		subSys->shutdown();
		delete subSys;
	}

	delete Core::instance->subSysEventManager;
	delete Core::instance->objectsEventManager;

	lua_close( Core::instance->globalLuaState );

	if (Core::instance != 0)
		delete Core::instance;

	return true;
}

Core::Core()
{
	Core::instance = this;

	this->globalLuaState = 0;

	this->runCore = false;

	this->subSysEventManager = 0;
	this->objectsEventManager = 0;

	this->ai = 0;
	this->audio = 0;
	this->physics = 0;
	this->graphics = 0;
	this->input = 0;
}

Core::~Core()
{
	Core::instance = 0;
}

void
Core::start()
{
	list<ISubSystem*>::iterator subSysIter = this->subSystems.begin();
	while ( subSysIter != this->subSystems.end() )
	{
		ISubSystem* subSys = *subSysIter++;
		subSys->start();
	}

	timeval t;
	double iterationFactor = 0;
	long long startTicks = 0;
	long long endTicks = 0;
	
	cout << "Start processing" << endl;

	this->runCore = true;
	while ( this->runCore )
	{
		gettimeofday( &t, NULL );
		startTicks = t.tv_usec + 1000000 * t.tv_sec;
		
		this->subSysEventManager->processQueue();
		this->objectsEventManager->processQueue();

		subSysIter = this->subSystems.begin();
		while ( subSysIter != this->subSystems.end() )
		{
			(*subSysIter)->process( iterationFactor );
			subSysIter++;
		}

		subSysIter = this->subSystems.begin();
		while ( subSysIter != this->subSystems.end() )
		{
			(*subSysIter)->finalizeProcess();
			subSysIter++;
		}
		
		gettimeofday( &t, NULL );
		endTicks = t.tv_usec + 1000000 * t.tv_sec;
		
		iterationFactor = (double)(endTicks - startTicks) / (double) 1000;
	};

	subSysIter = this->subSystems.begin();
	while ( subSysIter != this->subSystems.end() )
	{
		(*subSysIter)->stop();
		subSysIter++;
	}
}

void
Core::stop()
{
	this->runCore = false;
}

bool
Core::loadConfig()
{
	string fullFileName = "media/config.xml";

	TiXmlDocument doc(fullFileName.c_str());

	if ( false == doc.LoadFile() )
	{
		cout << "ERROR ... could not load file " << fullFileName << " - reason = " << doc.ErrorDesc() << endl;
		return false;
	}

	TiXmlElement* coreScriptNode = doc.FirstChildElement("coreScript");
	if ( 0 == coreScriptNode )
	{
		cout << "ERROR ... root-node \"coreScript\" in " << fullFileName << " not found" << endl;
		return false;
	}

	string coreScriptFileName = coreScriptNode->Attribute("file");

	// TODO: load script

	TiXmlElement* subSystemListNode = doc.FirstChildElement("subSystemList");

	for (TiXmlElement* subSystemNode = subSystemListNode->FirstChildElement(); subSystemNode != 0; subSystemNode = subSystemListNode->NextSiblingElement())
	{
		const char* str = subSystemNode->Value();
		if (str == 0)
			continue;

		if ( 0 == strcmp(str, "subSystem") )
		{
			str = subSystemNode->Attribute( "file" );
			if ( 0 == str )
			{
				cout << "No file defined for subSystem - will be ignored" << endl;
				continue;
			}
			else
			{
				ISubSystem* subSystem = this->loadSubSystem( str );
				if ( subSystem )
					this->subSystems.push_back( subSystem );
				else
					return 0;
			}
		}
	}

	TiXmlElement* objectListNode = doc.FirstChildElement("objectList");

	for (TiXmlElement* objectNode = objectListNode->FirstChildElement(); objectNode != 0; objectNode = objectListNode->NextSiblingElement())
	{
		const char* str = objectNode->Value();
		if ( 0 == str )
			continue;

		if ( 0 == strcmp( str, "object" ) )
		{
			IGameObject* object = this->loadObject( objectNode );
			if ( object )
				this->gameObjects.push_back( object );
		}
	}

	return true;
}

ISubSystem*
Core::loadSubSystem( const std::string& fileName )
{
	TiXmlDocument doc( fileName.c_str() );

	if ( false == doc.LoadFile() )
	{
		cout << "ERROR ... could not load file " << fileName << " - reason = " << doc.ErrorDesc() << endl;
		return 0;
	}

	string subSystemType;
	TiXmlElement* subSystemNode = doc.FirstChildElement("subSystem");

	const char* str = subSystemNode->Attribute( "type" );
	if ( 0 == str )
	{
		cout << "No type defined for subSystem" << endl;
		return 0;
	}
	else
	{
		subSystemType = str;
	}

	ISubSystem* subSystem = 0;

	if ( "AI" == subSystemType )
	{
		subSystem = new PlayGroundAI();
		if ( false == subSystem->initialize( ) )
		{
			delete subSystem;
			return 0;
		}
		else
		{
			Core::instance->ai = (IAi*) subSystem;
		}
	}
	else if ( "AUDIO" == subSystemType )
	{
		subSystem = new PlayGroundAudio();
		if ( false == subSystem->initialize( ) )
		{
			cout << "Initializing " << subSystemType << " Subsystem failed - exit" << endl;
			delete subSystem;
			return 0;
		}
		else
		{
			Core::instance->audio = (IAudio*) subSystem;
		}
	}
	else if ( "GRAPHICS" == subSystemType )
	{
		subSystem = new PlayGroundGraphics();
		if ( false == subSystem->initialize( ) )
		{
			cout << "Initializing " << subSystemType << " Subsystem failed - exit" << endl;
			delete subSystem;
			return 0;
		}
		else
		{
			Core::instance->graphics = (IGraphics*) subSystem;
		}
	}
	else if ( "INPUT" == subSystemType )
	{
		subSystem = new PlayGroundInput();
		if ( false == subSystem->initialize( ) )
		{
			cout << "Initializing " << subSystemType << " Subsystem failed - exit" << endl;
			delete subSystem;
			return 0;
		}
		else
		{
			Core::instance->input = (IInput*) subSystem;
		}
	}
	else if ( "PHYSICS" == subSystemType )
	{
		subSystem = new PlayGroundPhysics();
		if ( false == subSystem->initialize( ) )
		{
			cout << "Initializing " << subSystemType << " Subsystem failed - exit" << endl;
			delete subSystem;
			return 0;
		}
		else
		{
			Core::instance->physics = (IPhysics*) subSystem;
		}
	}
	else
	{
		cout << "Unknown SubSystem type \"" << subSystemType << "\"- will be ignored" << endl;
	}

	return subSystem;
}

IGameObject*
Core::loadObject( TiXmlElement* objectNode )
{
	string objectID;
	string scriptFile;

	const char* str = objectNode->Attribute( "id" );
	if ( 0 == str )
	{
		cout << "WARNING: no id defined for object - will be ignored" << endl;
		return 0;
	}
	else
	{
		objectID = str;
	}

	str = objectNode->Attribute( "script" );
	if ( 0 == str )
	{
		cout << "INFO: no script defined for object " << objectID << endl;
		return 0;
	}
	else
	{
		scriptFile = str;
	}

	IGameObject* object = new GameObject( objectID );
	// TODO: load script

	return object;
}
