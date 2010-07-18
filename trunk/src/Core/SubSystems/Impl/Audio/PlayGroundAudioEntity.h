/*
 * PlayGroundAudioEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDAUDIOENTITY_H_
#define PLAYGROUNDAUDIOENTITY_H_

#include "../../IFaces/IAudioEntity.h"
#include "../../IFaces/IConsumer.h"

class PlayGroundAudioEntity : public IAudioEntity, public IConsumer
{
	public:
		PlayGroundAudioEntity( );
		virtual ~PlayGroundAudioEntity();

		const std::string& getType() const { return this->type; };

		void consume( ISubSystemEntity* );

	private:
		std::string type;
};

#endif /* PLAYGROUNDAUDIOENTITY_H_ */
