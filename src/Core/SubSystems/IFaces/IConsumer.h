/*
 * IConsumer.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ICONSUMER_H_
#define ICONSUMER_H_

class ISubSystemEntity;

class IConsumer
{
	public:
		virtual ~IConsumer() {};

		virtual void consume(ISubSystemEntity*) = 0;

};
#endif /* ICONSUMER_H_ */
