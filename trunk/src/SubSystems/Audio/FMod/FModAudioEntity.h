/*
 * FModAudioEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef FMODAUDIOENTITY_H_
#define FMODAUDIOENTITY_H_

#include "../../../Core/SubSystems/IFaces/IAudioEntity.h"

#include <fmod/fmod.hpp>

class FModAudioEntity : public IAudioEntity
{
	public:
		friend class FModAudio;

		FModAudioEntity( IGameObject* p );
		virtual ~FModAudioEntity();

		const std::string& getType() const { return this->type; };

		virtual bool sendEvent(const Event& e);

		/*
		std::vector<std::string> getDependencies() const;

		void consume( ISubSystemEntity* );
		*/

	private:
		std::string type;

		FMOD::Sound* sound;
		FMOD::Channel* channel;
};

#endif /* FMODAUDIOENTITY_H_ */
