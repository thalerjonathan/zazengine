/*
 * PlayGround.h
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#ifndef CORE_H_
#define CORE_H_

#include "ObjectModel/IFaces/IGameObjectFactory.h"
#include "SubSystems/IFaces/ISubSystemFactory.h"

#include "EventSystem/EventManager.h"

#include "Utils/XML/tinyxml.h"

#include <list>

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

		void start();
		void stop();

		ISubSystem* getSubSystemByType( const std::string& );

		EventManager& getSubSysEventManager() const { return *this->subSysEventManager; };
		EventManager& getObjectsEventManager() const { return *this->objectsEventManager; };

	private:
		static Core* instance;

		bool runCore;

		EventManager* subSysEventManager;
		EventManager* objectsEventManager;

		IGameObjectFactory* gameObjectFactory;
		ISubSystemFactory* subSystemFactory;

		std::list<ISubSystem*> subSystems;
		std::list<IGameObject*> gameObjects;

		Core();
		~Core();

		bool loadConfig();
		ISubSystem* loadSubSystem( const std::string& );
};

#endif /* CORE_H_ */
