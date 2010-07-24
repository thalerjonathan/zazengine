/*
 *  PlayGroundInput.cpp
 *  PlayGround
 *
 *  Created by Jonathan Thaler on 17.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayGroundInput.h"

#include "../../../Core.h"

#include <iostream>

#include <SDL/SDL.h>

using namespace std;

PlayGroundInput::PlayGroundInput()
	: id ( "PlayGroundInput" ),
	type ("input")
{
	
}

PlayGroundInput::~PlayGroundInput()
{
	
}

bool
PlayGroundInput::initialize()
{
	cout << endl << "=============== PlayGroundInput initializing... ===============" << endl;

	cout << "================ PlayGroundInput initialized =================" << endl;

	return true;
}

bool
PlayGroundInput::shutdown()
{
	cout << endl << "=============== PlayGroundInput shutting down... ===============" << endl;

	cout << "================ PlayGroundInput shutdown =================" << endl;

	return true;
}

bool
PlayGroundInput::start()
{
	return true;
}

bool
PlayGroundInput::stop()
{
	return true;
}

bool
PlayGroundInput::pause()
{
	return true;
}

bool
PlayGroundInput::process(double factor)
{
	//cout << "PlayGroundInput::process" << endl;

	SDL_Event event;
	
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			cout << "keydown: " << event.key.keysym.sym << endl;

			if (event.key.keysym.sym == SDLK_q)
				Core::getInstance().stop();
			else
				pressedKeys.push_back(event.key.keysym.sym);
			
		} else if (event.type == SDL_KEYUP) {
			pressedKeys.remove(event.key.keysym.sym);
			
		} else if (event.type == SDL_MOUSEMOTION) {
			//this->camera->changeHeading(event.motion.xrel * loopFactor * 0.01);
			//this->camera->changePitch(event.motion.yrel * loopFactor * 0.01);
			
		} else if (event.type == SDL_QUIT) {
			Core::getInstance().stop();
		}
	}
	
	list<int>::iterator pressedKeysIter = pressedKeys.begin();
	while (pressedKeysIter != pressedKeys.end()) {
		int key = *pressedKeysIter++;
		
		switch(key){
			case SDLK_RIGHT:
				//this->camera->changeHeading(-1 * loopFactor);
				break;
				
			case SDLK_LEFT:
				//this->camera->changeHeading(1 * loopFactor);
				break;
				
			case SDLK_UP:
				//this->camera->changePitch(-1 * loopFactor);
				break;
				
			case SDLK_DOWN:
				//this->camera->changePitch(1 * loopFactor);
				break;
				
			case SDLK_w:
				//this->camera->strafeForward(5 * loopFactor);
				break;
				
			case SDLK_s:
				//this->camera->strafeForward(-5 * loopFactor);
				break;
				
			case SDLK_d:
				//this->camera->changeRoll(-1 * loopFactor);
				break;
				
			case SDLK_a:
				//this->camera->changeRoll(1 * loopFactor);
				break;
				
			default:
				break;
		}
	}
	
	return true;
}

bool
PlayGroundInput::finalizeProcess()
{
	//cout << "PlayGroundPhysics::finalizeProcess" << endl;

	return true;
}

bool
PlayGroundInput::sendEvent(const Event& e)
{
	return true;
}

ISubSystemEntity*
PlayGroundInput::createEntity( TiXmlElement* cfgNode )
{
	return 0;
}
