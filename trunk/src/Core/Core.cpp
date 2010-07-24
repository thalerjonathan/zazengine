/*
 * Core.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "Core.h"

#include "ScriptSystem/ScriptSystem.h"

#include "ObjectModel/Impl/GameObject.h"

#include "SubSystems/Impl/Ai/PlayGroundAI.h"
#include "SubSystems/Impl/Audio/PlayGroundAudio.h"
#include "SubSystems/Impl/Physics/PlayGroundPhysics.h"
#include "SubSystems/Impl/Graphics/PlayGroundGraphics.h"
#include "SubSystems/Impl/Input/PlayGroundInput.h"

#include <sys/time.h>

#include <iostream>
#include <sstream>

using namespace std;

Core* Core::instance = 0;

bool
Core::initalize()
{
	if ( 0 == Core::instance )
	{
		new Core();

		cout << "********************************************************" << endl;
		cout << "***************** Initializing Core... *****************" << endl;
		cout << "********************************************************" << endl;

		if ( false == ScriptSystem::initialize() )
		{
			cout << "ERROR ... failed initializing ScriptSystem - exit" << endl;
			return false;
		}

		if ( false == Core::instance->loadConfig() )
		{
			cout << "ERROR ... failed initializing Core - exit" << endl;
			return false;
		}

		Core::instance->subSysEventManager = new EventManager();
		Core::instance->objectsEventManager = new EventManager();

		cout << "********************************************************" << endl;
		cout << "************** Core successfully initialized ***********" << endl;
		cout << "********************************************************" << endl;
	}

	return true;
}

bool
Core::shutdown()
{
	if ( Core::instance )
	{
		list<ISubSystem*>::iterator iter;

		iter = Core::instance->subSystems.begin();
		while ( iter != Core::instance->subSystems.end() )
		{
			ISubSystem* subSys = *iter++;
			subSys->shutdown();
			delete subSys;
		}

		if ( Core::instance->subSysEventManager )
			delete Core::instance->subSysEventManager;

		if ( Core::instance->objectsEventManager )
			delete Core::instance->objectsEventManager;

		ScriptSystem::shutdown();

		delete Core::instance;
	}

	return true;
}

Core::Core()
{
	Core::instance = this;

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

		if ( false == ScriptSystem::getInstance().callFunc( "beginFrame" ) )
		{
			this->runCore = false;
			break;
		}

		subSysIter = this->subSystems.begin();
		while ( subSysIter != this->subSystems.end() )
		{
			(*subSysIter)->process( iterationFactor );
			subSysIter++;
		}

		if ( false == ScriptSystem::getInstance().callFunc( "endFrame" ) )
		{
			this->runCore = false;
			break;
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

	TiXmlDocument doc( fullFileName.c_str() );

	if ( false == doc.LoadFile() )
	{
		cout << "ERROR ... could not load file " << fullFileName << " - reason = " << doc.ErrorDesc() << endl;
		return false;
	}

	TiXmlElement* rootNode = doc.FirstChildElement("config");
	if ( 0 == rootNode )
	{
		cout << "ERROR ... root-node \"config\" in " << fullFileName << " not found" << endl;
		return false;
	}

	TiXmlElement* coreScriptNode = rootNode->FirstChildElement("coreScript");
	if ( 0 == coreScriptNode )
	{
		cout << "ERROR ... node \"coreScript\" in " << fullFileName << " not found" << endl;
		return false;
	}

	string coreScriptFileName = coreScriptNode->Attribute("file");

	if ( false == ScriptSystem::getInstance().loadFile( coreScriptFileName ) )
	{
		return false;
	}

	if ( false == ScriptSystem::getInstance().callFunc( "onStartup" ) )
	{
		return false;
	}

	TiXmlElement* subSystemListNode = rootNode->FirstChildElement("subSystemList");
	if ( 0 == subSystemListNode )
	{
		cout << "ERROR ... node \"subSystemList\" in " << fullFileName << " not found" << endl;
		return false;
	}

	for (TiXmlElement* subSystemNode = subSystemListNode->FirstChildElement(); subSystemNode != 0; subSystemNode = subSystemNode->NextSiblingElement())
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
					return false;
			}
		}
	}

	TiXmlElement* objectListNode = doc.FirstChildElement("objectList");
	if ( 0 == objectListNode )
	{
		cout << "ERROR ... node \"objectList\" in " << fullFileName << " not found" << endl;
		return false;
	}

	for (TiXmlElement* objectNode = objectListNode->FirstChildElement(); objectNode != 0; objectNode = objectNode->NextSiblingElement())
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

	TiXmlElement* subSystemNode = doc.FirstChildElement( "subSystem" );
	if ( 0 == subSystemNode )
	{
		cout << "ERROR ... no root-node \"subSystem\" defined in " << fileName << " - exit " << endl;
		return 0;
	}

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
		cout << "Unknown SubSystem type \"" << subSystemType << "\" - exit" << endl;
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

	if ( false == ScriptSystem::getInstance().loadFile( scriptFile ) )
	{
		return false;
	}

	return object;
}
