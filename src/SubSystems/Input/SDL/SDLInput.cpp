/*
 *  SDLInput.cpp
 *  PlayGround
 *
 *  Created by Jonathan Thaler on 17.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "SDLInput.h"

#include "../../../Core/Core.h"

#include <iostream>

#include <SDL/SDL.h>

using namespace std;

SDLInput::SDLInput()
	: id ( "SDLInput" ),
	type ("input")
{
	
}

SDLInput::~SDLInput()
{
	
}

bool
SDLInput::initialize( TiXmlElement* )
{
	cout << endl << "=============== SDLInput initializing... ===============" << endl;

	cout << "================ SDLInput initialized =================" << endl;

	return true;
}

bool
SDLInput::shutdown()
{
	cout << endl << "=============== SDLInput shutting down... ===============" << endl;

	cout << "================ SDLInput shutdown =================" << endl;

	return true;
}

bool
SDLInput::start()
{
	return true;
}

bool
SDLInput::stop()
{
	return true;
}

bool
SDLInput::pause()
{
	return true;
}

bool
SDLInput::process(double factor)
{
	//cout << "SDLInput::process" << endl;

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
SDLInput::finalizeProcess()
{
	//cout << "PlayGroundPhysics::finalizeProcess" << endl;

	return true;
}

bool
SDLInput::sendEvent(const Event& e)
{
	return true;
}

ISubSystemEntity*
SDLInput::createEntity( TiXmlElement* cfgNode )
{
	return 0;
}
