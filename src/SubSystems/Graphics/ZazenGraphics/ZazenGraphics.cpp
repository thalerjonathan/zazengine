/*
 * ZazenGraphics.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "ZazenGraphics.h"

#include <GL/glew.h>

#include "Renderer/Model.h"
#include "Renderer/geom/GeomPlane.h"
#include "Renderer/geom/GeomSphere.h"
#include "Renderer/geom/GeomTeapot.h"

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

	cout << "Initializing SDL..." << endl;
	int error = SDL_Init(SDL_INIT_EVERYTHING);
	if (error != 0) {
		cout << "FAILED ... Initializing SDL failed - exit..." << endl;
		return false;
	} else {
		cout << "OK ... SDL initialized" << endl;
	}
	
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1) {
		cout << "FAILED ... SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER) failed with " << SDL_GetError() << endl;
		return false;
	}
	
	this->drawContext = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/);
	if (this->drawContext == 0) {
		cout << "FAILED ... SDL_SetVideoMode failed with " << SDL_GetError() << endl;
		return false;
	}
	
	cout << "OK ... Videocontext created" << endl;
	
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "ERROR ... GLEW failed with " <<  glewGetErrorString(err) << endl;
		return false;
	} else {
		cout << "OK ... GLEW " << glewGetString(GLEW_VERSION) << " initialized " << endl;
	}
	
	if (!GLEW_VERSION_2_0) {
		cout << "ERROR ... OpenGL not version 2.0 - exit..." << endl;
		return false;
	} else {
		cout << "OK ... OpenGL 2.0 supported" << endl;
	}
	
	if (!GLEW_ARB_vertex_buffer_object) {
		cout << "ERROR ... GL_ARB_vertex_buffer_object not supported - exit..." << endl;
		return false;
	} else {
		cout << "OK ... GL_ARB_vertex_buffer_object supported" << endl;
	}
	
	if (!GLEW_ARB_vertex_program) {
		cout << "ERROR ... GL_ARB_vertex_program not supported - exit..." << endl;
		return false;
	} else {
		cout << "OK ... GL_ARB_vertex_program supported" << endl;
	}
	
	if (!GLEW_ARB_fragment_program) {
		cout << "ERROR ... GL_ARB_fragment_program not supported - exit..." << endl;
		return false;
	} else {
		cout << "OK ... GL_ARB_fragment_program supported" << endl;
	}
	
	if (Material::loadMaterials() == false) {
		cout << "FAILED ... loading Materials" << endl;
		return false;
	}
	
	this->camera = new Camera(45.0, WINDOW_WIDTH, WINDOW_HEIGHT);
	this->camera->setPosition(0, 0, 20);
	this->camera->changeHeading(0);
	
	this->activeScene = new Scene( "NullScene", this->camera );

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

	delete this->activeScene;
	this->activeScene = 0;

	SDL_Quit();
	
	Material::freeAll();
	Texture::freeAll();
	
	cout << "================ ZazenGraphics shutdown =================" << endl;

	return true;
}

bool
ZazenGraphics::start()
{
	this->activeScene->load(false, 0, 0, 0);

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
	//cout << "ZazenGraphics::process" << endl;

	this->activeScene->processFrame( iterationFactor );

	return true;
}

bool
ZazenGraphics::finalizeProcess()
{
	//cout << "ZazenGraphics::finalizeProcess()" << endl;

	return true;
}

bool
ZazenGraphics::sendEvent(const Event& e)
{
	return true;
}

ZazenGraphicsEntity*
ZazenGraphics::createEntity( TiXmlElement* objectNode )
{
	ZazenGraphicsEntity* entity = new ZazenGraphicsEntity();

	TiXmlElement* instanceNode = objectNode->FirstChildElement( "instance" );
	if ( 0 == instanceNode )
	{
		cout << "ERROR ... no instance-node found for graphicsinstance - ignoring object" << endl;
		delete entity;
		return 0;
	}

	Scene::EntityInstance instance;

	const char* str = instanceNode->Attribute( "class" );
	if ( 0 != str )
	{
		instance.entity = str;
	}

	str = instanceNode->Attribute( "x" );
	if ( 0 != str )
	{
		instance.position.data[0] = atof( str );
	}

	str = instanceNode->Attribute( "y" );
	if ( 0 != str )
	{
		instance.position.data[1] = atof( str );
	}

	str = instanceNode->Attribute( "z" );
	if ( 0 != str )
	{
		instance.position.data[2] = atof( str );
	}

	instance.size = 1.0;

	this->activeScene->addInstance( instance );

	return entity;
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

		if ( 0 == strcmp( str, "geomClass" ) )
		{
			Scene::Entity entity;

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

			str = classNode->Attribute( "material" );
			if ( 0 != str )
			{
				// is optinal
				entity.material = str;
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
					GeomSphere* sphere = new GeomSphere( 1 );
					Model::registerGeom( sphere, entity.name );
				}
				else if ( "TEAPOT" == geomType )
				{
					GeomTeapot* teapot = new GeomTeapot( 1 );
					Model::registerGeom( teapot, entity.name );
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

					GeomPlane* plane = new GeomPlane( length, width );
					Model::registerGeom( plane, entity.name );
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
