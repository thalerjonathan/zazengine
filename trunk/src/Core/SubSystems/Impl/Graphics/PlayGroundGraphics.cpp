/*
 * PlayGroundGraphics.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "PlayGroundGraphics.h"

#include <GL/glew.h>

#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;

PlayGroundGraphics::PlayGroundGraphics()
	: id ("PlayGroundGraphics"),
	  type ("renderer")
{

}

PlayGroundGraphics::~PlayGroundGraphics()
{

}

bool
PlayGroundGraphics::initialize()
{
	cout << endl << "=============== PlayGroundGraphics initializing... ===============" << endl;
	
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
	
	cout << "================ PlayGroundGraphics initialized =================" << endl;
	
	return true;
}

bool
PlayGroundGraphics::shutdown()
{
	cout << endl << "=============== PlayGroundGraphics shutting down... ===============" << endl;

	SDL_Quit();
	
	Material::freeAll();
	Texture::freeAll();
	
	cout << "================ PlayGroundGraphics shutdown =================" << endl;

	return true;
}

bool
PlayGroundGraphics::start()
{
	return true;
}

bool
PlayGroundGraphics::stop()
{
	return true;
}

bool
PlayGroundGraphics::pause()
{
	return true;
}

bool
PlayGroundGraphics::process(double iterationFactor)
{
	cout << "PlayGroundGraphics::process" << endl;

	//this->activeScene->processFrame( iterationFactor );

	return true;
}

bool
PlayGroundGraphics::finalizeProcess()
{
	cout << "PlayGroundGraphics::finalizeProcess()" << endl;

	return true;
}

bool
PlayGroundGraphics::sendEvent(const Event& e)
{
	return true;
}

PlayGroundGraphicsEntity*
PlayGroundGraphics::createEntity()
{
	return 0;
}
