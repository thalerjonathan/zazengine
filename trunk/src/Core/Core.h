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
 * TODO: clear, powerful and flexible event-system
 * TODO: solve how input moves around
 * TODO: integrate event-system with LUA-scripting
 *
 * TODO: client-server model for world
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

		EventManager& getEventManager() const { return *this->m_eventManager; };

	private:
		static Core* instance;

		bool m_runCore;

		EventManager* m_eventManager;

		IGameObjectFactory* m_gameObjectFactory;
		ISubSystemFactory* m_subSystemFactory;

		std::list<ISubSystem*> m_subSystems;
		std::list<IGameObject*> m_gameObjects;

		Core();
		~Core();

		IGameObject* getObjectByName( const std::string& );

		bool loadConfig();
		ISubSystem* loadSubSystem( const std::string& );
};

#endif /* CORE_H_ */
