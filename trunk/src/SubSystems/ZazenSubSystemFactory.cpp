/*
 * SubSystemFactory.cpp
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#include "ZazenSubSystemFactory.h"

#include "AI/ZazenAI/ZazenAI.h"
#include "Audio/FMod/FModAudio.h"
#include "Graphics/ZazenGraphics/ZazenGraphics.h"
#include "Input/SDL/SDLInput.h"
#include "Physics/ODE/ODEPhysics.h"

#include <iostream>

using namespace std;

ZazenSubSystemFactory::ZazenSubSystemFactory()
{

}

ZazenSubSystemFactory::~ZazenSubSystemFactory()
{

}

ISubSystem*
ZazenSubSystemFactory::createSubSystem( const std::string& subSystemType )
{
	ISubSystem* subSystem = 0;

	if ( "ai" == subSystemType )
	{
		subSystem = new ZazenAI();
	}
	else if ( "audio" == subSystemType )
	{
		subSystem = new FModAudio();
	}
	else if ( "graphics" == subSystemType )
	{
		subSystem = new ZazenGraphics();
	}
	else if ( "input" == subSystemType )
	{
		subSystem = new SDLInput();
	}
	else if ( "physics" == subSystemType )
	{
		subSystem = new ODEPhysics();
	}
	else
	{
		cout << "Unknown SubSystem type \"" << subSystemType << "\" - exit" << endl;
	}

	return subSystem;
}
