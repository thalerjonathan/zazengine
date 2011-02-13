/*
 * ZazenGraphics.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "ZazenGraphics.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Geometry/GeometryFactory.h"
#include "Material/Texture.h"

#include "../../../Core/Core.h"

#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;

ZazenGraphics::ZazenGraphics()
	: id ("ZazenGraphics"),
	  type ("graphics")
{

}

ZazenGraphics::~ZazenGraphics()
{

}

bool
ZazenGraphics::initialize( TiXmlElement* configNode )
{
	cout << endl << "=============== ZazenGraphics initializing... ===============" << endl;

	if ( false == this->initSDL() )
	{
		return false;
	}

	if ( false == this->initGL() )
	{
		return false;
	}
	
	/*
	if ( Material::loadMaterials() == false )
	{
		cout << "FAILED ... loading Materials" << endl;
		return false;
	}
	*/

	/*
	TiXmlElement* skyBoxNode = configNode->FirstChildElement( "skyBox" );
	if ( skyBoxNode )
	{
		string folder;

		const char* str = skyBoxNode->Attribute( "folder" );
		if (str != 0)
			folder = str;

		this->activeScene->setSkyBoxFolder( folder );
	}
	else
	{
		cout << "INFO ... scene hast no Skybox defined" << endl;
	}
*/

	this->camera = new Viewer( 45.0, WINDOW_WIDTH, WINDOW_HEIGHT );
	this->camera->setPositionInv( glm::vec3( 0.0, 10, 150 ) );

	this->activeScene = new Scene( "NullScene", this->camera );

	if ( false == this->loadCameraConfig( configNode ) )
	{
		return false;
	}

	if ( false == this->loadGeomClasses( configNode ) )
	{
		return false;
	}


	Core::getInstance().getEventManager().registerForEvent( "SDLK_RIGHT", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_LEFT", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_UP", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_DOWN", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_w", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_s", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_d", this );
	Core::getInstance().getEventManager().registerForEvent( "SDLK_a", this );

	cout << "================ ZazenGraphics initialized =================" << endl;
	
	return true;
}

bool
ZazenGraphics::shutdown()
{
	cout << endl << "=============== ZazenGraphics shutting down... ===============" << endl;

	Core::getInstance().getEventManager().unregisterForEvent( "SDLK_RIGHT", this );

	std::list<ZazenGraphicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ZazenGraphicsEntity* entity = *iter++;
		delete entity;
	}

	this->entities.clear();

	delete this->activeScene;
	this->activeScene = 0;

	SDL_Quit();

	//Material::freeAll();
	Texture::freeAll();
	GeometryFactory::freeAll();
	
	cout << "================ ZazenGraphics shutdown =================" << endl;

	return true;
}

bool
ZazenGraphics::start()
{
	return this->activeScene->load();
}

bool
ZazenGraphics::stop()
{
	return true;
}

bool
ZazenGraphics::pause()
{
	return true;
}

bool
ZazenGraphics::process( double iterationFactor )
{
	bool flag = true;
	//cout << "ZazenGraphics::process enter" << endl;

	// process events of entities
	std::list<ZazenGraphicsEntity*>::iterator iter = this->entities.begin();
	while ( iter != this->entities.end() )
	{
		ZazenGraphicsEntity* entity = *iter++;

		std::list<Event>::iterator eventsIter = entity->queuedEvents.begin();
		while ( eventsIter != entity->queuedEvents.end() )
		{
			//Event& e = *eventsIter++;

			//cout << "received Event '" << e.getID() << "' in ZazenGraphics from GO '" << entity->getParent()->getName() << endl;
		}

		entity->queuedEvents.clear();
	}

	this->lastItFact = iterationFactor;
	flag = this->activeScene->processFrame( iterationFactor );

	//cout << "ZazenGraphics::process leave" << endl;

	return flag;
}

bool
ZazenGraphics::finalizeProcess()
{
	//cout << "ZazenGraphics::finalizeProcess" << endl;

	return true;
}

