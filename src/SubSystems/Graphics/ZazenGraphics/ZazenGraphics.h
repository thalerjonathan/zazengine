/*
 * ZazenGraphics.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef ZAZENGRAPHICS_H_
#define ZAZENGRAPHICS_H_

#include "../../../Core/SubSystems/IFaces/IGraphics.h"

#include "ZazenGraphicsEntity.h"

#include "Scene/Scene.h"
#include "Scene/Viewer.h"

#include <SDL/SDL.h>

class ZazenGraphics : public IGraphics
{
	public:
		ZazenGraphics();
		virtual ~ZazenGraphics();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return false; };

		bool initialize( TiXmlElement* );
		bool shutdown();

		bool start();
		bool stop();
		bool pause();

		bool process( double );
		bool finalizeProcess();

		bool sendEvent( Event& e );

		ZazenGraphicsEntity* createEntity( TiXmlElement*, IGameObject* parent );

	private:
		std::string id;
		std::string type;
		std::string controlTargetID;

		SDL_Surface* drawContext;

		Orientation* m_controlTarget;
		Viewer* camera;
		Scene* activeScene;
	
		double lastItFact;

		std::list<ZazenGraphicsEntity*> entities;

		bool initSDL();
		bool initGL();

		void loadControlConfig( TiXmlElement* );
		void loadCameraConfig( TiXmlElement* );
		bool loadGeomClasses( TiXmlElement* );
};

#endif /* ZAZENGRAPHICS_H_ */
