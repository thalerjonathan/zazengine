/*
 * ZazenGameObjectFactory.h
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#ifndef ZAZENGAMEOBJECTFACTORY_H_
#define ZAZENGAMEOBJECTFACTORY_H_

#include "../Core/ObjectModel/IFaces/IGameObjectFactory.h"

class ZazenGameObjectFactory : public IGameObjectFactory
{
	public:
		ZazenGameObjectFactory();
		~ZazenGameObjectFactory();

		IGameObject* createObject( const std::string& );
};

#endif /* ZAZENGAMEOBJECTFACTORY_H_ */
