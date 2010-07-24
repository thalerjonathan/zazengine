/*
 * ZazenSubSystemFactory.h
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#ifndef ZAZENSUBSYSTEMFACTORY_H_
#define ZAZENSUBSYSTEMFACTORY_H_

#include "../IFaces/ISubSystemFactory.h"

class ZazenSubSystemFactory : public ISubSystemFactory
{
	public:
		ZazenSubSystemFactory();
		~ZazenSubSystemFactory();

		ISubSystem* createSubSystem( const std::string& );
};

#endif /* SUBSYSTEMFACTORY_H_ */
