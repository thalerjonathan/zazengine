/*
 * PlaygroundAI.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "PlayGroundAI.h"

#include <iostream>

using namespace std;

PlayGroundAI::PlayGroundAI()
	: id ( "PlayGroundAI" ),
	type ("ai")
{

}

PlayGroundAI::~PlayGroundAI()
{

}

bool
PlayGroundAI::initialize()
{
	cout << endl << "=============== PlayGroundAI initializing... ===============" << endl;

	cout << "================ PlayGroundAI initialized =================" << endl;

	return true;
}

bool
PlayGroundAI::shutdown()
{
	cout << endl << "=============== PlayGroundAI shutting down... ===============" << endl;

	cout << "================ PlayGroundAI shutdown =================" << endl;

	return true;
}

bool
PlayGroundAI::start()
{
	return true;
}

bool
PlayGroundAI::stop()
{
	return true;
}

bool
PlayGroundAI::pause()
{
	return true;
}

bool
PlayGroundAI::process(double factor)
{
	//cout << "PlayGroundAI::process()" << endl;

	return true;
}

bool
PlayGroundAI::finalizeProcess()
{
	//cout << "PlayGroundAI::finalizeProcess()" << endl;

	return true;
}

bool
PlayGroundAI::sendEvent(const Event& e)
{
	return true;
}

PlayGroundAIEntity*
PlayGroundAI::createEntity( TiXmlElement* node )
{
	PlayGroundAIEntity* entity = new PlayGroundAIEntity();

	return entity;
}
