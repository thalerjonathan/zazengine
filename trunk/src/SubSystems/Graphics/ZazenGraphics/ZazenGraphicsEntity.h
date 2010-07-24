/*
 * ZazenGraphicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ZAZENGRAPHICSENTITY_H_
#define ZAZENGRAPHICSENTITY_H_

#include "../../../Core/SubSystems/IFaces/IGraphicsEntity.h"
#include "../../../Core/SubSystems/IFaces/IConsumer.h"

class ZazenGraphicsEntity : public IGraphicsEntity, public IConsumer
{
	public:
		ZazenGraphicsEntity();
		virtual ~ZazenGraphicsEntity();

		const std::string& getType() const { return this->type; };

		void consume( ISubSystemEntity* );

	private:
		std::string type;
};

#endif /* ZAZENGRAPHICSENTITY_H_ */
