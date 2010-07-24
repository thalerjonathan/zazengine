/*
 * PlaygroundAudio.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDAUDIO_H_
#define PLAYGROUNDAUDIO_H_

#include "../../IFaces/IAudio.h"

#include "PlayGroundAudioEntity.h"

#include <AL/alut.h>
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>

class PlayGroundAudio : public IAudio
{
	public:
		PlayGroundAudio();
		virtual ~PlayGroundAudio();

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

		bool sendEvent( const Event& e );

		PlayGroundAudioEntity* createEntity( TiXmlElement* );

	private:
		std::string id;
		std::string type;

		FMOD::System* system;
		FMOD::Sound* bgMusic;
		FMOD::Channel* bgMusicCh;

};

#endif /* PLAYGROUNDAUDIO_H_ */
