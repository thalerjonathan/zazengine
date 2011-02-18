/*
 * Core.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "Core.h"

#include "ScriptSystem/ScriptSystem.h"

#include "../GameObjects/ZazenGameObjectFactory.h"
#include "../SubSystems/ZazenSubSystemFactory.h"

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

		Core::instance->gameObjectFactory = new ZazenGameObjectFactory();
		Core::instance->subSystemFactory = new ZazenSubSystemFactory();

		Core::instance->eventManager = new EventManager();

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
		ScriptSystem::getInstance().callFunc( "onShutdown" );

		list<ISubSystem*>::iterator iter;

		iter = Core::instance->subSystems.begin();
		while ( iter != Core::instance->subSystems.end() )
		{
			ISubSystem* subSys = *iter++;
			subSys->shutdown();
			delete subSys;
		}

		if ( Core::instance->eventManager )
			delete Core::instance->eventManager;

		if ( Core::instance->gameObjectFactory )
			delete Core::instance->gameObjectFactory;

		if ( Core::instance->subSystemFactory )
			delete Core::instance->subSystemFactory;

		ScriptSystem::shutdown();

		delete Core::instance;
	}

	return true;
}

Core::Core()
{
	Core::instance = this;

	this->runCore = false;

	this->eventManager = 0;
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
		if ( false == subSys->start() )
		{
			cout << "Failed starting SubSystem \"" << subSys->getID() << " - exit" << endl;
			return;
		}
	}

	timeval t;
	double itFact = 0;
	long long startTicks = 0;
	long long endTicks = 0;
	
	cout << "Start processing" << endl;

	this->runCore = true;
	while ( this->runCore )
	{
		gettimeofday( &t, NULL );
		startTicks = t.tv_usec + 1000000 * t.tv_sec;
		
		this->eventManager->processQueue();

		if ( false == ScriptSystem::getInstance().callFunc( "beginFrame" ) )
		{
			this->runCore = false;
			break;
		}

		subSysIter = this->subSystems.begin();
		while ( subSysIter != this->subSystems.end() )
		{
			if ( false == (*subSysIter)->process( itFact ) )
			{
				this->runCore = false;
				break;
			}

			subSysIter++;
		}

		if ( false == ScriptSystem::getInstance().callFunc( "endFrame" ) )
		{
			this->runCore = false;
			break;
		}

		//sleep( 1 );

		subSysIter = this->subSystems.begin();
		while ( subSysIter != this->subSystems.end() )
		{
			(*subSysIter)->finalizeProcess();
			subSysIter++;
		}
		
		gettimeofday( &t, NULL );
		endTicks = t.tv_usec + 1000000 * t.tv_sec;
		
		itFact = (double)(endTicks - startTicks) / (double) 1000;
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

ISubSystem*
Core::getSubSystemByType( const std::string& type )
{
	list<ISubSystem*>::iterator iter = this->subSystems.begin();
	while ( iter != this->subSystems.end() )
	{
		ISubSystem* subSys = *iter++;
		if ( subSys->getType() == type )
		{
			return subSys;
		}
	}

	return 0;
}

bool
Core::loadConfig()
{
	string fullFileName = "media/config.xml";

	TiXmlDocument doc( fullFileName.c_str() );

	if ( false == doc.LoadFile() )
	{
		cout << "ERROR ... could not load file " << fullFileName << " - reason = " << doc.ErrorDesc() << " at row = " << doc.ErrorRow() << " col = " << doc.ErrorCol() << endl;
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

	TiXmlElement* objectListNode = rootNode->FirstChildElement("objectList");
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
			IGameObject* object = this->gameObjectFactory->createObject( "" );
			if ( object->initialize( objectNode ) )
				this->gameObjects.push_back( object );
			else
				delete object;
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

	TiXmlElement* subSystemNode = doc.FirstChildElement( "subSystem" );
	if ( 0 == subSystemNode )
	{
		cout << "ERROR ... no root-node \"subSystem\" defined in " << fileName << " - exit " << endl;
		return 0;
	}

	string subSystemType;
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

	ISubSystem* subSystem = this->subSystemFactory->createSubSystem( subSystemType );
	if ( 0 == subSystem )
	{
		return 0;
	}

	if ( false == subSystem->initialize( subSystemNode ) )
	{
		cout << "Initializing " << subSystemType << " Subsystem failed - exit" << endl;
		delete subSystem;
		return 0;
	}


	return subSystem;
}
