/*
 * PlayGround.h
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#ifndef CORE_H_
#define CORE_H_

#include "ObjectModel/IFaces/IGameObject.h"
#include "EventSystem/EventManager.h"

#include "SubSystems/IFaces/IAi.h"
#include "SubSystems/IFaces/IAudio.h"
#include "SubSystems/IFaces/IPhysics.h"
#include "SubSystems/IFaces/IGraphics.h"
#include "SubSystems/IFaces/IInput.h"

#include <map>
#include <vector>

#include "Utils/XML/tinyxml.h"

extern "C"
{
	#include <lua/lua.h>
}

/* BIG TARGET: Discrete Realtime Autonomus Entity Simulation
 *
 * Next Step: integrate LUA
 *
 */
class Core
{
	public:
		static bool initalize();
		static bool shutdown();
		static Core& getInstance() { return *Core::instance; };

		IAi* getAI() const { return this->ai; };
		IGraphics* getGraphics() const { return this->graphics; };
		IAudio* getAudio() const { return this->audio; };
		IPhysics* getPhysics() const { return this->physics; };
		IInput* getInput() const { return this->input; };

		EventManager& getSubSysEventManager() const { return *this->subSysEventManager; };
		EventManager& getObjectsEventManager() const { return *this->objectsEventManager; };

		void start();
		void stop();

	private:
		static Core* instance;

		lua_State* globalLuaState;

		bool runCore;

		IAi* ai;
		IGraphics* graphics;
		IAudio* audio;
		IPhysics* physics;
		IInput* input;

		EventManager* subSysEventManager;
		EventManager* objectsEventManager;

		std::list<ISubSystem*> subSystems;
		std::list<IGameObject*> gameObjects;

		Core();
		~Core();

		bool loadConfig();
		ISubSystem* loadSubSystem( const std::string& );
		IGameObject* loadObject( TiXmlElement* );
};

#endif /* CORE_H_ */
