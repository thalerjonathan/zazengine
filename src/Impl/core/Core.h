#ifndef _CORE_H_
#define _CORE_H_

#include <core/ICore.h>
#include <core/IGameObjectFactory.h>
#include <core/ILogger.h>

#include "ZazenSubSystemFactory.h"

#include "EventManager.h"

#include "DllExport.h"

#include <list>
#include <map>

class DLL_API Core : public ICore
{
	public:
		static bool initalize( const std::string&, IGameObjectFactory* );
		static bool shutdown();
		static ICore* getInstance() { return Core::instance; };
		static Core& getRef() { return *Core::instance; };

		void start();
		void stop();

		float getProcessingFactor() const { return this->m_processingFactor; };

		long long getCurrentMillis() const;

		ILogger& getCoreLogger() { return *this->m_logger; };
		ILogger* getLogger( const std::string& );

		IAudio* getAudio() { return this->m_audio; };
		IGraphics* getGraphics() { return this->m_graphics; };
		IInput* getInput() { return this->m_input; };
		IPhysics* getPhysics() { return this->m_physics; };

		ISubSystem* getSubSystemByID( const std::string& );
		ISubSystem* getSubSystemByType( const std::string& );

		IGameObject* getObjectByName( const std::string& );

		IEventManager& getEventManager() const { return *this->m_eventManager; };

	private:
		static Core* instance;

		float m_processingFactor;

		bool m_runCore;

		EventManager* m_eventManager;

		IAudio* m_audio;
		IGraphics* m_graphics;
		IInput* m_input;
		IPhysics* m_physics;

		IGameObjectFactory* m_gameObjectFactory;
		ZazenSubSystemFactory* m_subSystemFactory;

		std::list<ISubSystem*> m_subSystems;
		std::list<IGameObject*> m_gameObjects;

		ILogger* m_logger;
		std::map<std::string, ILogger*> m_loggers;

		Core();
		virtual ~Core();

		bool configLogging( const std::string& );
		bool loadConfig( const std::string& );
		ISubSystem* loadSubSystem( const std::string&, const std::string& );
		bool checkSubSystemType( ISubSystem* ); 
};

#endif /* _CORE_H_ */
