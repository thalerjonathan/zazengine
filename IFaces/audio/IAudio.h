/*
 * IAudio.h
 *
 *  Created on: 05.07.2010
 *      Author: Jonathan Thaler
 */

#ifndef IAUDIO_H_
#define IAUDIO_H_

#include "../core/ISubSystem.h"
#include "IAudioEntity.h"

class IAudio : public ISubSystem
{
	public:
		virtual ~IAudio() {};

		virtual IAudioEntity* createEntity( TiXmlElement*, IGameObject* parent ) = 0;

};

#endif /* IAUDIO_H_ */
