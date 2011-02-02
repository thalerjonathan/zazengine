#include <GL/glew.h>
#include <SDL/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "Renderer.h"

#include <algorithm>
#include <iostream>

using namespace std;

Renderer::Renderer( Camera& camera, std::string& skyBoxFolder )
	: camera( camera )
{
	this->frame = 1;

	this->skyBox = 0;
	
	if ( "" != skyBoxFolder )
		this->skyBox = new GeomSkyBox(this->camera, skyBoxFolder);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glCullFace(GL_FRONT);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[] = { 0.0f, 60.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One
}

Renderer::~Renderer()
{
}
