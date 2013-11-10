#ifndef _FMODAUDIO_H_
#define _FMODAUDIO_H_

#include <core/ILogger.h>
#include <audio/IAudio.h>

#include "FModAudioEntity.h"

#include <boost/filesystem.hpp>

#include <fmodex/fmod.hpp>

#include <list>

class FModAudio : public IAudio
{
	public:
		static FModAudio& getInstance() { return *FModAudio::instance; };

		FModAudio( const std::string&, ICore* );
		virtual ~FModAudio();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return false; };

		bool initialize( TiXmlElement* );
		bool shutdown();

		bool start();
		bool stop();
		bool pause();

		bool process( double );
		bool finalizeProcess();

		bool sendEvent( Event& e );

		ILogger& getLogger() const { return *this->m_logger; };

		FModAudioEntity* createEntity( TiXmlElement*, IGameObject* parent );

		FMOD::System* getSystem() { return this->m_system; };

	private:
		static FModAudio* instance;

		std::string id;
		std::string type;

		boost::filesystem::path m_audioDataPath;

		ICore* m_core;
		ILogger* m_logger;

		FMOD::System* m_system;
		FMOD::Sound* m_bgMusic;
		FMOD::Channel* m_bgMusicCh;

		std::list<FModAudioEntity*> entities;

		bool initFMod( TiXmlElement* );
		bool initAudioDataPath( TiXmlElement* );
		bool init3dSettings( TiXmlElement* );
		bool loadBackgroundMusic( TiXmlElement* );
};

#endif /* _FMODAUDIO_H_ */
