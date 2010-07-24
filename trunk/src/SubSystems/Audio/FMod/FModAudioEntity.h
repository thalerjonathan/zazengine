/*
 * FModAudioEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef FMODAUDIOENTITY_H_
#define FMODAUDIOENTITY_H_

#include "../../../Core/SubSystems/IFaces/IAudioEntity.h"
#include "../../../Core/SubSystems/IFaces/IConsumer.h"

class FModAudioEntity : public IAudioEntity, public IConsumer
{
	public:
		FModAudioEntity( );
		virtual ~FModAudioEntity();

		const std::string& getType() const { return this->type; };

		void consume( ISubSystemEntity* );

	private:
		std::string type;
};

#endif /* FMODAUDIOENTITY_H_ */
