/*
 * Core.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "Core.h"

#include "ObjectModel/IFaces/IGameObject.h"

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
	if (Core::instance == 0)
		new Core();

	cout << "Initializing Core..." << endl;

	Core::instance->subSysEventManager = new EventManager();
	Core::instance->objectsEventManager = new EventManager();

	Core::instance->ai = new PlayGroundAI();
	Core::instance->audio = new PlayGroundAudio();
	Core::instance->physics = new PlayGroundPhysics();
	Core::instance->graphics = new PlayGroundGraphics();
	Core::instance->input = new PlayGroundInput();

	if ( false == Core::instance->ai->initialize( ) )
	{
		return false;
	}

	if ( false == Core::instance->audio->initialize( ) )
	{
		return false;
	}

	if ( false == Core::instance->physics->initialize( ) )
	{
		return false;
	}

	if ( false == Core::instance->graphics->initialize( ) )
	{
		return false;
	}

	if ( false == Core::instance->input->initialize( ) )
	{
		return false;
	}


	// put physics first because its async and can start while others can sync work
	//Core::instance->subSystems.push_back( Core::instance->physics );
	Core::instance->subSystems.push_back( Core::instance->ai );
	Core::instance->subSystems.push_back( Core::instance->audio );
	Core::instance->subSystems.push_back( Core::instance->graphics );
	Core::instance->subSystems.push_back( Core::instance->input );

	// TODO: create gameobjects here

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

	if (Core::instance != 0)
		delete Core::instance;

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
