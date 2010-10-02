/*
 * Value.h
 *
 *  Created on: 07.07.2010
 *      Author: joni
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <string>

class Value
{
	public:
		Value() {};
		Value( const float* data ) : data( data ) {};
		Value( const Value& v ) : data( v.data ) {};
		~Value() {};

		const float* data;
/*
		enum ValueType {
			TYPE_INT,
			TYPE_LONG,
			TYPE_FLOAT,
			TYPE_DOUBLE,
			TYPE_BOOL,
			TYPE_STRING,
			TYPE_DATA,
			TYPE_COUNT	// NUMBER OF UNIQUE TYPES
		};

		const ValueType getType() const { return this->type; };

		int getInt() const { return this->intValue; };
		long getLong() const { return this->longValue; };
		float getFloat() const { return this->floatValue; };
		double getDouble() const { return this->doubleValue; };
		bool getBool() const { return this->boolValue; };
		const std::string& getString() const { return this->stringValue; };
		//T* getTypedPtr() {};
*/

	private:
		/*
		ValueType type;

		union {
			int intValue;
			float floatValue;
			double doubleValue;
			long longValue;
			bool boolValue;
			char* dataValue;
		};


		std::string stringValue;
		void* data;
		*/
};

#endif /* VALUE_H_ */
