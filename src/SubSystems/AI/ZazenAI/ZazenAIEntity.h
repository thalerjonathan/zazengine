/*
 * PlayGroundAIEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ZAZENAIENTITY_H_
#define ZAZENAIENTITY_H_

#include "../../../Core/SubSystems/IFaces/IAIEntity.h"

class ZazenAIEntity : public IAIEntity
{
	public:
		ZazenAIEntity() : type("ai") {};
		virtual ~ZazenAIEntity() {};

		const std::string& getType() const { return this->type; };

	private:
		std::string type;
};

#endif /* ZAZENAIENTITY_H_ */
