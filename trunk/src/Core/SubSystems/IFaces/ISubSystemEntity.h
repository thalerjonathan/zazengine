/*
 * ISubSystemEntity.h
 *
 *  Created on: 07.07.2010
 *      Author: joni
 */

#ifndef ISUBSYSTEMENTITY_H_
#define ISUBSYSTEMENTITY_H_

#include <string>
#include <vector>

#include "IConsumer.h"

#include "../../ObjectModel/IFaces/IGameObject.h"

class ISubSystemEntity
{
	public:
		virtual ~ISubSystemEntity() {};

		virtual const std::string& getType() const = 0;

		void addConsumer(IConsumer* c) { this->consumers.push_back( c ); };

		void processConsumers()
		{
			for ( unsigned int i = 0; i < consumers.size(); i++)
			{
				consumers[i]->consume( this );
			}
		}

	private:
		std::vector<IConsumer*> consumers;
};

#endif /* ISUBSYSTEMENTITY_H_ */
