/*
 *  PlayGroundInput.h
 *  PlayGround
 *
 *  Created by Jonathan Thaler on 17.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLAYGROUNDINPUT_H_
#define PLAYGROUNDINPUT_H_

#include "../../IFaces/IInput.h"

#include <list>

class PlayGroundInput : public IInput
{
	public:
		PlayGroundInput();
		virtual ~PlayGroundInput();
		
		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };
		
		bool isAsync() const { return false; };
		
		bool initialize();
		bool shutdown();
		
		bool start();
		bool stop();
		bool pause();
		
		bool process(double);
		bool finalizeProcess();
		
		bool sendEvent(const Event&);
		
		ISubSystemEntity* createEntity( TiXmlElement* );
		
	private:
		std::string id;
		std::string type;
	
		std::list<int> pressedKeys;
		
};

#endif /* PLAYGROUNDINPUT_H_ */
