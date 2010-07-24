/*
 * IGameObjectFactory.h
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#ifndef IGAMEOBJECTFACTORY_H_
#define IGAMEOBJECTFACTORY_H_

#include "IGameObject.h"

class IGameObjectFactory
{
	public:
		virtual IGameObject* createObject( const std::string& type ) = 0;
};

#endif /* IGAMEOBJECTFACTORY_H_ */
