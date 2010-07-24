/*
 * ISubSystemFactory.h
 *
 *  Created on: 24.07.2010
 *      Author: jonathan
 */

#ifndef ISUBSYSTEMFACTORY_H_
#define ISUBSYSTEMFACTORY_H_

#include "ISubSystem.h"

class ISubSystemFactory
{
	public:
		virtual ISubSystem* createSubSystem( const std::string& type ) = 0;
};

#endif /* ISUBSYSTEMFACTORY_H_ */
