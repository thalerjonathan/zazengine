/*
 * Event.h
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Value.h"

#include <map>
#include <string>

class IEventListener;

typedef std::string EventID;
//typedef long long EventID;

class Event
{
	public:
		Event() { this->id = ""; this->target = 0; this->source = 0; this->startTimestamp = 0; };
		Event(const Event& e) { this->id = e.id; this->source = e.source; this->target = e.target; this->startTimestamp = e.startTimestamp; };
		~Event() {};

		EventID id;
		IEventListener* source;
		IEventListener* target;

		long int startTimestamp;

		void addValue( const Value& );
		Value getValue( const std::string& id );

	private:
		std::map<std::string, Value> values;
};

#endif /* EVENT_H_ */
