/*
 * SubSystemFactory.cpp
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#include "ZazenSubSystemFactory.h"

#include "Ai/PlayGroundAI.h"
#include "Audio/PlayGroundAudio.h"
#include "Graphics/PlayGroundGraphics.h"
#include "Input/PlayGroundInput.h"
#include "Physics/PlayGroundPhysics.h"

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
		subSystem = new PlayGroundAI();
	}
	else if ( "audio" == subSystemType )
	{
		subSystem = new PlayGroundAudio();
	}
	else if ( "graphics" == subSystemType )
	{
		subSystem = new PlayGroundGraphics();
	}
	else if ( "input" == subSystemType )
	{
		subSystem = new PlayGroundInput();
	}
	else if ( "physics" == subSystemType )
	{
		subSystem = new PlayGroundPhysics();
	}
	else
	{
		cout << "Unknown SubSystem type \"" << subSystemType << "\" - exit" << endl;
	}

	return subSystem;
}
