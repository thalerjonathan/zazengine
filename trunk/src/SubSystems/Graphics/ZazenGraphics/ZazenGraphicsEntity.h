/*
 * ZazenGraphicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ZAZENGRAPHICSENTITY_H_
#define ZAZENGRAPHICSENTITY_H_

#include "../../../Core/SubSystems/IFaces/IGraphicsEntity.h"

#include "Renderer/Scene.h"

class ZazenGraphicsEntity : public IGraphicsEntity
{
	public:
		friend class ZazenGraphics;

		ZazenGraphicsEntity();
		virtual ~ZazenGraphicsEntity();

		const std::string& getType() const { return this->type; };

		std::vector<std::string> getDependencies() const;

		void consume( ISubSystemEntity* );

	private:
		std::string type;

		Scene::EntityInstance* instance;
};

#endif /* ZAZENGRAPHICSENTITY_H_ */
