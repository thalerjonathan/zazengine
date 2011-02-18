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

#include "Renderer/DRRenderer.h"

#include "../../../Core/Core.h"

#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;

ZazenGraphics::ZazenGraphics()
	: m_id ("ZazenGraphics"),
	  m_type ("graphics")
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
	
	this->m_renderer = new DRRenderer();
	if ( false == this->m_renderer->initialize() )
	{
		cout << "ERROR ... initializing renderer failed - exit" << endl;
		return false;
	}

	if ( false == this->loadGeomClasses( configNode ) )
	{
		return false;
	}

	cout << "================ ZazenGraphics initialized =================" << endl;
	
	return true;
}

bool
ZazenGraphics::shutdown()
{
	cout << endl << "=============== ZazenGraphics shutting down... ===============" << endl;

	Core::getInstance().getEventManager().unregisterForEvent( "SDLK_RIGHT", this );

	std::list<ZazenGraphicsEntity*>::iterator iter = this->m_entities.begin();
	while ( iter != this->m_entities.end() )
	{
		ZazenGraphicsEntity* entity = *iter++;
		delete entity;
	}

	this->m_entities.clear();

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
	return true;
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

	// process events of m_entities
	std::list<ZazenGraphicsEntity*>::iterator iter = this->m_entities.begin();
	while ( iter != this->m_entities.end() )
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

	this->m_lastItFact = iterationFactor;
	flag = this->m_renderer->renderFrame( this->m_instances );

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
	return true;
}

ZazenGraphicsEntity*
ZazenGraphics::createEntity( TiXmlElement* objectNode, IGameObject* parent )
{
	ZazenGraphicsEntity* entity = new ZazenGraphicsEntity( parent );

	TiXmlElement* instanceNode = objectNode->FirstChildElement( "instance" );
	if ( instanceNode )
	{
		glm::vec3 v;

		Instance* instance = new Instance();

		const char* str = instanceNode->Attribute( "class" );
		if ( 0 != str )
		{
			instance->geom = GeometryFactory::get( str );
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

		instance->id = parent->getName();
		instance->set( v, 0, 0, 0 );

		this->m_instances.push_back( instance );
	}

	this->m_entities.push_back( entity );

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

	this->m_drawContext = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/);
	if ( 0 == this->m_drawContext )
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
			string classID;
			string modelType;

			str = classNode->Attribute( "id" );
			if ( 0 == str )
			{
				cout << "ERROR ... id attribute missing in geomClass - ignoring" << endl;
				continue;
			}
			else
			{
				classID = str;
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
				std::string meshFile;

				str = classNode->Attribute( "file" );
				if ( 0 == str )
				{
					cout << "ERROR ... file attribute missing in geomClass - ignoring" << endl;
					continue;
				}
				else
				{
					meshFile = str;
				}

				GeometryFactory::loadMesh( classID, meshFile );
			}
		}
	}

	return true;
}


void
ZazenGraphics::loadCameraConfig( TiXmlElement* configNode )
{
	float fov = 90.0f;
	std::string mode = "PROJ";
	glm::vec3 pos;
	float roll = 0.0f;
	float pitch = 0.0f;
	float heading = 0.0f;

	TiXmlElement* cameraNode = configNode->FirstChildElement( "camera" );
	if ( 0 == cameraNode )
	{
		cout << "INFO ... no cameraNode defined - using defaults" << endl;
	}
	else
	{
		TiXmlElement* viewingNode = cameraNode->FirstChildElement( "viewing" );
		if ( 0 == viewingNode )
		{
			cout << "ERROR ... no viewingNode defined - using defaults" << endl;
		}
		else
		{
			const char* str = viewingNode->Attribute( "fov" );
			if ( 0 == str )
			{
				cout << "INFO ... fov attribute missing in viewingNode - use default " << endl;
			}
			else
			{
				fov = atof( str );
			}
		}

		TiXmlElement* orientationNode = cameraNode->FirstChildElement( "orientation" );
		if ( 0 == orientationNode )
		{
			cout << "ERROR ... no orientationNode defined - using defaults" << endl;
		}
		else
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			const char* str = orientationNode->Attribute( "x" );
			if ( 0 == str )
			{
				cout << "INFO ... x attribute missing in orientation - use default " << endl;
			}
			else
			{
				x = atof( str );
			}

			str = orientationNode->Attribute( "y" );
			if ( 0 == str )
			{
				cout << "INFO ... y attribute missing in orientation - use default " << endl;
			}
			else
			{
				y = atof( str );
			}

			str = orientationNode->Attribute( "z" );
			if ( 0 == str )
			{
				cout << "INFO ... z attribute missing in orientation - use default " << endl;
			}
			else
			{
				z = atof( str );
			}

			str = orientationNode->Attribute( "heading" );
			if ( 0 == str )
			{
				cout << "INFO ... heading attribute missing in orientation - use default " << endl;
			}
			else
			{
				heading = atof( str );
			}

			str = orientationNode->Attribute( "roll" );
			if ( 0 == str )
			{
				cout << "INFO ... roll attribute missing in orientation - use default " << endl;
			}
			else
			{
				roll = atof( str );
			}

			str = orientationNode->Attribute( "pitch" );
			if ( 0 == str )
			{
				cout << "INFO ... pitch attribute missing in orientation - use default " << endl;
			}
			else
			{
				pitch = atof( str );
			}

			pos = glm::vec3( x, y, z );
		}
	}

	this->m_camera = new Viewer( WINDOW_WIDTH, WINDOW_HEIGHT );

	if ( mode == "PROJ" )
	{
		this->m_camera->setFov( 90 );
		this->m_camera->setupPerspective();
	}
	else if ( mode == "ORTHO" )
	{
		this->m_camera->setupOrtho();
	}


	this->m_camera->set( pos, pitch, heading, roll );
}