bool
ZazenGraphics::sendEvent( Event& e )
{
	if ( e == "SDLK_RIGHT" )
	{
		this->camera->changeHeadingInv( 0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_LEFT" )
	{
		this->camera->changeHeadingInv( -0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_UP" )
	{
		this->camera->changePitchInv( 0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_DOWN" )
	{
		this->camera->changePitchInv( -0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_w" )
	{
		this->camera->strafeForwardInv( 0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_s" )
	{
		this->camera->strafeForwardInv( -0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_d" )
	{
		this->camera->changeRollInv( 0.1 * this->lastItFact );
	}
	else if ( e == "SDLK_a" )
	{
		this->camera->changeRollInv( -0.1 * this->lastItFact );
	}

	return true;
}

ZazenGraphicsEntity*
ZazenGraphics::createEntity( TiXmlElement* objectNode, IGameObject* parent )
{
	ZazenGraphicsEntity* entity = new ZazenGraphicsEntity( parent );

	TiXmlElement* instanceNode = objectNode->FirstChildElement( "instance" );
	if ( 0 == instanceNode )
	{
		cout << "ERROR ... no instance-node found for graphicsinstance - ignoring object" << endl;
		delete entity;
		return 0;
	}

	glm::vec3 v;
	entity->instance = new Scene::InstanceDefinition();
	entity->instance->modelMatrix = new glm::mat4();

	const char* str = instanceNode->Attribute( "class" );
	if ( 0 != str )
	{
		entity->instance->entity = str;
	}

	str = instanceNode->Attribute( "x" );
	if ( 0 != str )
	{
		v[ 0 ] = atof( str );
	}

	str = instanceNode->Attribute( "y" );
	if ( 0 != str )
	{
		v[ 1 ] = atof( str );
	}

	str = instanceNode->Attribute( "z" );
	if ( 0 != str )
	{
		v[ 2 ] = atof( str );
	}

	entity->instance->size = 1.0;

	*entity->instance->modelMatrix = glm::translate( glm::mat4(1.0f), v );

	this->activeScene->addInstance( entity->instance );

	this->entities.push_back( entity );

	return entity;
}


bool
ZazenGraphics::initSDL()
{
	cout << "Initializing SDL..." << endl;
	int error = SDL_Init(SDL_INIT_VIDEO);
	if (error != 0)
	{
		cout << "FAILED ... Initializing SDL failed - exit..." << endl;
		return false;
	}
	else
	{
		cout << "OK ... SDL initialized" << endl;
	}

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
	{
		cout << "FAILED ... SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER) failed with " << SDL_GetError() << endl;
		return false;
	}

	this->drawContext = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/);
	if ( 0 == this->drawContext )
	{
		cout << "FAILED ... SDL_SetVideoMode failed with " << SDL_GetError() << endl;
		return false;
	}

	cout << "OK ... Videocontext created" << endl;

	return true;
}

bool
ZazenGraphics::initGL()
{
	//int argc = 0;
	//glutInit(&argc, NULL);

	int major, minor;
	GLenum err = glewInit();
	if ( GLEW_OK != err )
	{
		cout << "ERROR ... GLEW failed with " <<  glewGetErrorString(err) << endl;
		return false;
	}
	else
	{
		cout << "OK ... GLEW " << glewGetString(GLEW_VERSION) << " initialized " << endl;
	}

	cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

	glGetIntegerv(GL_MAJOR_VERSION, &major); // major = 3
	glGetIntegerv(GL_MINOR_VERSION, &minor); // minor = 2

	// opengl 3.3 minimum
	if ( 3 > major )
	{
		cout << "ERROR ... OpenGL3.3 or above required" << endl;
		return false;
	}
	else if ( 3 == major )
	{
		if ( 3 > minor )
		{
			cout << "ERROR ... OpenGL3.3 or above required" << endl;
			return false;
		}
	}

	return true;
}

bool
ZazenGraphics::loadGeomClasses( TiXmlElement* configNode )
{
	TiXmlElement* geomClassesNode = configNode->FirstChildElement( "geomClasses" );
	if ( 0 == geomClassesNode )
	{
		cout << "ERROR ... no geomClasses defined - exit" << endl;
		return false;
	}

	for (TiXmlElement* classNode = geomClassesNode->FirstChildElement(); classNode != 0; classNode = classNode->NextSiblingElement())
	{
		const char* str = classNode->Value();
		if ( 0 == str )
			continue;

		if ( 0 == strcmp( str, "class" ) )
		{
			Scene::EntityDefinition entity;

			string modelType;

			str = classNode->Attribute( "id" );
			if ( 0 == str )
			{
				cout << "ERROR ... id attribute missing in geomClass - ignoring" << endl;
				continue;
			}
			else
			{
				entity.name = str;
			}

			str = classNode->Attribute( "modelType" );
			if ( 0 == str )
			{
				cout << "ERROR ... modelType attribute missing in geomClass - ignoring" << endl;
				continue;
			}
			else
			{
				modelType = str;
			}

			if ( "BUILTIN" == modelType)
			{
				string geomType;
				str = classNode->Attribute( "geomType" );
				if ( 0 == str )
				{
					cout << "ERROR ... geomType attribute missing in geomClass - ignoring" << endl;
					continue;
				}
				else
				{
					geomType = str;
				}

				if ( "SPHERE" == geomType )
				{
//					GeomSphere* sphere = new GeomSphere( 1 );
//					GeometryFactory::registerGeom( sphere, entity.name );
				}
				else if ( "TEAPOT" == geomType )
				{
//					GeomTeapot* teapot = new GeomTeapot( 1 );
//					GeometryFactory::registerGeom( teapot, entity.name );
				}
				else if ( "BOX" == geomType )
				{
//					GeomBox* box = new GeomBox( 1 );
//					GeometryFactory::registerGeom( box, entity.name );
				}
				else if ( "PLANE" == geomType )
				{
					float length = 200;
					float width = 400;

					str = classNode->Attribute("length");
					if (str != 0)
						length = atof(str);

					str = classNode->Attribute("width");
					if (str != 0)
						width = atof(str);

//					GeomPlane* plane = new GeomPlane( length, width );
//					GeometryFactory::registerGeom( plane, entity.name );
				}
			}
			else if ( "MESH" == modelType)
			{
				str = classNode->Attribute( "file" );
				if ( 0 == str )
				{
					cout << "ERROR ... file attribute missing in geomClass - ignoring" << endl;
					continue;
				}
				else
				{
					entity.modelFile = str;
				}
			}

			this->activeScene->addEntity( entity );
		}
	}

	return true;
}

bool
ZazenGraphics::loadCameraConfig( TiXmlElement* configNode )
{
	return true;
}
